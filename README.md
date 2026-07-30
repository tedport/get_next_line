*This project has been created as part of the 42 curriculum by vtarasov*

# get_next_line

*This project was created as part of the 42 curriculum.*

## Description

`get_next_line` is a function that reads from a file descriptor and
returns one line per call. It preserves unread data between calls,
allowing sequential line-by-line reading without loading the entire file
into memory.

### Features

-   Reads one line at a time.
-   Supports multiple file descriptors simultaneously (bonus).
-   Uses a persistent internal stash implemented as a linked list.
-   Allows the read buffer size to be configured through `BUFFER_SIZE`.

### Goals

-   Understand static storage duration.
-   Learn how low-level I/O with `read()` works.
-   Manage memory manually while preserving state across function calls.

## Algorithm

Because `read()` returns arbitrary-sized chunks, the implementation must
preserve unread bytes between calls. This project stores those bytes in
a linked list of fixed-size nodes.

``` c
typedef struct s_fdlist
{
    struct s_fdlist *next;
    int             fd;
    char            content[BUFFER_SIZE + 1];
} t_fdlist;
```

Each node stores a single chunk read from one file descriptor. The list
head is held in a static variable inside `get_next_line()`, allowing
state to persist across calls while remaining invisible to the caller.

### Workflow

1.  Check whether the stash already contains a newline. If so, no
    `read()` call is performed.
2.  Otherwise, keep reading fixed-size chunks until either a newline or
    EOF is reached.
3.  Extract the next line by concatenating only the required bytes.
4.  Free fully consumed nodes.
5.  Shift any remaining bytes after the newline to the beginning of the
    first remaining node.

This separation keeps reading, extraction, cleanup, and leftover
preservation independent and easy to reason about.

## Design decisions

-   **Linked list of fixed-size buffers** --- avoids repeatedly resizing
    a growing buffer while naturally supporting multiple file
    descriptors.
-   **Static head pointer** --- preserves state between calls without
    exposing global variables.
-   **Early termination after `'\n'`** --- avoids reading more data than
    necessary.
-   **Pre-check before `read()`** --- skips unnecessary system calls
    when a complete line is already buffered.
-   **Zero-byte `read(fd, ..., 0)` validation** --- provides a portable
    way to validate the file descriptor before processing.

## Complexity

  Operation                         Complexity
  ------------------- ------------------------
  Reading                  O(number of chunks)
  Extracting a line             O(line length)
  Cleanup                    O(consumed nodes)
  Memory usage          O(longest unread line)

## Usage

``` bash
gcc main.c ./get_next_line/get_next_line.c \
./get_next_line/get_next_line_utils.c \
-I./get_next_line
```

Override the buffer size at compile time:

``` bash
gcc main.c ./get_next_line/get_next_line.c \
./get_next_line/get_next_line_utils.c \
-I./get_next_line \
-DBUFFER_SIZE=42
```

## Testing

``` bash
git clone https://github.com/tedport/gnlTester.git
cd gnlTester
uv run pytest -s -v
```

For the bonus version:

``` bash
uv run pytest -s -v --bonus
```

## Resources

-   `man 2 read`
-   `man 2 open`
-   `man 3 malloc`
-   `man 3 free`

## AI Usage

AI was used to help generate parts of the test suite and to improve the
project's documentation.