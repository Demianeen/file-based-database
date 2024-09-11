#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_endian.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "common.h"
#include "parse.h"

// Employees

int read_employees(int fd, struct dbheader_t *dbHeader,
                   struct employee_t **employeesOut) {
  if (fd == -1) {
    printf("Issue with opening a file: invalid file description\n");
    return STATUS_ERROR;
  }

  int count = dbHeader->count;

  struct employee_t *employees = calloc(count, sizeof(struct employee_t));
  if (employees == NULL) {
    printf("Employees memory allocation failed\n");
  }

  for (int i = 0; i < dbHeader->count; i++) {
    if (read(fd, &employees[i], sizeof(struct employee_t)) !=
        sizeof(struct employee_t)) {
      printf("Couldn't read a database employees data.");
      return STATUS_ERROR;
    }
  }

  for (int i = 0; i < count; i++) {
    employees[i].hours = ntohl(employees[i].hours);
  }

  *employeesOut = employees;
  return STATUS_SUCCESS;
}

int realloc_employees(struct dbheader_t *dbHeader,
                      struct employee_t **employees, int num_new_employees) {
  dbHeader->count += num_new_employees;
  struct employee_t *temp =
      realloc(*employees, dbHeader->count * sizeof(struct employee_t));
  if (temp == NULL) {
    printf("Employees memory reallocation failed\n");
    return STATUS_ERROR;
  }

  *employees = temp;

  return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t *dbHeader, struct employee_t **employees,
                 char *addEmployeeString) {
  if (realloc_employees(dbHeader, employees, 1) == STATUS_ERROR) {
    return STATUS_ERROR;
  }

  char *name = strsep(&addEmployeeString, EMPLOYEE_DELIMETER);
  char *addr = strsep(&addEmployeeString, EMPLOYEE_DELIMETER);
  char *hours = strsep(&addEmployeeString, EMPLOYEE_DELIMETER);

  strlcpy((*employees)[dbHeader->count - 1].name, name,
          sizeof((*employees)[dbHeader->count - 1].name));

  strlcpy((*employees)[dbHeader->count - 1].address, addr,
          sizeof((*employees)[dbHeader->count - 1].address));

  (*employees)[dbHeader->count - 1].hours = atoi(hours);

  return STATUS_SUCCESS;
}

void list_employees(struct dbheader_t *dbHeader, struct employee_t *employees) {
  for (int i = 0; i < dbHeader->count; i++) {
    printf("Employee %d\n", i);
    printf("\t Name: %s\n", employees[i].name);
    printf("\t Address: %s\n", employees[i].address);
    printf("\t Hours: %d\n", employees[i].hours);
    printf("\n");
  }
}

// Db file

int output_file(int fd, struct dbheader_t *dbHeader,
                struct employee_t *dbEmployees) {
  if (fd == -1) {
    printf("Issue with opening a file: invalid file description\n");
    return STATUS_ERROR;
  }

  int count = dbHeader->count;

  dbHeader->version = htons(dbHeader->version);
  dbHeader->count = htons(dbHeader->count);
  dbHeader->magic = htonl(dbHeader->magic);
  dbHeader->filesize =
      htonl(sizeof(struct dbheader_t) + sizeof(struct employee_t) * count);

  lseek(fd, 0, SEEK_SET);

  write(fd, dbHeader, sizeof(struct dbheader_t));

  int i = 0;
  for (int i = 0; i < count; i++) {
    dbEmployees[i].hours = htonl(dbEmployees[i].hours);
    write(fd, &dbEmployees[i], sizeof(struct employee_t));
  }

  return STATUS_SUCCESS;
}

// Headers

int validate_db_header(int fd, struct dbheader_t **headerOut) {
  if (fd == -1) {
    printf("Issue with opening a file: invalid file description\n");
    return STATUS_ERROR;
  }

  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if (header == NULL) {
    printf("Memory allocation failed for dbheader\n");
  }

  if (read(fd, header, sizeof(struct dbheader_t)) !=
      sizeof(struct dbheader_t)) {
    perror("read\n");
    free(header);
    return -1;
  }

  header->version = ntohs(header->version);
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(header->filesize);

  if (header->magic != HEADER_MAGIC) {
    printf("Invalid header format. Magic number doesn't match\n");
    free(header);
    return STATUS_ERROR;
  }

  if (header->version != 1) {
    printf("Invalid version is specified in the header\n");
    free(header);
    return STATUS_ERROR;
  }

  struct stat dbStat = {0};
  fstat(fd, &dbStat);
  if (header->filesize != dbStat.st_size) {
    printf("Corrupted database\n");
    free(header);
    return STATUS_ERROR;
  }

  *headerOut = header;
  return STATUS_SUCCESS;
}

int create_db_header(int fd, struct dbheader_t **headerOut) {
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if (header == NULL) {
    printf("Failed to create db header\n");
    return STATUS_ERROR;
  }
  header->version = 1;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header;
  return STATUS_SUCCESS;
}
