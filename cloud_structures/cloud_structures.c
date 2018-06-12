#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "cloud_structures.h"
#include "common.h"

/* TODO: Create macros for default values */
int set_default_metadata(dict **metadata, char filetype[], char fileid[])
{
    dict *_metadata = NULL;
    struct timespec cur_time;
    char cur_time_str[BUF_SIZE] = { 0 };

    _metadata = calloc(METADATACOUNT, sizeof(dict));

    sprintf(_metadata[FILETYPE].name, "%d", FILETYPE);
    _metadata[FILETYPE].value = strdup(filetype);

    sprintf(_metadata[FILESIZE].name, "%d", FILESIZE);
    _metadata[FILESIZE].value = strdup("0");

    sprintf(_metadata[FSID].name, "%d", FSID);
    _metadata[FSID].value = strdup("0");

    sprintf(_metadata[FILEID].name, "%d", FILEID);
    _metadata[FILEID].value = strdup(fileid);

    sprintf(_metadata[MODE].name, "%d", MODE);
    _metadata[MODE].value = strdup("0755");

    sprintf(_metadata[NUMLINKS].name, "%d", NUMLINKS);
    _metadata[NUMLINKS].value = strdup("2");

    sprintf(_metadata[OWNER].name, "%d", OWNER);
    _metadata[OWNER].value = strdup("1000");

    sprintf(_metadata[GROUP].name, "%d", GROUP);
    _metadata[GROUP].value = strdup("1000");

    clock_gettime(CLOCK_REALTIME, &(cur_time));
    sprintf(cur_time_str, "%ld.%ld", cur_time.tv_sec, cur_time.tv_nsec);

    sprintf(_metadata[ATIME].name, "%d", ATIME);
    _metadata[ATIME].value = strdup(cur_time_str);

    sprintf(_metadata[CTIME].name, "%d", CTIME);
    _metadata[CTIME].value = strdup(cur_time_str);

    sprintf(_metadata[CHGTIME].name, "%d", CHGTIME);
    _metadata[CHGTIME].value = strdup(cur_time_str);

    sprintf(_metadata[MTIME].name, "%d", MTIME);
    _metadata[MTIME].value = strdup(cur_time_str);

    sprintf(_metadata[SPACEUSED].name, "%d", SPACEUSED);
    _metadata[SPACEUSED].value = strdup("0");

    *metadata = _metadata;

    return 0;
}

void replace_metadata_value(dict metadata[], metadata_name mname, char mvalue[])
{
    int i = 0;
    char mname_str[BUF_SIZE] = { 0 };

    sprintf(mname_str, "%d", mname);
    for (i = 0; i < METADATACOUNT; i++) {
        if (strcmp(metadata[i].name, mname_str) == 0) {
            free(metadata[i].value);
            metadata[i].value = strdup(mvalue);
            return;
        }
    }
    assert(i <= METADATACOUNT);
}

void get_metadata_value(dict metadata[], metadata_name mname, char mvalue[])
{
    int i = 0;
    char mname_str[BUF_SIZE] = { 0 };

    sprintf(mname_str, "%d", mname);
    for (i = 0; i < METADATACOUNT; i++) {
        if (strcmp(metadata[i].name, mname_str) == 0) {
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
