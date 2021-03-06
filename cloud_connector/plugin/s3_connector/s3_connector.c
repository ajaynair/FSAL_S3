#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libs3.h>
#include <assert.h>
#include <limits.h>
#include <libgen.h>

#include "s3_connector.h"

static cloud_ops s3_ops  = {
  .get_object = s3_get_object,
  .put_object = s3_put_object,
  .list_objects = s3_list_objects,
  .get_object_metadata = s3_get_object_metadata,
  .put_object_metadata = s3_put_object_metadata,
  .delete_object = s3_delete_object,
  .cloud_deinit = s3_deinit_connector,
};

/** 
 * state is used to make sure no function except init_s3_connector 
 * is called after deinit_s3_connector 
 */
typedef enum {
   DEINITIALIZED = 0,
   INITIALIZED = 1,
} state;

static state current_state = DEINITIALIZED;

/* Amazon S3 keys */
static char *access_key = NULL;
static char *secret_access_key = NULL;

static void _null_complete_function(S3Status status,
                                    const S3ErrorDetails *error,
                                    void *callbackData)
{
  object_list *objects = callbackData;
  objects->status = 1;
  return; 
}


/** 
 * The file is divided into 2 parts:
 * SECTION 1: STATIC HELPLER FUNCTIONS 
 * Section 2: API FUNCTIONS
 */

/* SECTION 1: STATIC HELPLER FUNCTIONS */

/**
 * @brief Callback function called when an operation is complete 
 *
 * @param[in]  status        Status code of the operation 
 * @param[in]  error         Detailed error description 
 * @param[in]  callbackData  Unused parameter 
 */
static void _responseCompleteCallback(S3Status status,
                                     const S3ErrorDetails *error,
                                     void *callbackData)
{
  int i = 0;
  data_pointer *data = callbackData;
  
  if (data == NULL)
  {
    return;
  }

  data->status = 1;

  if (status == S3StatusOK) {
    printf("Operation successful\n");
    return;
  }

  if (error && error->message) { 
    printf("Error message=: %s\n", error->message);
  }
  if (error && error->resource) { 
    printf("Error message=: %s\n", error->resource);
  }
  if (error && error->furtherDetails) { 
    printf("Error message=: %s\n", error->furtherDetails);
  }
  for (i = 0; i < error->extraDetailsCount; i++) {
    printf("Error Details %s:\n", error->extraDetails[i].name);
    printf("%s\n", error->extraDetails[i].value);
  }
}

static S3Status _null_properties_function(const S3ResponseProperties *properties, void *callbackData)
{

  return S3StatusOK;
}
/**
 * @brief Callback function to get object metadata  
 *
 * The function reads from a buffer which has the data of the object 
 * and stores the data into a file.
 *
 * @param[in]  bufferSize    Size of the buffer 
 * @param[in]  buffer        Buffer in which object data is stored
 * @param[in]  callbackData  data_pointer in which file pointer and metadata pointer are stored
 *
 * @retval 0 Success
 */
static S3Status _responsePropertiesCallback(const S3ResponseProperties *properties, 
                                                 void *callbackData) 
{
    data_pointer *data = callbackData;
    int i = 0;
    S3Status st = S3StatusOK;

    if (data == NULL) {
        return st;
    }

    for (i = 0; i < properties->metaDataCount; i++) {
        //printf("%s: %s\n", properties->metaData[i].name,
               //properties->metaData[i].value);
        data->metadata[i].name = strdup(properties->metaData[i].name);
        data->metadata[i].value = strdup(properties->metaData[i].value);
    }
    data->metadata_count = properties->metaDataCount;

    return st;
}

/**
 * @brief Callback function to get object data  
 *
 * The function reads from a buffer which has the data of the object 
 * and stores the data into a file.
 *
 * @param[in]  bufferSize    Size of the buffer 
 * @param[in]  buffer        Buffer in which object data is stored
 * @param[in]  callbackData  FILE pointer in which we will write the object data
 *
 * @retval 0 Success
 */
static S3Status _getObjectDataCallback(int bufferSize, const char *buffer,
                                            void *callbackData)
{
  data_pointer *data = callbackData;
  S3Status st = S3StatusOK;

  fwrite(buffer, 1, bufferSize, data->fp);
  fflush(data->fp);

  /* TODO Think when should the file be closed */
  return st;
}

