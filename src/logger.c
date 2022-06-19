#include "../inc/logger.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#define LEVEL_LENGTH 5
#define DATE_LENGTH 25
#define THREAD_NAME_LENGTH 10
#define ACTION_LENGTH 160

static char* get_current_date(void) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char* date = malloc(40);
  snprintf(date, 40, "%d-%02d-%02d_%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

  return date;
}

Logger* logger_create(void) {
  char* date = get_current_date();
  char* path = malloc(51);

  strcpy(path, "./log/cpu-usage-tracker_");
  strcat(path, date);
  strcat(path, ".log");

  struct stat st = {0};
  if (stat("./log", &st) != 0)
    mkdir("./log", 0700);

  Logger* logger = malloc(sizeof(*logger));

  *logger = (Logger){.f = fopen(path, "a+")};

  if (logger->f == NULL)
    return NULL;

  free(path);
  free(date);

  return logger;
}

void logger_put(register Buffer* buffer, const char* log_type, const char* date, const char* thread_name, const char* action, const size_t packet_size) {
  if (date == NULL || thread_name == NULL || action == NULL || log_type == NULL)
    return;

  if (strcmp(log_type, "INFO") != 0 && strcmp(log_type, "ERROR") != 0)
    return;

  uint8_t* log = malloc(packet_size);
  snprintf((char*)log, 210, "[%s] [%s] [%s] %s", log_type, date, thread_name, action);

  buffer_put(buffer, log, packet_size);

  free(log);
};

void logger_read(register const Logger* const logger, register Buffer* buffer) {
  uint8_t* log = buffer_get(buffer);

  char* log_type = malloc(LEVEL_LENGTH + 2);
  char* date = malloc(DATE_LENGTH + 2);
  char* thread_name = malloc(THREAD_NAME_LENGTH + 2);
  char* action = malloc(ACTION_LENGTH);

  sscanf((char*)log, "%s %s %s %s", log_type, date, thread_name, action);

  fprintf(logger->f, "%*s %s %*s %s", -LEVEL_LENGTH, log_type, date, -THREAD_NAME_LENGTH, thread_name, action);
  fflush(logger->f);

  free(log);
  free(log_type);
  free(date);
  free(thread_name);
  free(action);
}

void logger_destroy(Logger* const logger) {
  if (logger == NULL) 
    return;
  
  fclose(logger->f);
  free(logger);
}