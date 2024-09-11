#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -f <database file> [-n]\n", argv[0]);
  printf("\t -f <database file>        - (required) path to database file\n");
  printf("\t -n                        - create new database file\n");
  printf("\t -f 'name,address,hours'   - path to database file\n");
  printf("\t -l                        - list employees\n");
  return;
}

int main(int argc, char *argv[]) {
  struct dbheader_t *dbHeader = NULL;
  struct employee_t *dbEmployees = NULL;

  // command line options
  int c;
  bool shouldCreateNewFile = false;
  bool shouldListEmployees = false;
  char *filepath = NULL;
  char *addEmployeeString = NULL;

  while ((c = getopt(argc, argv, "nlf:a:")) != -1) {
    switch (c) {
    case 'n':
      shouldCreateNewFile = true;
      break;
    case 'f':
      filepath = optarg;
      break;
    case 'a':
      addEmployeeString = optarg;
      break;
    case 'l':
      shouldListEmployees = true;
      break;
    case '?':
      printf("Unknown option - %c\n", c);
      break;
    default:
      return -1;
    }
  }

  if (filepath == NULL) {
    printf("Filepath is a required argument\n");
    print_usage(argv);

    return 0;
  }

  int dbfd = -1;
  if (shouldCreateNewFile) {
    printf("Creating a new file...\n");
    dbfd = create_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to create database file at path: %s\n", filepath);
      return EXIT_FAILURE;
    }

    if (create_db_header(dbfd, &dbHeader) == STATUS_ERROR) {
      printf("Failed to create database header");
      return EXIT_FAILURE;
    }
  } else {
    printf("Reading db at: %s\n", filepath);
    dbfd = open_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to open database file at path: %s\n", filepath);
      return EXIT_FAILURE;
    }

    if (validate_db_header(dbfd, &dbHeader) == STATUS_ERROR) {
      printf("Failed to validate database header\n");
      return EXIT_FAILURE;
    }
  }

  if (read_employees(dbfd, dbHeader, &dbEmployees) == STATUS_ERROR) {
    printf("Failed to read employees from database\n");
    return EXIT_FAILURE;
  }

  if (addEmployeeString != NULL) {
    add_employee(dbHeader, &dbEmployees, addEmployeeString);
  }

  if (shouldListEmployees) {
    list_employees(dbHeader, dbEmployees);
  }

  if (output_file(dbfd, dbHeader, dbEmployees) == STATUS_ERROR) {
    printf("Error writing database header\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
