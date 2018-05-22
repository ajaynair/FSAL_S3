#include <errno.h>

#include "curl4s3.h"
typedef struct curl4s3 {
    char *auth_key;
    char *secret_auth_key;
    char *url;
} curl4s3_t;

#define CURL4S3_SET_OPTION(curl, option, value)                 \
    do {                                                        \
        int curl_err;                                           \
        curl_err = curl_easy_setopt((curl), (option), (value)); \
        if (CURLE_OK != curl_err) {                             \
            return (curl_err)                                   \
        }                                                       \
    } while (0);

static const char *region = "us-east-1";
static const char *bucket_id = "curl4s3";

int curl4s3_init()
{
    CURLcode curl_err = CURLE_OK;
    curl_err = curl_global_init(CURL_GLOBAL_ALL);
    if (curl_err != CURLE_OK)
        return EINVAL;

    curl_easy_init();
    return 0;
}

void curl4s3_cleanup() 
{
    curl_global_cleanup();
}

int curl4s3_connect(curl4s3_t *curl4s3, const char *base_url, const char *auth_key,
                    const char *secret_auth_key)
{
    if (curl4s3->auth_key) {
        gsh_free(curl4s3->auth_key);
    }
    curl4s3->auth_key = strdup(auth_key);

    if (curl4s3->secret_auth_key) {
        gsh_free(curl4s3->secret_auth_key);
    }
    curl4s3->secret_auth_key = strdup(secret_auth_key);

    if (curl4s3->base_url) {
        gsh_free(curl4s3->base_url);
    }
    curl4s3->base_url = strdup(base_url);

    return 0;
}

static int curl4s3_generate_req(curl4s3_t *curl4s3, curl4s3_req_t *request, const char *object_id)
{
    char header[BUZ_SIZE];
    char *token = NULL;

    memset(request, sizeof(curl4s3_req_t), 0);

    request->curl = curl_easy_init();
    if (!request->curl) {
        return EINVAL;
    }
    request->headers = NULL;
    request->base_url = NULL;
    request->region = strdup(region);

    CURL4S3_SET_OPTION(request->curl, CURLOPT_VERBOSE, /* Verbose */ 1, status);
    CURL4S3_SET_OPTION(request->curl, CURLOPT_POSTFIELDS, NULL, status);

    token = strstr(curl4s3->base_url, "http://");
    if (!token) {
        curl_easy_cleanup(request->curl);
        request->curl = NULL;
        return EINVAL;
    }
    token += 7;
    sprintf(header, "Host: %s.%s", bucket_id, token);
    request->headers = curl_slist_append(request->headers, header);
    return 0;
}

static int curl4s3_generate_url(curl4s3_req_t *request, char *base_url, curl4s3_ops_t ops,
                             const char *object_id)
{
    size_t url_len = strlen(base_url), nbytes = 0;
    char *token = NULL, *temp_url = NULL;

    switch (ops) {

    case OBJ_PUT_OBJECT:
    case OBJ_GET_OBJECT:
    case OBJ_SET_OBJECT_METADATA:
    case OBJ_GET_OBJECT_METADATA:
    case OBJ_DELETE_OBJECT:
        url_len += ( 1 /* '/' */ + strlen(object_id) + 1 /* '.' */ + strlen(bucket_id))
        break;

    case AUTHENTICATE_USER:
        break;

    default:
        ASSERT(0);
    }

    url_len++; /* '\0' */

    gsh_malloc(request->base_url, char, url_len);
    if (NULL == request->base_url) {
        return EINVAL;
    }
    temp_url = request->base_url;
    switch (ops) {

    case OBJ_PUT_OBJECT:
    case OBJ_GET_OBJECT:
    case OBJ_SET_OBJECT_METADATA:
    case OBJ_GET_OBJECT_METADATA:
    case OBJ_DELETE_OBJECT:
        token = strtoken(base_url, "http://");
        nbytes = token - base_url;
        memcpy(temp_url, base_url, nbytes);
        temp_url += nbytes;
        sprintf(temp_url, "%s.%s/%s", bucket_id, token, object_id);
        break;

    case AUTHENTICATE_USER:
        break;

    default:
        break;
   }

    return 0;
}

static int curl4s3_generate_string_to_sign(curl4s3_t *request, char *url, curl4s3_ops_t ops,
                                           const char *object_id, char *string_to_sign,
                                           char *amz_metadata_pairs, char *amz_metadata_headers,
                                           char *payload)
{
    return 0;
}

static int curl4s3_generate_signing_key(curl4s3_req_t *request, curl4s3_ops_t ops,
                                        const char *secret_auth_key, const char *object_id,
                                        unsigned char *signing_key, char *buffer)
{
    return 0;
}


static int curl4s3_generate_version_signature(curl4s3_t *curl4s3, curl4s3_req_t *request, curl4s3_ops_t ops,
                                              const char *object_id, char *amz_metadata_pairs,
                                              char *amz_metadata_headers, char *payload)
{
    return 0;
}

static int curl4s3_send_request(curl4s3_req_t *req, const char *base_url)
{
    return 0;
}

static size_t curl4s3_get_cb(char *buffer, size_t size, size_t nmemb, void *args)
{
    return 0;
}

static void curl4s3_cleanup_req(curl4s3_req_t *req)
{

}

int curl4s3_ops_obj_get(const char *object_id, char **object_data)
{
    int status;
    curl4s3_req_t request;
    char curl_header[BUZ_SIZE], headers_list[MAX_HEADERS_LIST], hash_payload[PAYLOADLEN];
    curl4s3_cb_arg_t cb_args;

    /* Initialize the callback arguments */
    cb_args.args = object_data;
    cb_args.state = CB_PROCESS_DATA;
    cb_args.content_len = 0;
    cb_args.request = &request;

    strcpy(hash_payload, EMPTY_STRING_HASH);

    status = curl4s3_generate_url(&request, curl4s3->base_url, OBJ_GET_OBJECT, object_id);
    if (status) return status;

    status = curl4s3_generate_version_signature(curl4s3, &request, OBJ_GET_OBJECT, object_id, headers_list, NULL, hash_payload);
    if (status) return status;

    sprintf(curl_header, "x-amz-content-sha256: %s", hash_payload);
    request.headers = curl_slist_append(request.headers, curl_header);

    CURL4S3_SET_OPTION(request.curl, CURLOPT_WRITEFUNCTION, curl4s3_get_cb, status);
    CURL4S3_SET_OPTION(request.curl, CURLOPT_WRITEDATA, &cb_args, status);
    CURL4S3_SET_OPTION(request.curl, CURLOPT_HEADERFUNCTION, curl4s3_get_metadata, status);
    CURL4S3_SET_OPTION(request.curl, CURLOPT_HEADERDATA, &cb_args, status);
    CURL4S3_SET_OPTION(request.curl, CURLOPT_HTTPGET, 1L, status);

    status = curl4s3_send_request(&request, request.base_url);
    if (status)
        return status;

    curl4s3_cleanup_req(&request);
    return 0;
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
