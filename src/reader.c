#include "../inc/reader.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

Reader* reader_create(char* const path, const size_t read_frequency) {
  if (path == NULL || read_frequency == 0)
    return NULL;

  struct stat buffer;
  if(stat(path, &buffer) != 0)
    return NULL;

  Reader* const reader = malloc(sizeof(*reader));

  *reader = (Reader){.path = path,
                     .f = fopen(path, "r"),
                     .read_frequency = read_frequency,
                    };

  return reader;
}

uint8_t* reader_read(register const Reader* reader, const size_t cores, const size_t one_core_size) {
  if (reader == NULL)
    return NULL;
  
  char* const line = malloc(sizeof(char) * one_core_size);
  uint8_t* const packet = malloc(one_core_size * (cores + 1)); 
  if (line == NULL || packet == NULL)
    return NULL;

  for (size_t i = 0; i <= cores; ++i) {
    char* result = fgets(&line[0], (int)one_core_size, reader->f);
    (void)result;

    memcpy(&packet[i * one_core_size], &line[0], one_core_size);
    
    memset(&line[0], 0, one_core_size);
  }

  free(line);
  return packet;
}

void reader_reset(Reader* const reader) {
  if (reader == NULL)
    return;

  fclose(reader->f);
  reader->f = fopen(reader->path, "r");
  if (reader->f == NULL)
    return;
}

void reader_destroy(Reader* const reader) {
  if (reader == NULL)
    return;

  fclose(reader->f);
  free(reader);
}
