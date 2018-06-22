#ifndef __CLOUD_CONNECTOR__
#define __CLOUD_CONNECTOR__

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

typedef struct {
  char **objects;
  int object_count;
  int    status;
} object_list;

typedef struct {
  int (*get_object)(char bucketName[], char objectName[], data_pointer *data);
  int (*put_object)(char bucketName[], char objectName[], data_pointer *data);
  int (*delete_object)(char bucketName[], char objectName[]);
  int (*list_objects)(char bucketName[], char delimiter[], object_list *objects);
  int (*get_object_metadata)(char bucketName[], char objectName[], data_pointer *data);
  int (*put_object_metadata)(char bucketName[], char objectName[], data_pointer *data);
  void (*cloud_deinit)();
} cloud_ops;

int cloud_init();
int get_object(char bucketName[], char objectName[], data_pointer *data);
int put_object(char bucketName[], char objectName[], data_pointer *data);
int delete_object(char bucketName[], char objectName[]);
int list_objects(char bucketName[], char delimiter[], object_list *objects);
int get_object_metadata(char bucketName[], char objectName[], data_pointer *data);
int put_object_metadata(char bucketName[], char objectName[], data_pointer *data);
void cloud_deinit();

#endif 
