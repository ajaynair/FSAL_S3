#ifndef _CURL4S3_
#define _CURL4S3_

#include <curl/curl.h>

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

#define CURL4S3_CONNECT_TIMEOUT    10
#define CURL4S3_REQUEST_TIMEOUT    (60*45)
#define CURL4S3_MAX_RETRIES        2
#define BUF_SIZE             1024
#define EMPTY_STRING_HASH  "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"

typedef struct _curl4s3_req_t {
    struct curl_slist *headers;
    CURL *curl;       /* Handle to curl library's easy interface */
    char *signature;  /* Holds signature calculated from HMAC SHA1 */
    char *base_url;   /* S3 base URL, with s3 and account, but without bucket or object */
    char *date;
    char *time;
    char *amz_metadata_pairs;  /* List of metadata. Needed for generating string_to_sign */
} curl4s3_req_t;

typedef enum curl4s3_ops {
    USER_AUTH                = 1,
    OBJ_POST_DATA            = 2,
    OBJ_PUT_DATA             = 3,
    OBJ_GET_DATA             = 4,
    OBJ_POST_METADATA        = 5,
    OBJ_GET_METADATA         = 6,
    OBJ_DELETE               = 7,
} curl4s3_ops_t;

typedef enum _cb_state {
    CB_PROCESS_DATA,
    CB_INVALID
} cb_state_t;

typedef struct _curl4s3_read_cb_arg_t {
    void              *args;
    cb_state_t         state;
    double             content_len;
} curl4s3_read_cb_arg_t;

typedef struct _curl4s3_get_cb_arg_t {
    char              *object_data;
    size_t             object_data_len;
    char              *object_metadata;
    size_t             object_metadata_len;
    cb_state_t         state;
    double             content_len;
    curl4s3_req_t     *request;
} curl4s3_get_cb_arg_t;

int curl4s3_init();
int curl4s3_connect(const char *base_url, const char *auth_key,
                    const char *secret_auth_key)
void curl4s3_cleanup();

int curl4s3_ops_obj_get(const char *object_id, char **object_metadata, char **object_data);

int curl4s3_ops_obj_get_metadata(const char *object_id, const char amz_metadata_key[BUF_SIZE][BUF_SIZE], char amz_metadata_value[BUF_SIZE][BUF_SIZE]);

int curl4s3_ops_obj_put(const char *object_id, char *buff);

int curl4s3_ops_obj_post(const char *object_id, char *buff);

int curl4s3_ops_obj_post_metadata(const char *object_id, const char amz_metadata_key[BUF_SIZE][BUF_SIZE], const char amz_metadata_value[BUF_SIZE][BUF_SIZE]);

int curl4s3_ops_obj_delete(const char *object_id);

#endif /* _CURL4S3_ */
