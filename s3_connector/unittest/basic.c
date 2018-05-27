#include <sys/stat.h> 

#include "s3_connector.h"

#define BUCKETNAME     "fsals3bucket"
#define TESTSTR        "Some random data"
#define BUF_SIZE       1024
#define PATH_LEN       20
#define FILENAME_LEN   10
#define TMPDIR         "/tmp/fsals3_test/"
#define RETRIES        100

static void _create_random_string(char *string, unsigned length)
{
  int i = 0;  

  srand((unsigned int) time(0) + getpid());
   
  for (i = 0; i < length; i++)
    {
      string[i] = rand() % 20 + 66;
    }
 
  string[i] = '\0';  
}

static void _read_data(char filepath[PATH_LEN], char buf[BUF_SIZE], size_t *read_size)
{
  FILE *fp = fopen(filepath, "r");
  assert(fp != NULL);

  *read_size = fread(buf, BUF_SIZE, 1, fp);

  fclose(fp);
  printf("Random data read %s\n", buf);
}

static void _write_data(char filepath[PATH_LEN], char *buf, size_t *write_size)
{
  FILE *fp = fopen(filepath, "w");
  assert(fp != NULL);

  fwrite(buf, strlen(buf), 1, fp);
  fflush(fp);

  *write_size = ftell(fp);
  fclose(fp);
  printf("Random data written %s\n", buf);
}

static void _create_rand_file(char filepath[PATH_LEN])
{
  int i = 0;
  FILE *fp = NULL;
  char filename[FILENAME_LEN] = {0};

  mkdir(TMPDIR, S_IRWXU);
  strcpy(filepath, TMPDIR);

  for (i = 0; i < RETRIES; i++) {
      _create_random_string(filename, FILENAME_LEN);
      strcat(filepath, filename);
      fp = fopen(filepath, "r");
      if (fp != NULL) {
          fclose(fp);
          continue;
      }
      break;
  }
  fp = fopen(filepath, "w");
  assert(fp != NULL);
  fclose(fp);

  printf("Temporary file create: %s\n", filepath);
}


static void get_object_test()
{
  char put_objName[PATH_LEN] = {0};
  char put_filepath[PATH_LEN] = {0};
  char get_filepath[PATH_LEN] = {0};
  data_pointer *put_data = malloc(sizeof(data_pointer));
  data_pointer *get_data = malloc(sizeof(data_pointer));
  data_pointer *put_metadata = malloc(sizeof(data_pointer));
  data_pointer *get_metadata = malloc(sizeof(data_pointer));
  size_t put_size = 0;
  FILE *put_fp = 0;
  FILE *get_fp = 0;
  int i = 0;

  init_s3_connector();
  printf("S3 connector Module Initialized\n");

  _create_random_string(put_objName, PATH_LEN);
  printf("Creating object %s\n", put_objName);

  _create_rand_file(put_filepath);
  
  _write_data(put_filepath, TESTSTR, &put_size);

  put_fp = fopen(put_filepath, "r");
  put_data->metadata = NULL;
  put_data->metadata_count = 0;
  put_data->fp = put_fp;
  put_data->file_size = put_size;

  put_object(BUCKETNAME, put_objName, put_data);

  _create_rand_file(get_filepath);

  get_fp = fopen(get_filepath, "w");
  get_data->metadata = NULL;
  get_data->metadata_count = 0;
  get_data->fp = get_fp;
  put_data->file_size = 0;

  get_object(BUCKETNAME, put_objName, get_data);

  put_metadata->fp = 0;
  put_metadata->file_size = 0;
  put_metadata->metadata_count = 2;
  put_metadata->metadata = malloc(sizeof(dict) * 2);
  put_metadata->metadata[0].name = strdup("name1");
  put_metadata->metadata[0].value = strdup("value1");
  put_metadata->metadata[1].name = strdup("name2");
  put_metadata->metadata[1].value = strdup("value2");

  put_object_metadata(BUCKETNAME, put_objName, put_metadata);

  get_metadata->fp = 0;
  get_metadata->file_size = 0;
  get_metadata->metadata_count = 0;
  get_metadata->metadata = NULL;
  
  get_object_metadata(BUCKETNAME, put_objName, get_metadata);
  for (i = 0; i < get_metadata->metadata_count; i++) {
    printf("Meta name '%s' Meta value '%s'", get_metadata->metadata[i].name,
           get_metadata->metadata[i].value);
  }

  deinit_s3_connector();
}

static void get_object_metadata_test()
{

}

int main(int argc, char **argv) {

  get_object_test();
/*
  get_object_metadata_test();

  char *objName = argv[1];
  char filepath[PATH_LEN] = {0};
  size_t size = 0;
  FILE *fp = NULL;
  struct dict *metadata = malloc(sizeof(dict)); 

  assert(objName);

  initialize();
  _create_rand_file(filepath);
  _write_data(filepath, &size);

  printf("Press enter");
  getc(stdin);
  fp = fopen(filepath, "r");
  put_object(objName, fp, size);

  printf("Press enter");
  getc(stdin);
  _create_rand_file(filepath);
  assert(filepath);
  fp = fopen(filepath, "w");
  assert(fp);
  get_object(objName, fp, metadata);

  printf("Press enter");
  getc(stdin);
  metadata[0].name = strdup("filesize");
  metadata[0].value = strdup("5");
  put_object_metadata(objName, metadata, 1);

  printf("Press enter");
  getc(stdin);
  get_object_metadata(objName, metadata);
  S3_deinitialize();

  delete_object(objName);
*/
  return 0;
}