/**
 * @brief Callback function to put data to object
 *
 * The function reads from a file and stores the data into an object
 *
 * @param[in]  bufferSize    Maximum size of the buffer 
 * @param[in]  buffer        Buffer in which object data is to be stored
 * @param[in]  callbackData  FILE pointer from which we will read the object data
 *
 * @retval 0 Success
 */
static int _putObjectDataCallback(int bufferSize, char *buffer,
                                 void *callbackData)
{
  data_pointer *data = callbackData;
  size_t readSize = 0;

  if (data->fp == NULL)
    return 0;

  rewind(data->fp);

  readSize = fread(buffer, 1, bufferSize, data->fp);
  //printf("Read from file size=%zu\n", readSize);

  return readSize;     
}

void static inline _clean_data_pointer(data_pointer *dp)
{
    fclose(dp->fp);
    dp->fp = NULL;
}

static S3Status listBucketCallback(int isTruncated, const char *nextMarker,
                                   int contentsCount,
                                   const S3ListBucketContent *contents,
                                   int commonPrefixesCount,
                                   const char **commonPrefixes,
                                   void *callbackData)
{
  object_list *objects = callbackData;
  int i = 0;
  int obj_index = 0;
  S3Status st = S3StatusOK;
  int start_index = 0;
 
  if (contentsCount + commonPrefixesCount == 0)
    return objects->object_count = 0;

  if (objects->object_count == 0) {
    start_index = 1;
    objects->object_count = contentsCount + commonPrefixesCount - 1;
    objects->objects = calloc(100, sizeof(char *)); // TODO make 100 dynamic
  }
  else {
    obj_index = objects->object_count;
    objects->object_count += contentsCount + commonPrefixesCount;
  }

  for (i = start_index; i < contentsCount; i++) {
    objects->objects[obj_index++] = strdup(basename(contents[i].key));
  }
  for (i = 0; i < commonPrefixesCount; i++) {
    objects->objects[obj_index++] = strdup(basename(commonPrefixes[i]));
  }

  return st;
}

/* Section 2: API functions */
int s3_delete_object(char *bucketName, char *objName)
{
  if (current_state != INITIALIZED) {
    //printf("Module not initialized\n");
    return 0;
  }

  S3ResponseHandler responseHandler =
  {
    0,
    &_responseCompleteCallback
  };

  S3BucketContext bucketContext =
  {
    0,
    bucketName,
    S3ProtocolHTTP,
    S3UriStylePath,
    access_key,
    secret_access_key,
    0,
    NULL
  };

  S3_delete_object(&bucketContext, objName, 0, 0, &responseHandler, 0);
  return 0;
}

int s3_put_object_metadata(char *bucketName, char *objName, data_pointer *data)
{
  /* TODO: Create inline function for this */
  if (current_state != INITIALIZED) {
    //printf("Module not initialized\n");
    return 0;
  }

  /**
   * S3PutProperties structure: 
   *  contentType
   *  md5
   *  cacheControl
   *  contentDispositionFilename
   *  contentEncoding
   *  expires
   *  cannedAcl
   *  metaDataCount
   *  metaData
   *  useServerSideEncryption
   */ 
  S3PutProperties putProperties =
  {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    -1,
    S3CannedAclPublicReadWrite,
    data->metadata_count,
    (S3NameValue *) data->metadata,
    0
  };

  int64_t lastModified;
  char eTag[256];

  S3ResponseHandler responseHandler =
  {
    &_responsePropertiesCallback,
    &_responseCompleteCallback
  };

  S3BucketContext bucketContext =
  {
    0,
    bucketName,
    S3ProtocolHTTP,
    S3UriStylePath,
    access_key,
    secret_access_key,
    0,
    NULL
  };

  S3_copy_object(&bucketContext, objName, NULL,
                 objName, &putProperties,
                 &lastModified, sizeof(eTag), eTag, NULL,
                 0,
                 &responseHandler, 0);
  return 0;
}

int s3_get_object_metadata(char *bucketName, char *objName, data_pointer *data)
{
  if (current_state != INITIALIZED) {
    //printf("Module not initialized\n");
    return 0;
  }

  S3ResponseHandler responseHandler =
  {
    &_responsePropertiesCallback,
    &_responseCompleteCallback
  };

  S3BucketContext bucketContext =
  {
    0,
    bucketName,
    S3ProtocolHTTP,
    S3UriStylePath,
    access_key,
    secret_access_key,
    0,
    NULL
  };

  S3_head_object(&bucketContext, objName, 0, 0, &responseHandler, data);
  return 0;
}

