# Operating-System

This repository contains multiple projects, each demonstrating different aspects of system programming and utility development.

## Projects

1. **kit - File Concatenation Utility**
   - Simple command-line utility for concatenating and copying files
   - Similar to the standard UNIX `cat` command with additional features
   - Handles binary files and large file processing efficiently
   - Supports input/output redirection
   - See `kit/README.md` for detailed documentation

2. **filesystem - Recursive Filesystem Statistics**
   - Analyzes filesystem structure and provides detailed statistics
   - Reports file types, sizes, and potential issues
   - Handles symlinks, hard links, and special files
   - Identifies problematic filenames
   - See `filesystem/README.md` for complete documentation

3. **shell - Custom Shell Implementation**
   - Basic Unix-like shell with built-in commands
   - Supports file redirections and process execution
   - Provides detailed process statistics
   - Handles command parsing and execution
   - See `shell/README.md` for usage and features

## Getting Started

Each project has its own directory with a dedicated README file containing specific instructions for building, using, and testing the project.

### Building All Projects

```bash
# Build all projects
make all

# Build specific project
make kit
make filesystem
make shell
```


## Project Structure

```
.
├── kit/
│   ├── kit.c
│   └── README.md
├── filesystem/
│   ├── filesystem.c
│   ├── filesystem.h
│   └── README.md
├── shell/
│   ├── builtins.c
│   ├── executor.c
│   ├── main.c
│   ├── parser.c
│   └── README.md
└── README.md
```
