#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define IS_PRINTABLE_ASCII(c) (((c) > 31) && ((c) < 127))
#define NON_PRINTABLE_ASCII '.'

size_t file_size(FILE *file) {
  size_t file_length;
  fseek(file, 0L, SEEK_END);
  file_length = ftell(file);
  fseek(file, 0L, SEEK_SET);
  return file_length;
}

char *file_open_and_read(char *filename, size_t *length) {
  if (access(filename, F_OK) != 0) {
    fprintf(stderr, "ERROR: file %s not found\n", filename);
    exit(1);
  }
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s\n", filename);
    exit(1);
  }
  size_t file_length = file_size(file);
  if (file_length <= 0) {
    fprintf(stderr, "ERROR: file %s is too small \n", filename);
    exit(1);
  }
  char *buffer = malloc(sizeof(char) * file_length);
  if (!buffer) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(buffer, 1, file_length, file);
  if (bytes_read != file_length) {
    fprintf(stderr, "Error: Failed to read entire file\n");
    fclose(file);
    free(buffer);
    return NULL;
  }
  *length = file_length;
  fclose(file);
  return buffer;
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
  free(buffer);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "USAGE: %s <FILE>\n", argv[0]);
    exit(1);
  }
  char *filename = argv[1];
  size_t file_length;
  char *buffer = file_open_and_read(filename, &file_length);
  if (!buffer) {
    exit(1);
  }
  hexdump(buffer, file_length);
  return 0;
}
