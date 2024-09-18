# kit - File Concatenation Utility

## Description

`kit` is a simple command-line utility that concatenates and copies files. It's similar to the standard UNIX `cat` command but with some additional features.

## Usage

'''bash
kit [-o outfile] infile1 [...infile2....]
kit [-o outfile]
'''

## Features

- Concatenate multiple files
- Read from standard input using `-`
- Write output to a file or standard output
- Handle binary files
- Process large files efficiently

## Building

To compile the program, simply run:

'''bash
make
'''

This will produce the `kit` executable.

## Examples

1. Concatenate two files:

'''bash
./kit file1.txt file2.txt
'''

2. Use standard input with files:

'''bash
echo 'Standard input' | ./kit file1.txt - file2.txt
'''
3. Output to a file:

'''bash
./kit -o output.txt file1.txt file2.txt
'''

4. Read only from standard input:

'''bash
./kit
'''

## Error Handling

`kit` provides appropriate error messages for common issues such as file not found or permission denied. It returns 0 on success and -1 on error.

## Limitations

- The program uses a fixed buffer size of 4096 bytes for reading and writing.
- It does not support in-place editing of files.

## Testing

A test script `test_kit.sh` is provided to verify the functionality of `kit`. Run it using:

'''bash
./test_kit.sh
'''

The test results will be saved in `kit_test_results.txt`.


### Large File Handling Test

The repository includes `input.txt`, a ChatGPT-generated passage of 8886 words. This file is used to test `kit`'s ability to handle files larger than its 4096-byte buffer, ensuring proper functionality with larger inputs.

To test large file handling:

'''bash
./kit input.txt | wc -c
'''

This command should output 8886, confirming that `kit` correctly processes the entire file.