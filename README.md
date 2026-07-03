# oshell

`oshell` is a small Unix-like shell written in C. It reads a command line, tokenizes it, and uses `fork()` plus `execvp()` to run external programs. It also supports background execution with `&` and a simple command history replay feature with `!!`.

## Features

- Run normal shell commands such as `ls`, `pwd`, `cat`, or any other program available on your `PATH`
- Launch jobs in the background by ending a command with `&`
- Replay the previous command with `!!`
- Save command history to `.osh_history`

## Project Layout

- [unix_shell.c](unix_shell.c) - main shell loop, prompt handling, process creation, and built-in commands
- [parser.c](parser.c) - tokenization helper used to split a command line into arguments
- [history.c](history.c) - in-memory session history and persistence to `.osh_history`
- [parser.h](parser.h) - parser interface and input size limit
- [history.h](history.h) - history interface and shared session state
- [Makefile](Makefile) - build targets for compiling, running, and cleaning the project

## Requirements

- A C toolchain with `gcc`
- `make`
- A Unix-like environment

## Build

From the project directory:

```bash
make
```

This produces the `unix_shell` executable.

## Run

Start the shell with:

```bash
make run
```

Or run the binary directly after building:

```bash
./unix_shell
```

You should see the `osh>` prompt.

## Usage

Type a command and press Enter:

```text
osh> ls -l
osh> pwd
osh> sleep 5 &
```

Background commands return immediately, and the shell prints the background process ID. When a background job finishes, the shell reports it on the next prompt cycle.

### Built-ins

- `exit` closes the shell
- `!!` reruns the most recent command in the current session

## History File

The shell keeps a session history buffer in memory and flushes it to `.osh_history` in the project directory. The file is overwritten as the shell runs, so it always reflects the most recently recorded session history.

## Notes

- Input is limited by `MAX_LINE` in [parser.h](parser.h), which is currently set to 80 characters
- The shell stores up to `SIZE` bytes of history in memory
- `!!` only works after at least one command has been recorded

## Clean

Remove build outputs with:

```bash
make clean
```