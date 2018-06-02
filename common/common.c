#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "common.h"

void create_random_string(char *string, unsigned length)
{
  int i = 0;

  srand((unsigned int) time(0) + getpid());

  for (i = 0; i < length; i++)
    {
      string[i] = (rand() % 10) + '0';
    }

  string[i] = '\0';
}

int get_tmp_file(FILE **fp)
{
    int i = 0;
    char filename[BUF_SIZE] = {0};
    char filepath[BUF_SIZE] = {0};
    FILE *_fp = NULL;

    for (i = 0; i < RETRIES; i++) {
        strcpy(filepath, TMPDIR);
        create_random_string(filename, 10);
        strcat(filepath, filename);
        _fp = fopen(filepath, "r");
        if (_fp != NULL) {
            fclose(_fp);
            continue;
        }
        break;
    }

    *fp = fopen(filepath, "w+");
    assert(*fp != NULL);
    printf("Temp file %s created\n", filepath);

    return 0;
}

int get_next_path_component(const char *path, int *path_index, char name[BUF_SIZE])
{
    int name_index = 0;

    if (path[*path_index] == '/')
        (*path_index)++;

    for (name_index = 0; path[*path_index] != '/' && path[*path_index] != '\0'; name_index++,(*path_index)++) {
        name[name_index] = path[*path_index];
    }
    name[name_index] = '\0';

    if (path[*path_index] == '\0' || path[(*path_index) + 1] == '\0')
        return 0;

    return 1;
}

