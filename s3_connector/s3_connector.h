#ifndef __S_CONNECTOR__
#define __S_CONNECTOR__

typedef struct {
  char *name;
  char *value;
} dict;

typedef struct {
  FILE   *fp;
  size_t file_size;
  dict   *metadata; 
  size_t metadata_count; 
  int    status;
} data_pointer; 

void get_object(char *bucketName, char *objectName, data_pointer *data);

void put_object(char *bucketName, char *objectName, data_pointer *data); 

void get_object_metadata(char *bucketName, char *objName, data_pointer *data);

void put_object_metadata(char *bucketName, char *objName, data_pointer *data);

void delete_object(char *bucketName, char *objName);

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
