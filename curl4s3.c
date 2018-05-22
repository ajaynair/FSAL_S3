#include <errno.h>

#include "curl4s3.h"
typedef struct curl4s3 {
    char *auth;
    char *url;
} curl4s3_t;

int curl4s3_init()
{
    CURLcode curl_err = CURLE_OK;
    curl_err = curl_global_init(CURL_GLOBAL_ALL);
    if (curl_err != CURLE_OK)
        return EINVAL;

    curl_easy_init();
    /* TODO complete the function */
}

void curl4s3_cleanup() 
{
    curl_global_cleanup();
}

int curl4s3_ops_obj_get(const char *object_id, char **object_data)
{

}

int curl4s3_ops_obj_get_metadata(const char *object_id, const char metadata_key[BUF_SIZE][BUF_SIZE], char metadata_value[BUF_SIZE][BUF_SIZE])
{

}

int curl4s3_ops_obj_put(const char *object_id, char *buff)
{

}

int curl4s3_ops_obj_post(const char *object_id, char *buff)
{

}

int curl4s3_ops_obj_post_metadata(const char *object_id, const char metadata_key[BUF_SIZE][BUF_SIZE], const char metadata_value[BUF_SIZE][BUF_SIZE])
{

}

int curl4s3_ops_obj_delete(const char *object_id)
{

}
