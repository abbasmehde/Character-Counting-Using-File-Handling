// sample2.c
#include "sample1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
// Function to retrieve file information
int GetFileInfo(char *filename, int *start_block, int *file_size) {
    // Open the file
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return -1; // Error opening file
    }

    // Use fstat to get file information
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        close(fd);
        return -1; // Error getting file info
    }

    // Close the file
    close(fd);

    // Populate start_block and file_size with relevant information
    *start_block = 0; // For simplicity, assuming start block is always 0
    *file_size = file_stat.st_size;

    return 0; // Success
}

// Function to read blocks of data from a file
int LBAread(int fd, int start_block, char *buffer, int num_blocks) {
    // Seek to the start block
    off_t offset = start_block * B_CHUNK_SIZE;
    if (lseek(fd, offset, SEEK_SET) == -1) {
        return -1; // Error seeking in file
    }

    // Read data into buffer
    ssize_t bytes_read = read(fd, buffer, num_blocks * B_CHUNK_SIZE);
    if (bytes_read == -1) {
        return -1; // Error reading from file
    }

    return bytes_read; // Return number of bytes read
}

#define B_CHUNK_SIZE 512

// Structure to store file information and internal buffer
typedef struct {
  int fd;
  int start_block;
  int file_size;
  char *buffer;
  int bytes_in_buffer;
  int current_block;
} b_io_file_info;

b_io_fd b_open(char *filename, int flags) {
  b_io_file_info *info = (b_io_file_info *)malloc(sizeof(b_io_file_info));
  if (info == NULL) {
    return (b_io_fd)-1; // Error allocating memory
  }

  int start_block, file_size;
  if (GetFileInfo(filename, &start_block, &file_size) == -1) {
    free(info);
    return (b_io_fd)-1; // Error getting file info
  }

  // Open the file using the provided flags
  info->fd = open(filename, flags);
  if (info->fd == -1) {
    free(info);
    return (b_io_fd)-1; // Error opening file
  }

  info->start_block = start_block;
  info->file_size = file_size;
  info->buffer = (char*)(malloc(B_CHUNK_SIZE));
  if (info->buffer == NULL) {
    close(info->fd);
    free(info);
    return (b_io_fd)-1; // Error allocating buffer
  }
  info->bytes_in_buffer = 0;
  info->current_block = start_block;

  return (b_io_fd)(intptr_t)info; // Returning pointer directly, without cast
}


int b_read(b_io_fd fd, char *buffer, int count) {
  b_io_file_info *info = (b_io_file_info *)fd;
  int total_read = 0;
  while (count > 0) {
    int bytes_to_read = B_CHUNK_SIZE - info->bytes_in_buffer;
    if (bytes_to_read > count) {
      bytes_to_read = count;
    }

    // Check if we need to refill the buffer
    if (info->bytes_in_buffer == 0) {
      int blocks_to_read = (bytes_to_read + B_CHUNK_SIZE - 1) / B_CHUNK_SIZE;
      int bytes_read = LBAread(info->fd, info->current_block, info->buffer, blocks_to_read);
      if (bytes_read == -1) {
        return -1; // Error reading from disk
      }
      info->bytes_in_buffer = bytes_read;
      info->current_block += blocks_to_read;
      if (info->bytes_in_buffer == 0) {
        return total_read; // Reached end of file
      }
    }

    // Copy data from internal buffer to caller's buffer
    memcpy(buffer, info->buffer + info->bytes_in_buffer - bytes_to_read, bytes_to_read);
    info->bytes_in_buffer -= bytes_to_read;
    buffer += bytes_to_read;
    count -= bytes_to_read;
    total_read += bytes_to_read;
  }

  return total_read;
}

void b_close(b_io_fd fd) {
  b_io_file_info *info = (b_io_file_info *)fd;
  // Check for successful opening before closing and freeing
  if (info->fd != -1) {
    close(info->fd);
  }
  free(info->buffer);
  free(info);
}
