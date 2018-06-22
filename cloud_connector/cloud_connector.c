#include <dlfcn.h>

#include "cloud_connector.h"

static cloud_ops *cops;

int cloud_init()
{
  char lib_path[BUF_SIZE] = {0};
  char type[BUF_SIZE] = "s3";    // Should be read from the config file
  char cloud_init_fn[BUF_SIZE] = {0};
  void *handle = NULL;
  int (*cloud_init)(cloud_ops **);

  sprintf(lib_path, "lib%sconnector.so", type);
  handle = dlopen(lib_path, RTLD_NOW);
  if (!handle) {
    printf("Error in dlopen() %s", dlerror());
    goto out;
  }

  sprintf(cloud_init_fn, "%s_init_connector", type);
  cloud_init = dlsym(handle, cloud_init_fn);
  if (dlerror()) {
    printf("Error in dlerror()");
    goto out;
  }

  cloud_init(&cops);
/*
  sprintf(cloud_deinit, "deinit_%s_connector", type);
  cp->cloud_deinit = dlsym(handle, cloud_deinit);
  if (dlerror()) {
    gci_log(GLOG_INFO, "error: cant find symbol %s", plugin_cleanup_sym);
    goto outerr;
  }
*/

out:
  return 0;
}

int get_object(char bucketName[], char objectName[], data_pointer *data)
{
  return cops->get_object(bucketName, objectName, data);
}
int put_object(char bucketName[], char objectName[], data_pointer *data)
{
  return cops->put_object(bucketName, objectName, data);
}

int delete_object(char bucketName[], char objectName[])
{
  return cops->delete_object(bucketName, objectName);
}

int list_objects(char bucketName[], char delimiter[], object_list *objects)
{
  return cops->list_objects(bucketName, delimiter, objects);
}

int get_object_metadata(char bucketName[], char objectName[], data_pointer *data)
{
  return cops->get_object_metadata(bucketName, objectName, data);
}

int put_object_metadata(char bucketName[], char objectName[], data_pointer *data)
{
  return cops->put_object_metadata(bucketName, objectName, data);
}

void cloud_deinit()
{
  return cops->cloud_deinit();
}
