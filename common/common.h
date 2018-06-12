#ifndef __COMMON__
#define __COMMON__

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE                 1024
#define RETRIES                  100
#define TMPDIR                   "/tmp/"

typedef struct {
  char *name;
  char *value;
} dict;

void create_random_string(char *string, unsigned length);
int get_tmp_file(FILE **fp);
int get_next_path_component(const char *path, int *path_index, char name[BUF_SIZE]);
#endif
