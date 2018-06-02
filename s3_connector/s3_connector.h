#ifndef __S_CONNECTOR__
#define __S_CONNECTOR__

#include "common.h"

/*
 * This structure is used to store the file data and metadata
 * fp - Stores the local-file pointer which 
 *       stores the information of the NFS file 
 * metadata - A dictionary which stores the attributes of a file
 *             in a name value dictionary
 * metadata_count - Number of metadata that are stored
 * status - A (temporary) variable used to make the async libs3 calls 
 *           sync (Only part of the POC) 
 */
typedef struct {
  FILE   *fp;
  dict   *metadata; 
  size_t metadata_count; 
  int    status;
} data_pointer; 

void get_object(char *bucketName, char *objectName, data_pointer *data);

void put_object(char *bucketName, char *objectName, data_pointer *data); 

void delete_object(char *bucketName, char *objName);

void get_object_metadata(char *bucketName, char *objName, data_pointer *data);

void put_object_metadata(char *bucketName, char *objName, data_pointer *data);

void static inline clean_data_pointer(data_pointer *dp)
{
    fclose(dp->fp);
    dp->fp = NULL;
}

/**
 * @brief Initialize s3_connector.
 *
 * The function initializes libs3 and also fetches Amazon S3 keys
 * from environment variables 
 */
void init_s3_connector();

/**
 * @brief deinitialize s3_connector.
 *
 * The function calls libs3 deinitialize
 */
void deinit_s3_connector();

#endif
