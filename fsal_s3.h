#ifndef _S3_FSAL_H
#define _S3_FSAL_H

#include "fsal.h"
#include "fsal_convert.h"
#include "abstract_mem.h"
#include "obj_structs.h"
#include "s3_connector.h"
#include "common.h"

typedef struct s3_fsal_export {
    struct fsal_export export;
} s3_fsal_export_t;

typedef struct {
    char *oid;
    struct fsal_obj_handle obj_handle;
} s3_fsal_obj_handle;

fsal_status_t s3_create_export(struct fsal_module *fsal_hdl, void *parse_node,
                                      const struct fsal_up_vector *up_ops);
void s3_handle_ops_init(struct fsal_obj_ops *ops);

struct fsal_staticfsinfo_t *s3_staticinfo(struct fsal_module *hdl);

/* Handle management */
fsal_status_t s3_lookup_path(struct fsal_export *exp_hdl,
                 const char *path, struct fsal_obj_handle **handle);

fsal_status_t s3_create_handle(struct fsal_export *exp_hdl,
                   struct gsh_buffdesc *hdl_desc, struct fsal_obj_handle **handle);

fsal_status_t s3_readlink(struct fsal_obj_handle *, fsal_errors_t *);

/* I/O management */
fsal_status_t s3_open(struct fsal_obj_handle *obj_hdl, fsal_openflags_t flags);
fsal_openflags_t s3_status(struct fsal_obj_handle *obj_hdl);
fsal_status_t s3_read(struct fsal_obj_handle *obj_hdl, uint64_t offset,
                    size_t buffer_size, void *buffer, size_t *read_amount, bool *end_of_file);

fsal_status_t s3_write(struct fsal_obj_handle *obj_hdl, uint64_t offset,
                    size_t buffer_size, void *buffer, size_t *write_amount, bool *fsal_stable);
fsal_status_t s3_renamefile(struct fsal_obj_handle *fparent_old,
                                   const char *name_old,
                                   struct fsal_obj_handle *fparent_new,
                                   const char *name_new);
fsal_status_t s3_commit(struct fsal_obj_handle *obj_hdl, off_t offset, size_t len);
fsal_status_t s3_lock_op(struct fsal_obj_handle *obj_hdl, void *p_owner,
                                fsal_lock_op_t lock_op, fsal_lock_param_t *request_lock,
                                fsal_lock_param_t *conflicting_lock);

fsal_status_t s3_share_op(struct fsal_obj_handle *obj_hdl, void *p_owner,
                                 fsal_share_param_t request_share);
fsal_status_t s3_close(struct fsal_obj_handle *obj_hdl);
fsal_status_t s3_lru_cleanup(struct fsal_obj_handle *obj_hdl,
                                    lru_actions_t requests);

static void inline _fill_attrlist(dict metadata[], struct attrlist *attrs, object_file_type_t *type)
{
    size_t metadata_count = 0;
    int i = 0;
    int mindex = 0;

    get_metadata_count(&metadata_count);

    for (i = 0; i < metadata_count; i++) {
        mindex = metadata[i].name[0];
        switch (mindex) {
        case '0':
          (strcmp(metadata[i].value, "directory") == 0) ? 
             (attrs->type = DIRECTORY) : (attrs->type = REGULAR_FILE);
          *type = attrs->type;
          FSAL_SET_MASK(attrs->mask, ATTR_TYPE); 
          break;

        case '1':
          sscanf(metadata[i].value, "%llu", (long long unsigned int *) &(attrs->filesize));
          attrs->spaceused = attrs->filesize;
          FSAL_SET_MASK(attrs->mask, ATTR_SIZE);
          FSAL_SET_MASK(attrs->mask, ATTR_SPACEUSED);
          break;

        case '2':
          attrs->fsid.major = 0;
          attrs->fsid.minor = 0;
          FSAL_SET_MASK(attrs->mask, ATTR_FSID);
          break;

        case '3':
          sscanf(metadata[i].value, "%llu", (long long unsigned int *) &(attrs->fileid));
          FSAL_SET_MASK(attrs->mask, ATTR_FILEID);
          break;

        case '4':
          sscanf(metadata[i].value, "%o", (unsigned int *) &(attrs->mode));
          FSAL_SET_MASK(attrs->mask, ATTR_MODE);
          break;

        case '5':
          sscanf(metadata[i].value, "%u", (unsigned int *) &(attrs->numlinks));
          FSAL_SET_MASK(attrs->mask, ATTR_NUMLINKS);
          break;

        case '6':
          sscanf(metadata[i].value, "%llu", (long long unsigned int *) &(attrs->owner));
          FSAL_SET_MASK(attrs->mask, ATTR_OWNER);
          break;

        case '7':
          sscanf(metadata[i].value, "%llu", (long long unsigned int *) &(attrs->group));
          FSAL_SET_MASK(attrs->mask, ATTR_GROUP);
          break;

        case '8':
           sscanf(metadata[i].value, "%ld.%ld", &(attrs->atime.tv_sec), &(attrs->atime.tv_nsec));
        FSAL_SET_MASK(attrs->mask, ATTR_ATIME);

          break;

        case '9':
           sscanf(metadata[i].value, "%ld.%ld", &(attrs->ctime.tv_sec), &(attrs->ctime.tv_nsec));
        FSAL_SET_MASK(attrs->mask, ATTR_CTIME);
          break;

        case 'a':
           sscanf(metadata[i].value, "%ld.%ld", &(attrs->chgtime.tv_sec), &(attrs->chgtime.tv_nsec));
        FSAL_SET_MASK(attrs->mask, ATTR_MTIME);
          break;

        case 'b':
           sscanf(metadata[i].value, "%ld.%ld", &(attrs->mtime.tv_sec), &(attrs->mtime.tv_nsec));
        FSAL_SET_MASK(attrs->mask, ATTR_MTIME);
          break;

        case 'c':
            break;

        default:
          printf("Error %s", metadata[i].name);
        }
    } 
}

#endif
