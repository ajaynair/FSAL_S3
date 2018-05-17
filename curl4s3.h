#ifndef CURL4S3
#define CURL4S3

#include <curl/curl.h>

typedef struct curl4s3 {
    char *auth;
    char *url;
} curl4s3_t;

/* 
 * Amazon S3 supports the following operations 
 *
 * Delete Multiple Objects
 * DELETE Object
 * DELETE Object tagging
 * GET Object
 * GET Object ACL
 * GET Object tagging
 * GET Object torrent
 * HEAD Object
 * OPTIONS object
 * POST Object
 * POST Object restore
 * PUT Object
 * PUT Object - Copy
 * PUT Object acl
 * PUT Object tagging
 * SELECT Object Content
 * Abort Multipart Upload
 * Complete Multipart Upload
 * Initiate Multipart Upload
 * List Parts
 * Upload Part
 * Upload Part - Copy
 */

typedef enum curl4s3_ops {
    USER_AUTH                = 1,
    OBJ_POST_DATA            = 2,
    OBJ_PUT_DATA             = 3,
    OBJ_GET_DATA             = 4,
    OBJ_POST_METADATA        = 5,
    OBJ_GET_METADATA         = 6,
    OBJ_DELETE               = 7,
} curl4s3_ops_t;

typedef struct curl4s3_req {
    CURL *curl;
} curl4s3_req_t;

int curl4s3_init();
void curl4s3_cleanup();

int curl4s3_ops_obj_get(curl4s3_t *curl4s3, const char *object_id, char **object_data);

int curl4s3_ops_obj_get_metadata(curl4s3_t *curl4s3, const char *object_id, const char *metadata_key, char **metadata_value);

int curl4s3_ops_obj_put(curl4s3_t *curl4s3, const char *object_id, char *buff);

int curl4s3_ops_obj_post(curl4s3_t *curl4s3, const char *object_id, char *buff);

int curl4s3_ops_obj_post_metadata(curl4s3_t *curl4s3, const char *object_id, const char *metadata_key, const char *metadata_value);

int curl4s3_ops_obj_delete(curl4s3_t *curl4s3, const char *object_id);

#endif // S3CURLLIB
