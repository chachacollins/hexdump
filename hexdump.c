#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BYTES 1024 * 500
#define IS_PRINTABLE_ASCII(c) (((c) > 31) && ((c) < 127))
#define NON_PRINTABLE_ASCII '.'

size_t file_open_and_read(char *filename, char *buffer, size_t size) {
  if (access(filename, F_OK) != 0) {
    fprintf(stderr, "ERROR: file %s not found", filename);
    exit(1);
  }
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s", filename);
    exit(1);
  }
  size_t file_length;
  fseek(file, 0L, SEEK_END);
  file_length = ftell(file);
  fseek(file, 0L, SEEK_SET);
  if (file_length > size) {
    fprintf(stderr, "ERROR: file %s is too large ", filename);
    exit(1);
  } else if (file_length <= 0) {
    fprintf(stderr, "ERROR: file %s is too small ", filename);
    exit(1);
  }
  fread(buffer, sizeof(char), size, file);
  fclose(file);
  return file_length;
}

void hexdump(void *buffer, size_t size) {
  uint8_t *data = buffer;
  size_t i, j;
  for (i = 0; i < size; i++) {
    uint8_t byte = data[i];
    if ((i % 16) == 0) {
      printf("%08lx ", i);
    }
    if (i % 8 == 0) {
      printf(" ");
    }

    printf("%02x ", byte);

    if (((i % 16) == 15) || (i == size - 1)) {
      for (j = 0; j < 15 - (i % 16); j++) {
        printf("   ");
      }
      printf(" |");
      for (j = (i - (i % 16)); j <= i; j++) {
        printf("%c",
               IS_PRINTABLE_ASCII(data[j]) ? data[j] : NON_PRINTABLE_ASCII);
      }

      printf("|\n");
    }
  }

  printf("%08lx\n", i);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "USAGE: %s <FILE>\n", argv[0]);
    exit(0);
  }
  char *filename = argv[1];
  char buffer[MAX_BYTES] = {0};
  size_t file_length = file_open_and_read(filename, buffer, MAX_BYTES);
  hexdump(buffer, file_length);
  return 0;
}
