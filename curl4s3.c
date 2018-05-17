#include <errno.h>

#include "curl4s3.h"

int curl4s3_init()
{
    CURLcode curl_err = CURLE_OK;
    curl_err = curl_global_init(CURL_GLOBAL_ALL);
    if (curl_err != CURLE_OK)
        return EINVAL;

    curl_easy_init();
    /* TODO complete the function */
}

void cdmi_curl_cleanup() 
{
    curl_global_cleanup();
}

int curl4s3_ops_obj_get(curl4s3_t *curl4s3, const char *object_id, char **object_data)
{

}

int curl4s3_ops_obj_get_metadata(curl4s3_t *curl4s3, const char *object_id, const char *metadata_key, char **metadata_value)
{

}

int curl4s3_ops_obj_put(curl4s3_t *curl4s3, const char *object_id, char *buff)
{

}

int curl4s3_ops_obj_post(curl4s3_t *curl4s3, const char *object_id, char *buff)
{

}

int curl4s3_ops_obj_post_metadata(curl4s3_t *curl4s3, const char *object_id, const char *metadata_key, const char *metadata_value)
{

}

int curl4s3_ops_obj_delete(curl4s3_t *curl4s3, const char *object_id)
{

}
