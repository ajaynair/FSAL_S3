#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "cloud_structures.h"
#include "common.h"

/* TODO: Create macros for default values */
int set_default_metadata(dict **metadata, filetype_value filetypeval, char fileid[])
{
    dict *_metadata = NULL;
    struct timespec cur_time;
    char cur_time_str[BUF_SIZE] = { 0 };
    char tmp_str[BUF_SIZE] = { 0 };

    _metadata = calloc(METADATACOUNT, sizeof(dict));

    sprintf(tmp_str, "%d", FILETYPE);
    _metadata[FILETYPE].name = strdup(tmp_str);
    sprintf(tmp_str, "%d", filetypeval);
    _metadata[FILETYPE].value = strdup(tmp_str);

    sprintf(tmp_str, "%d", FILESIZE);
    _metadata[FILESIZE].name = strdup(tmp_str);
    _metadata[FILESIZE].value = strdup("0");

    sprintf(tmp_str, "%d", FSID);
    _metadata[FSID].name = strdup(tmp_str);
    _metadata[FSID].value = strdup("0");

    sprintf(tmp_str, "%d", FILEID);
    _metadata[FILEID].name = strdup(tmp_str);
    _metadata[FILEID].value = strdup(fileid);

    sprintf(tmp_str, "%d", MODE);
    _metadata[MODE].name = strdup(tmp_str);
    _metadata[MODE].value = strdup("493");

    sprintf(tmp_str, "%d", NUMLINKS);
    _metadata[NUMLINKS].name = strdup(tmp_str);
    _metadata[NUMLINKS].value = strdup("2");

    sprintf(tmp_str, "%d", OWNER);
    _metadata[OWNER].name = strdup(tmp_str);
    _metadata[OWNER].value = strdup("1000");

    sprintf(tmp_str, "%d", GROUP);
    _metadata[GROUP].name = strdup(tmp_str);
    _metadata[GROUP].value = strdup("1000");

    clock_gettime(CLOCK_REALTIME, &(cur_time));
    sprintf(cur_time_str, "%ld.%ld", cur_time.tv_sec, cur_time.tv_nsec);

    sprintf(tmp_str, "%d", ATIME);
    _metadata[ATIME].name = strdup(tmp_str);
    _metadata[ATIME].value = strdup(cur_time_str);

    sprintf(tmp_str, "%d", CTIME);
    _metadata[CTIME].name = strdup(tmp_str);
    _metadata[CTIME].value = strdup(cur_time_str);

    sprintf(tmp_str, "%d", CHGTIME);
    _metadata[CHGTIME].name = strdup(tmp_str);
    _metadata[CHGTIME].value = strdup(cur_time_str);

    sprintf(tmp_str, "%d", MTIME);
    _metadata[MTIME].name = strdup(tmp_str);
    _metadata[MTIME].value = strdup(cur_time_str);

    sprintf(tmp_str, "%d", SPACEUSED);
    _metadata[SPACEUSED].name = strdup(tmp_str);
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
