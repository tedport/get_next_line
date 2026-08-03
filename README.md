*This project has been created as part of the 42 curriculum by vtarasov*

# get_next_line
 
# Description
`get_next_line` is a function that reads a file pointed to by a file descriptor, line by line. 

### Features
- Reads one line at a time (including the trailing newline, if present).
- Can read from multiple file descriptors simultaneously (bonus version).
- Does not load the entire file into memory; it preserves unread data between calls.

### Goals
- Learn how to use static variables to preserve state across function calls.
- Learn low-level file I/O by reading a file line by line, advancing only as needed.
- Manage memory manually and implement custom standard library utilities.

## Algorithm Explanation
To read a file line by line, we obviously need somewhere to store our data. I decided to use a linked list for this to avoid `realloc`-style memory allocations. The linked list is defined as follows:

```c
typedef struct s_fdlist
{
	struct s_fdlist	*next;
	int				fd;
	char			content[BUFFER_SIZE + 1];
}					t_fdlist;
```

Each node holds a raw chunk read from a file descriptor, plus the `fd` it came from. The list head is kept in a static variable inside `get_next_line`, so it persists between calls. I used this approach because it was the simplest way I found to implement the logic without over-engineering.

**Read loop (`read_and_put`)**
While the last read returned more than 0 bytes:
- Allocate a new node (`fdlist_addnew`) and append it to the list.
- Call `read(fd, node->content, BUFFER_SIZE)`.
- Null-terminate the chunk at `content[bytes_read]`.
- If the chunk contains a `\n`, we stop because we already have a full line available.

**Line extraction (`extract_line`)**
Walk the list, and for every node whose `fd` matches, append its content to an accumulator string using `strjoin_rct`. The join stops as soon as a `\n` is reached, so a partial chunk after a newline is never copied into the returned line.

**Stash cleanup (`fdlist_clean_for_fd`)**
After the line has been extracted, all nodes belonging to `fd` that have been fully consumed are freed. The cleaning stops at the first node that still contains a newline, because the bytes that follow it must be preserved for the next call (this is the leftover of the stash).

**Shifting leftovers (`strip_pre_newline`)**
Once the consumed nodes are freed, the function finds the first remaining node for the `fd` containing a `\n` and shifts the bytes after the newline to the beginning of that node. This ensures the next call starts reading exactly at the beginning of the next line.

**Pre-check (`stash_has_newline`)**
Before issuing any `read()`, the function scans the stash for an existing newline. If one is present, no system call is made: the line is extracted directly from memory. This avoids useless I/O and is essential for correctness when the file has been fully read but a partial line still lives in the stash.

## Justification

Justification of the chosen algorithm:
- **Linked list of fixed-size nodes** — Simple to implement, avoids re-allocating a single growing buffer, and naturally supports the bonus part: multiple file descriptors share the same list, distinguished by the `fd` field. There is no need for a separate stash per `fd`.
- **Static head pointer** — Required by the 42 subject (no global variables); each `fd` retains its reading context across calls without exposing state to the user.
- **Early termination on `\n` during reading** — Avoids reading more bytes than strictly necessary to produce one line. This keeps memory usage proportional to a single line rather than to the whole file.
- **Cleanup + shift strategy** — Splitting the work into "free consumed nodes" and "shift leftover bytes" is more readable than a single combined operation and clearly expresses the two post-conditions that must hold after each call: (1) no node still holds a fully consumed line, (2) the first remaining node for this `fd` begins exactly at the start of the next line.
- **Pre-check before `read()`** — Guarantees that when a previous read fetched more than one line in a single chunk, no extra system call is performed. It also makes the function correct when the file descriptor points to a non-seekable stream (pipes, stdin) where re-reading is not possible.
- **`read(fd, &line, 0)` validation** — Detects invalid or closed file descriptors at the entry of the function in a portable way, without making any assumption about the underlying file type.
- **Custom string utilities** — Implements `cstrchr`, `strlen_ct`, and `strjoin_rct` to strictly adhere to 42's restrictions on standard library functions while safely calculating exact memory requirements.

# Instructions

## Usage
To use the function inside your project, include the `get_next_line` directory with `-I<path>` and add the source files. 

**Mandatory version:**
```bash
gcc -Wall -Wextra -Werror main.c ./get_next_line/get_next_line.c ./get_next_line/get_next_line_utils.c -I./get_next_line
```

**Bonus version (multiple file descriptors):**
```bash
gcc -Wall -Wextra -Werror main.c ./get_next_line/get_next_line_bonus.c ./get_next_line/get_next_line_utils_bonus.c -I./get_next_line
```

You can also change the amount of bytes read per call with `-DBUFFER_SIZE=<Desired size>`:

```bash
gcc main.c ./get_next_line/get_next_line.c ./get_next_line/get_next_line_utils.c -I./get_next_line -DBUFFER_SIZE=42
```

## Testing
You can download tests for the project:
```sh
git clone https://github.com/tedport/gnlTester.git
```
To use the tests, enter the tester directory and call:
```sh
uv run pytest -s -v
```
Or, to test the bonus part:
```sh
uv run pytest -s -v --bonus
```

# Resources
- `man 2 read`, `man 2 open`, `man 3 malloc`, `man 3 free` — official Linux manual pages.
- [Linked List Data Structure](https://www.geeksforgeeks.org/dsa/linked-list-data-structure/) - GeeksforGeeks.

## AI Usage
AI was used in the project to partially create testing suites and tailor this README file.