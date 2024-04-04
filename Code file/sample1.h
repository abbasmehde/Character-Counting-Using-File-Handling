// sample1.h
#ifndef B_IO_H
#define B_IO_H

#define B_CHUNK_SIZE 512

// Function prototypes for buffered I/O functions
typedef int b_io_fd;


b_io_fd b_open(char *filename,int flags);
int b_read(b_io_fd fd, char *buffer, int count);
void b_close(b_io_fd fd);

// Assume GetFileInfo and LBAread functions are provided elsewhere

#endif /* B_IO_H */

