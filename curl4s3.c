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
        return EINVAL;
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
        return EINVAL;
   }

    return 0;
}

static int curl4s3_generate_string_to_sign(curl4s3_t *request, char *url, curl4s3_ops_t ops,
                                           const char *object_id, char *string_to_sign,
                                           char *amz_metadata_pairs, char *amz_metadata_headers,
                                           char *payload)
{
    int status = 0;
    char resource[BUF_SIZE], time[BUF_SIZE], canonical_request[BUF_SIZE], date[BUF_SIZE];
    char base_url[BUF_SIZE], digest[SHA256_DIGEST_LEN];
    char *token = NULL;
    struct tm *_tm;
    time_t _time;

    time(&_time);

    _tm = gmtime(&_time);
    if (NULL == _tm) {
        return EINVAL;
    }

    /* Format time and time as required */
    status = strftime(time, 2048,"%Y%m%dT%H%M%SZ", _tm);
    if (!status) return status;

    status = strftime(date, 2048,"%Y%m%d", _tm);
    if (!status) return status;

    request->time = strdup(time);
    request->date = strdup(date);

    strcpy(base_url, url);

    token = strstr(base_url, "http://");
    token += 7;

    switch (ops) {
    case OBJ_PUT_DATA:
    if (strcmp(amz_metadata_pairs,""))
        sprintf(canonical_request,
            "PUT\n/%s\n\nhost:%s.%s\nx-amz-content-sha256:%s\nx-amz-date:%s\n%s\nhost;x-amz-content-sha256;x-amz-date;%s\n%s",
            object_id, bucket_id, token, payload, request->time, amz_metadata_pairs, amz_metadata_headers, payload);

    else
        sprintf(canonical_request,
            "PUT\n/%s\n\nhost:%s.%s\nx-amz-content-sha256:%s\nx-amz-date:%s\n\nhost;x-amz-content-sha256;x-amz-date\n%s",
            object_id, bucket_id, token, payload, request->time, payload);

    break;

    case OBJ_GET_DATA:
        sprintf(canonical_request,
        "GET\n/%s\n\nhost:%s.%s\nx-amz-content-sha256:%s\nx-amz-date:%s\n\nhost;x-amz-content-sha256;x-amz-date\n%s",
        object_id, bucket_id, token, payload, request->time, payload);
        break;

   case OBJ_DELETE_DATA:
        sprintf(canonical_request,
        "DELETE\n/%s\n\nhost:%s.%s\nx-amz-content-sha256:%s\nx-amz-date:%s\n\nhost;x-amz-content-sha256;x-amz-date\n%s",
        object_id, bucket_id, token, payload, request->time, payload);

    case AUTHENTICATE_USER:
        break;

    default:
        ASSERT(0);
   }

    /* Need to look at this */
    status = sha256_encryption(canonical_request, digest);

    sprintf(str_to_sign, "AWS4-HMAC-SHA256\n%s\n%s/%s/s3/aws4_request\n%s", request->time,
        request->date, region, digest);

    return status;
}

static int curl4s3_generate_signing_key(curl4s3_req_t *request, curl4s3_ops_t ops,
                                        const char *secret_auth_key, const char *object_id,
                                        unsigned char *signing_key, char *buffer)
{
    int status = 0;

    unsigned char secret_key[BUF_SIZE];
    unsigned char date_key[SHA256_DIGEST_LENGTH];
    unsigned char date_region_key[SHA256_DIGEST_LENGTH];
    unsigned char date_region_service_key[SHA256_DIGEST_LENGTH];

    sprintf((char *)secret_key, "AWS4%s", secret_key);

    /* Four-step signing key calculation */
    status = hmac_sha256_encryption(secret_key, strlen((char *)secret_key), (unsigned char *)request->date, date_key);
    if (status)
        return EINVAL;

    status = hmac_sha256_encryption(date_key, SHA256_DIGEST_LENGTH, (unsigned char *)region, date_region_key);
    if (status)
        return EINVAL;

    status = hmac_sha256_encryption(date_region_key, SHA256_DIGEST_LENGTH, (unsigned char *)"s3", date_region_service_key);
    if (status)
        return EINVAL;

    status = hmac_sha256_encryption(date_region_service_key, SHA256_DIGEST_LENGTH, (unsigned char *)"aws4_request", signing_key);
    if (status)
        return EINVAL;

    return status;
}


