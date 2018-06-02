#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "obj_structs.h"
#include "common.h"

void pack_dirent(char filename[], char oid[], s3_dirent **dirent)
{
    s3_dirent *_dirent = NULL;

   _dirent = calloc(1, sizeof(s3_dirent));
   _dirent->filename = strdup(filename);
   _dirent->oid = strdup(oid);
   *dirent = _dirent; 
}

// void get_dirent(s3_dirent *dirent, char filename[], char oid[]);

void dirent_to_str(s3_dirent *dirent, char dirent_str[])
{
  strcpy(dirent_str, MAGIC);
  strcat(dirent_str, SEPARATOR);

  strcat(dirent_str, dirent->filename);
  strcat(dirent_str, SEPARATOR);

  strcat(dirent_str, dirent->oid);
  strcat(dirent_str, SEPARATOR);

  strcat(dirent_str, MAGIC);
  strcat(dirent_str, SEPARATOR);
}

void next_dirent_to_file(FILE *fp, char dirent[])
{
    fwrite(dirent, 1, strlen(dirent), fp);
    fflush(fp);

    /* _get_metadata_value(dp, FILESIZE, filesize_str);
    */
}

static void _check_magic(FILE *fp)
{
    char buf[BUF_SIZE] = {0};

    fread(buf, 1, strlen(MAGIC), fp);
    buf[strlen(MAGIC)] = '\0';
    assert(strcmp(buf, MAGIC) == 0);
}

static void _check_separator(FILE *fp)
{
    char buf[BUF_SIZE] = {0};

    fread(buf, 1, 1, fp);
    buf[1] = '\0';
    assert(strcmp(buf, SEPARATOR) == 0);
}

static void _read_name(FILE *fp, char buf[])
{
    int i = 0;
    char c[1] = {0};

    while (1) {
       fread(c, 1, 1, fp);
       if (c[0] == SEPARATOR[0]) {
           fseek(fp, -1, SEEK_CUR);
           break;
        }
       buf[i++] = c[0];
    }

    buf[i] = '\0';
}

void next_dirent_from_file(FILE *fp, s3_dirent **dirent)
{
    char filename[BUF_SIZE] = {0};
    char oid[BUF_SIZE] = {0};

    _check_magic(fp);
    _check_separator(fp);

    _read_name(fp, filename);
    _check_separator(fp);

    _read_name(fp, oid);
    _check_separator(fp);

    _check_magic(fp);
    _check_separator(fp);

    pack_dirent(filename, oid, dirent);
}

void find_dirent_in_file(FILE *fp, char filename[], s3_dirent **dirent)
{
  s3_dirent *_dirent = NULL;

  *dirent = NULL;
  rewind(fp);

  while(1) {
    next_dirent_from_file(fp, &_dirent);
    if (strcmp(_dirent->filename, filename) == 0) {
        *dirent = _dirent;
        return;
    }
    free(_dirent);
  }
}

int set_default_metadata(dict **metadata, char filetype[], char fileid[])
{
    dict *_metadata = NULL;
    struct timespec cur_time;
    char cur_time_str[BUF_SIZE] = { 0 };

    _metadata = calloc(METADATACOUNT, sizeof(dict));

    clock_gettime(CLOCK_REALTIME, &(cur_time));
    sprintf(cur_time_str, "%ld.%ld", cur_time.tv_sec, cur_time.tv_nsec);

    _metadata[0].name = strdup(FILETYPE);
    _metadata[0].value = strdup(filetype);

    _metadata[1].name = strdup(FILESIZE);
    _metadata[1].value = strdup("0");

    _metadata[2].name = strdup(FSID);
    _metadata[2].value = strdup("0");

    _metadata[3].name = strdup(FILEID);
    _metadata[3].value = strdup(fileid);

    _metadata[4].name = strdup(MODE);
    _metadata[4].value = strdup("0755");

    _metadata[5].name = strdup(NUMLINKS);
    _metadata[5].value = strdup("2");

    _metadata[6].name = strdup(OWNER);
    _metadata[6].value = strdup("1000");

    _metadata[7].name = strdup(GROUP);
    _metadata[7].value = strdup("1000");

    _metadata[8].name = strdup(ATIME);
    _metadata[8].value = strdup(cur_time_str);

    _metadata[9].name = strdup(CTIME);
    _metadata[9].value = strdup(cur_time_str);

    _metadata[10].name = strdup(CHGTIME);
    _metadata[10].value = strdup(cur_time_str);

    _metadata[11].name = strdup(MTIME);
    _metadata[11].value = strdup(cur_time_str);

    _metadata[12].name = strdup(SPACEUSED);
    _metadata[12].value = strdup("0");

    *metadata = _metadata;

    return 0;
}

void replace_metadata_value(dict metadata[], char mname[], char mvalue[])
{
    int i = 0;

    for (i = 0; i < METADATACOUNT; i++) {
        if (strcmp(metadata[i].name, mname) == 0) {
            free(metadata[i].value);
            metadata[i].value = strdup(mvalue);
            return;
        }
    }
    assert(i <= METADATACOUNT);
}

void get_metadata_value(dict metadata[], char mname[], char mvalue[])
{
    int i = 0;

    for (i = 0; i < METADATACOUNT; i++) {
        if (strcmp(metadata[i].name, mname) == 0) {
            strcpy(mvalue, metadata[i].value);
            return;
        }
    }
    assert(i <= METADATACOUNT);
}

void get_metadata_count(size_t *count)
{
    *count = METADATACOUNT;
}

void inc_filesize_metadata(dict metadata[], size_t inc_val)
{
    char filesize_str[BUF_SIZE] = {0};
    size_t filesize = 0;

    get_metadata_value(metadata, FILESIZE, filesize_str);

    sscanf(filesize_str, "%zu", &filesize);
    filesize += inc_val;
    sprintf(filesize_str, "%zu", filesize);

    replace_metadata_value(metadata, FILESIZE, filesize_str);
    replace_metadata_value(metadata, SPACEUSED, filesize_str);
}
