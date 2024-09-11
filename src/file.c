#include <stdio.h>

#include <fcntl.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "common.h"
#include "file.h"

int open_file(char *filename, int flags) {
  int fd = open(filename, flags, 0644);
  return fd;
}

int create_db_file(char *filename) {
  int fd = open_file(filename, O_RDWR);
  if (fd != -1) {
    close(fd);
    printf("File already exists\n");
    return STATUS_ERROR;
  }

  fd = open_file(filename, O_RDWR | O_CREAT);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }
  return fd;
}

int open_db_file(char *filename) {
  int fd = open_file(filename, O_RDWR);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }
  return fd;
}
