#include <stdio.h>
#include "sample1.h"
#include "sample2.c"
#include <fcntl.h>
#include <string.h>

#ifndef B_CHUNK_SIZE
#define B_CHUNK_SIZE 1024 // Adjust if needed based on assignment
#endif

int main(int argc, char *argv[]) {

  // Open the data file in read-only mode
  int fd = b_open("DATA.txt",  O_RDONLY);
  if (fd < 0) {
    printf("Error opening file: DATA (%d)\n", fd);
    return 1;
  }

  // Read data from file in a loop
  char buffer[B_CHUNK_SIZE];
  int bytes_read;
  do {
    bytes_read = b_read(fd, buffer, B_CHUNK_SIZE);
    if (bytes_read < 0) {
      printf("Error reading from file: DATA\n");
      break;
    }

    // Handle full buffer or end of file
    if (bytes_read > 0) {
      // Print read data
      fwrite(buffer, sizeof(char), bytes_read, stdout);
    }
  } while (bytes_read == B_CHUNK_SIZE); // Loop until less than chunk size is read

  // Close the file
  b_close(fd);

  // Open and read data from DecOfInd.txt
  FILE *file1 = fopen("DecOfInd.txt", "r");
  if (file1 == NULL) {
    printf("Error opening file: DecOfInd.txt\n");
    return 1;
  }
  fseek(file1, 0, SEEK_END);
  long file_size1 = ftell(file1);
  fseek(file1, 0, SEEK_SET);
  char *DecOfInd_txt = (char *)malloc(file_size1 + 1);
  if (DecOfInd_txt == NULL) {
    printf("Error allocating memory\n");
    fclose(file1);
    return 1;
  }
  fread(DecOfInd_txt, sizeof(char), file_size1, file1);
  fclose(file1);
  DecOfInd_txt[file_size1] = '\0';

  // Open and read data from CommonSense.txt
  FILE *file2 = fopen("CommonSense.txt", "r");
  if (file2 == NULL) {
    printf("Error opening file: CommonSense.txt\n");
    free(DecOfInd_txt);
    return 1;
  }
  fseek(file2, 0, SEEK_END);
  long file_size2 = ftell(file2);
  fseek(file2, 0, SEEK_SET);
  char *CommonSense_txt = (char *)malloc(file_size2 + 1);
  if (CommonSense_txt == NULL) {
    printf("Error allocating memory\n");
    fclose(file2);
    free(DecOfInd_txt);
    return 1;
  }
  fread(CommonSense_txt, sizeof(char), file_size2, file2);
  fclose(file2);
  CommonSense_txt[file_size2] = '\0';

  printf("\nWe have read %ld characters from file DecOfInd.txt\n", file_size1);
  printf("We have read %ld characters from file CommonSense.txt\n", file_size2);

  // Free allocated memory
  free(DecOfInd_txt);
  free(CommonSense_txt);

  return 0;
}