int s3_get_object(char *bucketName, char *objectName, data_pointer *data)
{
  S3GetConditions getConditions =
  {
    -1,
    -1,
    NULL,
    NULL,
  };

  S3GetObjectHandler getObjectHandler =
  {
    { &_responsePropertiesCallback, &_responseCompleteCallback },
    &_getObjectDataCallback
  };

  S3BucketContext bucketContext =
  {
    0,
    bucketName,
    S3ProtocolHTTP,
    S3UriStylePath,
    access_key,
    secret_access_key,
    0,
    NULL
  };

  data->status = 0;
 
  S3_get_object(&bucketContext, objectName, &getConditions, 0,
                0, NULL, 0, &getObjectHandler, (void *) data);

  while (data->status == 0);
  return 0;
}

int s3_put_object(char *bucketName, char *objectName, data_pointer *data) 
{
  size_t filesize = 0;  

  if (current_state != INITIALIZED) {
    //printf("Module not initialized\n");
    return 0;
  }

  /**
   * S3PutProperties structure: 
   *  contentType
   *  md5
   *  cacheControl
   *  contentDispositionFilename
   *  contentEncoding
   *  expires
   *  cannedAcl
   *  metaDataCount
   *  metaData
   *  useServerSideEncryption
   */ 
  S3PutProperties putProperties =
  {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    -1,
    S3CannedAclPublicReadWrite,
    data->metadata_count,
    (S3NameValue *) data->metadata,
    0
  };

  S3PutObjectHandler putObjectHandler =
  {
      { &_responsePropertiesCallback, &_responseCompleteCallback },
      &_putObjectDataCallback
  };

  S3BucketContext bucketContext =
  {
    0,
    bucketName,
    S3ProtocolHTTP,
    S3UriStylePath,
    access_key,
    secret_access_key,
    0,
    NULL
  };
  data->status = 0;

  if (data->fp != NULL) { 
    fseek(data->fp, 0L, SEEK_END);
    filesize = ftell(data->fp);
    rewind(data->fp);
  }
  else {
    filesize = 0;
  }

  S3_put_object(&bucketContext, objectName, filesize, &putProperties,
                NULL, 0, &putObjectHandler, (void *) data);

  while (data->status == 0);
  return 0;
  
}

int s3_list_objects(char bucketName[], char prefix[], object_list *objects) 
{
  if (current_state != INITIALIZED) {
    //printf("Module not initialized\n");
    return 0;
  }

    S3ListBucketHandler listBucketHandler =
    {
        // { &responsePropertiesCallback, &responseCompleteCallback },
        { _null_properties_function, _null_complete_function },
        &listBucketCallback
    };
 
  S3BucketContext bucketContext =
  {
    0,
    bucketName,
    S3ProtocolHTTP,
    S3UriStylePath,
    access_key,
    secret_access_key,
    0,
    NULL
  };

  S3_list_bucket(&bucketContext, prefix, NULL, "/", INT_MAX, NULL, 0, &listBucketHandler, objects);
  while (objects->status == 0);
  return 0;
}

/**
 * @brief Initialize s3_connector.
 *
 * The function initializes libs3 and also fetches Amazon S3 keys
 * from environment variables 
 */
int s3_init_connector(cloud_ops **cops)
{
  S3Status st = S3StatusOK;

  /* Initialize libs3 */
  st = S3_initialize("s3", S3_INIT_ALL, NULL);
  if (st != S3StatusOK) {
    //printf("S3)initialize failed!");
    return 0;
  }

  /* Read Amazon S3 access key and secret access key from environment variables */
  access_key = getenv("ACCESS_KEY");
  if (!access_key) {
    //printf("access_key environment variable not set\n");
    exit(0);
  }

  secret_access_key = getenv("SECRET_ACCESS_KEY");
  if (!secret_access_key) {
    //printf("secret_access_key environment variable not set\n");
    exit(0);
  }

  *cops = &s3_ops;
  current_state = INITIALIZED;
  return 0;
}

/**
 * @brief deinitialize s3_connector.
 *
 * The function calls libs3 deinitialize
 */
void s3_deinit_connector()
{
  S3_deinitialize();
  current_state = DEINITIALIZED;
}
