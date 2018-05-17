#include "fsal_s3.h"

typedef struct s3_fsal_module {
    struct fsal_module fsal;
    struct fsal_staticfsinfo_t fs_info;
} s3_fsal_module_t;

static struct s3_fsal_module S3;
static const char myname[] = "S3";

#define S3_SUPPORTED_ATTRIBUTES   (                 \
    ATTR_TYPE     | ATTR_SIZE     |                 \
    ATTR_FSID     | ATTR_FILEID   |                 \
    ATTR_MODE     | ATTR_OWNER    |                 \
    ATTR_GROUP    | ATTR_ATIME    |                 \
    ATTR_CTIME    | ATTR_MTIME                     \
    )

/* TODO Check if the following values are correct for S3 */
static struct fsal_staticfsinfo_t s3_info = {
    .maxfilesize = UINT64_MAX,
    .maxlink = _POSIX_LINK_MAX,
    .maxnamelen = 1024,
    .maxpathlen = 1024,
    .no_trunc = true,
    .chown_restricted = true,
    .case_insensitive = false,
    .case_preserving = true,
    .link_support = false,
    .symlink_support = false,
    .lock_support = false,
    .lock_support_owner = false,
    .lock_support_async_block = false,
    .named_attr = false,
    .unique_handles = true,
    .lease_time = {10, 0},
    .acl_support = FSAL_ACLSUPPORT_DENY,
    .cansettime = true,
    .homogenous = true,
    .supported_attrs = S3_SUPPORTED_ATTRIBUTES,
    .maxread = FSAL_MAXIOSIZE,
    .maxwrite = FSAL_MAXIOSIZE,
    .umask = 0,
    .auth_exportpath_xdev = false,
    .xattr_access_rights = 0400,
};

struct fsal_staticfsinfo_t *s3_staticinfo(struct fsal_module *hdl)
{
    LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return &s3_info;
}

static fsal_status_t s3_init_config(struct fsal_module *fsal_hdl,
                                    config_file_t config_struct)
{
    /* TODO Read all the following data from config file ganesha.conf */
    char *host = "https://s3.amazonaws.com";
    char *bucket = "nascc1"; 

    LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);
}

/*
 * Function: fsal_init
 * Definition: Constructor of the FSAL_S3 module
 * Registers itself(FSAL_S3) and fills it's ops
 * Return Value: void
 */
/* TODO change void fsal_init() to MODULE_INIT s3_init() */
void fsal_init(void)
{
        int retval;
        struct fsal_module *myself = &S3.fsal;

        retval = register_fsal(myself, myname, FSAL_MAJOR_VERSION,
            FSAL_MINOR_VERSION, FSAL_ID_EXPERIMENTAL);
        if (retval != 0) {
                fprintf(stderr, "S3 module failed to register");
                return;
        }

    myself->ops->create_export = s3_create_export;
    myself->ops->init_config = s3_init_config;
}

/*
 * Function: fsal_unload
 * Definition: Destructor of the FSAL_S3 module
 * Unregisters itself(FSAL_S3)
 * Return Value: void
 */
/* TODO change void fsal_unload() to MODULE_FINI s3_unload() */
void fsal_unload(void)
{
        int retval;

        retval = unregister_fsal(&S3.fsal);
        if (retval != 0) {
                fprintf(stderr, "S3 module failed to unregister");
                return;
        }
}
