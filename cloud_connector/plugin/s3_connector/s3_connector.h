#ifndef __S_CONNECTOR__
#define __S_CONNECTOR__

#include "cloud_connector.h"

/* TODO: All the fuctions should return some status */
int s3_get_object(char bucketName[], char objectName[], data_pointer *data);

int s3_put_object(char bucketName[], char objectName[], data_pointer *data); 

int s3_delete_object(char bucketName[], char objName[]);

int s3_list_objects(char bucketName[], char delimiter[], object_list *objects);

int s3_get_object_metadata(char bucketName[], char objName[], data_pointer *data);

int s3_put_object_metadata(char bucketName[], char objName[], data_pointer *data);

/**
 * @brief Initialize s3_connector.
 *
 * The function initializes libs3 and also fetches Amazon S3 keys
 * from environment variables 
 */
int s3_init_connector(cloud_ops **cops);

/**
 * @brief deinitialize s3_connector.
 *
 * The function calls libs3 deinitialize
 */
void s3_deinit_connector();

#endif