static int curl4s3_generate_version_signature(curl4s3_t *curl4s3, curl4s3_req_t *request, curl4s3_ops_t ops,
                                              const char *object_id, char *amz_metadata_pairs,
                                              char *amz_metadata_headers, char *payload)
{
    char string_to_sign[BUF_SIZE], header[BUF_SIZE];
    char credential_header[BUF_SIZE], signed_header[BUF_SIZE];
    unsigned char digest[SHA256_DIGEST_LENGTH], signing_key[BUF_SIZE],
    temp[SHA256_DIGEST_LENGTH];

    status = curl4s3_generate_string_to_sign(request, curl4s3->base_url, ops, object_id, string_to_sign,
                amz_metadata_pairs, amz_metadata_headers, payload);
    if (status)
        return status;

    status = curl4s3_genegrate_signing_key(request, ops, curl4s3->secret_auth_key, object_id, signing_key, amz_metadata_headers);
    if (status)
        return status;

    /* Use the signing key to sign string_to_sign and add the created version_signature to the
       Authorization header */

    request->signature = NULL;
    gsh_malloc(request->signature, char, SHA256_DIGEST_LENGTH);

    /* Generate message digest using HAMC-SHA256 */
    status = hmac_sha256_encryption(signing_key, SHA256_DIGEST_LENGTH,
                                    (unsigned char *)string_to_sign, digest);
    generate_hex_key(digest, temp);
    request->signature = strndup((char *)temp, SHA256_DIGEST_LENGTH * 2);

    sprintf(credential_header, "%s/%s/%s/s3/aws4_request", curl4s3->auth_key, request->date_scope,
        request->region);
    sprintf(signed_header, "host;x-amz-content-sha256;x-amz-date");

    if (amz_metadata_headers != NULL) {
        strcat(signed_header, ";");
        strncat(signed_header, amz_metadata_headers, strlen(amz_metadata_headers));
    }
    sprintf(header, "Authorization: AWS4-HMAC-SHA256 Credential=%s, SignedHeaders=%s, Signature=%s",
            credential_header, signed_header, request->signature);
    request->headers = curl_slist_append(request->headers, header);

    sprintf(header, "x-amz-date: %s", request->date);
    request->headers = curl_slist_append(request->headers, header);

    return status;
}

static int curl4s3_send_request(curl4s3_req_t *request, const char *base_url)
{
    CURLcode curl_err = CURLE_OK;
    int max_retries = CURL4S3_MAX_RETRIES;
    long http_status = 0;

    CURL_EASY_SETOPT(request->curl, CURLOPT_URL, base_url, status);
    CURL_EASY_SETOPT(request->curl, CURLOPT_USERAGENT, "curl/7.32.0", status);
    CURL_EASY_SETOPT(request->curl, CURLOPT_CONNECTTIMEOUT, CURL4S3_CONNECT_TIMEOUT, status);
    CURL_EASY_SETOPT(request->curl, CURLOPT_TIMEOUT, CURL4S3_REQUEST_TIMEOUT, status);
    CURL_EASY_SETOPT(request->curl, CURLOPT_HTTPHEADER, request->headers, status);

    do {
        curl_err = curl_easy_perform(request->curl);
        if (CURLE_OK != curl_err) {
            return curl_err;
        }
        curl_easy_getinfo(request->curl, CURLINFO_RESPONSE_CODE, &http_status);
    } while ((http_status == HTTP_STATUS_TIMEOUT) && (max_retries--));

    return 0;
}

static size_t curl4s3_get_cb(char *buffer, size_t size, size_t nitems, void *args)
{
    return 0;
}

static size_t curl4s3_get_metadata_cb(char *buffer, size_t size, size_t nitems, void *args)
{
    return 0;
}

static void curl4s3_cleanup_req(curl4s3_req_t *req)
{

}

int curl4s3_ops_obj_get(const char *object_id, char **object_metadata, char **object_data)
{
    int status;
    curl4s3_req_t request;
    char curl_header[BUZ_SIZE], headers_list[MAX_HEADERS_LIST], hash_payload[PAYLOADLEN];
    curl4s3_get_cb_arg_t cb_args;

    /* Initialize the callback arguments */
    cb_args.object_data = object_data;
    cb_args.object_data_len = 0;
    cb_args.object_metadata = object_metadata;
    cb_args.object_metadata_len = 0;
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
