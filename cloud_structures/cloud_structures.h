#ifndef __OBJSTRUCTS__
#define __OBJSTRUCTS__

/* TODO: typedef dict to metadata */
#include <stdint.h>

#include "common.h"

#define OIDLEN        10
#define ROOTOID       "0"
#define BUCKETNAME    "fsalcloudbucket"
#define MAXOBJSIZE    50000 /* TODO Change this to some actual value */

#define METADATACOUNT 13

/* Metadata keys */
typedef enum {
  FILETYPE  = 0,
  FILESIZE  = 1,
  FSID      = 2,
  FILEID    = 3,
  MODE      = 4,
  NUMLINKS  = 5,
  OWNER     = 6,
  GROUP     = 7,
  ATIME     = 8,
  CTIME     = 9,
  CHGTIME   = 10,
  MTIME     = 11,
  SPACEUSED = 12,
} metadata_name;

/* Metadata values */
typedef enum {
  REGULARFILE_T = 0,
  DIRECTORY_T   = 1,
} metadata_filetype_values;

/* Set dict with default metadata names and values. Values to be filled for Filetype 
 * and FileId are provided as parameter */
int set_default_metadata(dict **metadata, char filetype[], char fileid[]);

/* Replace the metadata of name 'mname' with 'mvalue' */
/* TODO: rename it to put_metadata_value */
void replace_metadata_value(dict metadata[], metadata_name mname, char mvalue[]);

/* Get the metadata value of metadata name 'mname' */
void get_metadata_value(dict metadata[], metadata_name mname, char mvalue[]);

/* Get the count of metadata */
void get_metadata_count(size_t *count);

/* Increment filesize metadata by the given value */
void inc_filesize_metadata(dict metadata[], size_t inc_val);

#endif
