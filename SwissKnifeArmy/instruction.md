Problem 3 -- Swiss Army Knife
A basic principle of UNIX is to have many small, simple tools, that can be interconnected through simple means,
such as pipes. In contrast, many other operating systems, such as Windows, like to have big, bulky, complicated
tools, with complicated and sometimes proprietary interfaces. In this assignment, we’re going to build a tool that by
all rights shouldn’t exist in UNIX because we could more or less (no pun intended) build the same functionality from
three existing commands. I’ve added a few quirky features to justify combining them into a new tool, and which will
force you to use pipes and signal handlers.
The tool, which I call catgrepmore, is invoked with the following syntax:
catgrepmore pattern infile1 [...infile2...]
For each infilespecified:
1) open that file, report error and exit if you can’t open it
2) Establish two pipes and fork off two children, performing the proper I/O redirection for each child. The first child
is the grep command with patternas its sole argument, and the second is the more command with no arguments,
establishing a pipeline:
catgrepmore ===> grep pattern ===> more
In other words, the grep process has its standard input as the read side of pipe #1 and its standard output is the write
side of pipe #2. The more process has standard input from the read side of pipe #2 and standard output is not
modified. Your catgrepmore process writes to the write side of pipe #1.
3) In your catgrepmore program, use a simple loop to read from file you just opened in step (1) and write to the first
pipe. Use a read/write buffer of 4096 bytes. Check for and correctly handle partial (short writes). Continue until
end of file. Do not treat interrupted system calls in read or write to be fatal. Either make a decision based on
errno==EINTR, or use SA_RESTART.
4) After you have read the entire input file and copied it to the first pipe, 4K at a time, close the input file and close
the first pipe. This will cause grep to see an EOF once it has processed any input still in the first pipe, and in turn the
more program will see an EOF and let you know that. Both grep and more will exit on their own.
5) Before moving on to the next input file, use the wait system call twice to grab the exit status of the two children
that you had forked (the grep and the more processes). There is no need to report the exit status. You need to do the
two waits because otherwise zombie processes will pile up.
The intention is to display only those lines of the file that match the pattern, and to page through these results one
screen at a time. When the user exits the pager program (by pressing the q key), this will cause the more program to
terminate immediately. The grep program will die on a broken pipe #2, and your catgrepmore program will see a
broken pipe #1. This must not cause your catgrepmore program to die! It is time to move on to the next input file, if
any.
Your catgrepmore program must keep track of the total number of files and the total number of bytes processed so
far. Establish a signal handler so that sending a SIGUSR1 to your catgrepmore program will cause it to display this
information immediately, to standard error (in other words, do not print this into the pipe to grep!) After printing the
info, resume. The SIGUSR1 must not cause catgrepmore to die.
If a SIGUSR2 is sent to catgrepmore, this should cause it to immediately move on to the next input file (if any), as if
EOF was seen on the input file. This must happen even if the pipeline is blocked up waiting for the more process to
read input. Print a brief informative message to standard error, such as *** SIGUSR2 received, moving
on to file #5. The best way to implement this is with setjmp/longjmp or sigsetjmp/siglongjmp.
Be on the lookout: Setting up the pipes, and in particular avoiding dangling file descriptors, is important to proper
program operation. If your program terminates unexpectedly or gets stuck in an endless loop, this may be a symptom
of sloppy file descriptor work.
In order to properly test all aspects of your program, you must use sample files that are at least 64K long, so that
the pipe buffers can actually fill up. You must also test for proper behavior when the user quits the pager program, or
when you send SIGUSR1 or SIGUSR2 from another shell. Using the ps command, make sure that when your
program exits, under any circumstances, that the more and grep commands have also exited. If they get stuck, it is
probably because of dangling file descriptors.
Helpful note: In order for moreto receive keystrokes without interfering with reading from stdin, it opens a special
device /dev/tty which is your session terminal. Furthermore, to receive the keystrokes without waiting for a
newline character, more will put the ttydevice into a special mode. It may happen during development and testing
of this program that more exits without restoring the ttyto the normal mode, and your terminal session will appear
to be "crashed". Characters will not echo and nothing will seem to work. To get out of this mode, press Control-J
several times, then type the command stty saneand then press Control-J (NOT the Enter key!) again.