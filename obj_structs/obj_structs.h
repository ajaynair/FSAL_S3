#ifndef __OBJSTRUCTS__
#define __OBJSTRUCTS__

/* TODO: 1) rename file to s3_structs.h and cli to s3_cli.h 
 *       2) typedef dict to metadata 
 */
#include <stdint.h>

#include "common.h"

#define OIDLEN        10
#define ROOTOID       "0"
#define BUCKETNAME    "fsals3bucket"
#define MAGIC         "FSALS3MAGICSTR"
/* TODO: it should be one character */
#define SEPARATOR     "@"  /* Should be a single character */ 
#define S3MAXOBJSIZE  5000000000000

#define METADATACOUNT 13

/* TODO: Convert to enum */
/* Metadata keys */
#define FILETYPE      "0"
#define FILESIZE      "1"
#define FSID          "2"
#define FILEID        "3"
#define MODE          "4"
#define NUMLINKS      "5"
#define OWNER         "6"
#define GROUP         "7"
#define ATIME         "8"
#define CTIME         "9"
#define CHGTIME       "a"
#define MTIME         "b"
#define SPACEUSED     "c"

/* TODO: Convert to enum */
/* Metadata values */
#define  REGULARFILE_T  "regularfile"
#define  DIRECTORY_T    "directory"

typedef struct  {
  char *filename;
  char *oid;
} s3_dirent;

/* Create a dirent from individual values */
/* TODO: put_dirent */
void pack_dirent(char filename[], char oid[], s3_dirent **dirent);

/* Unpack a dirent to get individual values */
void get_dirent(s3_dirent *dirent, char filename[], char oid[]);

/* Converts dirent structure to character array to be stored in a file */
/* TODO: make it static */
void dirent_to_str(s3_dirent *dirent, char dirent_str[]);

/* TODO: write str_to_dirent and make it static */

/* Store next dirent to the file */
/* TODO: Change it to put_dirent_to_file */
void next_dirent_to_file(FILE *fp, char dirent[]);

/* Read next dirent from a file. */
/* TODO: Change it to get_dirent_find_file */
void next_dirent_from_file(FILE *fp, s3_dirent **dirent);

void find_dirent_in_file(FILE *fp, char filename[], s3_dirent **dirent);

/* Set dict with default metadata names and values. Values to be filled for Filetype 
 * and FileId are provided as parameter */
int set_default_metadata(dict **metadata, char filetype[], char fileid[]);

/* Replace the metadata of name 'mname' with 'mvalue' */
/* TODO: rename it to put_metadata_value */
void replace_metadata_value(dict metadata[], char mname[], char mvalue[]);

/* Get the metadata value of metadata name 'mname' */
void get_metadata_value(dict metadata[], char mname[], char mvalue[]);

/* Get the count of metadata */
void get_metadata_count(size_t *count);

/* Increment filesize metadata by the given value */
void inc_filesize_metadata(dict metadata[], size_t inc_val);

#endif
