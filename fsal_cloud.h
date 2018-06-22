#ifndef _S3_FSAL_H
#define _S3_FSAL_H

#include "fsal.h"
#include "fsal_convert.h"
#include "abstract_mem.h"
#include "cloud_structures.h"
#include "cloud_connector.h"
#include "common.h"

typedef struct cloud_fsal_export {
    struct fsal_export export;
} cloud_fsal_export_t;

typedef struct {
    char *oid;
    data_pointer *dp;
    struct fsal_obj_handle obj_handle;
} cloud_fsal_obj_handle;

fsal_status_t cloud_create_export(struct fsal_module *fsal_hdl, void *parse_node,
                                      const struct fsal_up_vector *up_ops);
void cloud_handle_ops_init(struct fsal_obj_ops *ops);

struct fsal_staticfsinfo_t *cloud_staticinfo(struct fsal_module *hdl);

/* Handle management */
fsal_status_t cloud_lookup_path(struct fsal_export *exp_hdl,
                 const char *path, struct fsal_obj_handle **handle);

fsal_status_t cloud_create_handle(struct fsal_export *exp_hdl,
                   struct gsh_buffdesc *hdl_desc, struct fsal_obj_handle **handle);

fsal_status_t cloud_readlink(struct fsal_obj_handle *, fsal_errors_t *);

/* I/O management */
fsal_status_t cloud_open(struct fsal_obj_handle *obj_hdl, fsal_openflags_t flags);
fsal_openflags_t cloud_status(struct fsal_obj_handle *obj_hdl);
fsal_status_t cloud_read(struct fsal_obj_handle *obj_hdl, uint64_t offset,
                    size_t buffer_size, void *buffer, size_t *read_amount, bool *end_of_file);

fsal_status_t cloud_write(struct fsal_obj_handle *obj_hdl, uint64_t offset,
                    size_t buffer_size, void *buffer, size_t *write_amount, bool *fsal_stable);
fsal_status_t cloud_renamefile(struct fsal_obj_handle *fparent_old,
                                   const char *name_old,
                                   struct fsal_obj_handle *fparent_new,
                                   const char *name_new);
fsal_status_t cloud_commit(struct fsal_obj_handle *obj_hdl, off_t offset, size_t len);
fsal_status_t cloud_lock_op(struct fsal_obj_handle *obj_hdl, void *p_owner,
                                fsal_lock_op_t lock_op, fsal_lock_param_t *request_lock,
                                fsal_lock_param_t *conflicting_lock);

fsal_status_t cloud_share_op(struct fsal_obj_handle *obj_hdl, void *p_owner,
                                 fsal_share_param_t request_share);
fsal_status_t cloud_close(struct fsal_obj_handle *obj_hdl);
fsal_status_t cloud_lru_cleanup(struct fsal_obj_handle *obj_hdl,
                                    lru_actions_t requests);

static void inline _refill_metadata(struct attrlist *attrs, dict metadata[])
{
    char tmp_str[BUF_SIZE] = { 0 };

    if (FSAL_TEST_MASK(attrs->mask, ATTR_TYPE)) {
        switch (attrs->type) {
        case REGULAR_FILE:
          sprintf(tmp_str, "%d", REGULARFILE_T);
          replace_metadata_value(metadata, FILETYPE, tmp_str);
          break;
        case DIRECTORY:
          sprintf(tmp_str, "%d", DIRECTORY_T);
          replace_metadata_value(metadata, FILETYPE, tmp_str);
          break;
        default:
          printf("Not supported\n");
          exit(0);
        }
    }

    if (FSAL_TEST_MASK(attrs->mask, ATTR_SIZE)) {
      sprintf(tmp_str, "%llu", (unsigned long long) attrs->filesize);
      replace_metadata_value(metadata, FILESIZE, tmp_str);
    }
 
      /* TODO FSID */

    if (FSAL_TEST_MASK(attrs->mask, ATTR_FILEID)) {
      sprintf(tmp_str, "%llu", (unsigned long long) attrs->fileid);
      replace_metadata_value(metadata, FILEID, tmp_str);
    }

    if (FSAL_TEST_MASK(attrs->mask, ATTR_MODE)) {
      sprintf(tmp_str, "%u", attrs->mode);
      replace_metadata_value(metadata, MODE, tmp_str);
    }

    if (FSAL_TEST_MASK(attrs->mask, ATTR_OWNER)) {
      sprintf(tmp_str, "%llu", (unsigned long long) attrs->owner);
      replace_metadata_value(metadata, OWNER, tmp_str);
    }

    if (FSAL_TEST_MASK(attrs->mask, ATTR_GROUP)) {
      sprintf(tmp_str, "%llu", (unsigned long long) attrs->group);
      replace_metadata_value(metadata, GROUP, tmp_str);
    }

    if (FSAL_TEST_MASK(attrs->mask, ATTR_ATIME)) {
      sprintf(tmp_str, "%ld.%ld", attrs->atime.tv_sec, attrs->atime.tv_nsec);
      replace_metadata_value(metadata, ATIME, tmp_str);
    }

    if (FSAL_TEST_MASK(attrs->mask, ATTR_CTIME)) {
      sprintf(tmp_str, "%ld.%ld", attrs->ctime.tv_sec, attrs->ctime.tv_nsec);
      replace_metadata_value(metadata, CTIME, tmp_str);
    }

    if (FSAL_TEST_MASK(attrs->mask, ATTR_CHGTIME)) {
      sprintf(tmp_str, "%ld.%ld", attrs->chgtime.tv_sec, attrs->chgtime.tv_nsec);
      replace_metadata_value(metadata, CHGTIME, tmp_str);
    }

    if (FSAL_TEST_MASK(attrs->mask, ATTR_MTIME)) {
      sprintf(tmp_str, "%ld.%ld", attrs->mtime.tv_sec, attrs->mtime.tv_nsec);
      replace_metadata_value(metadata, MTIME, tmp_str);
    }

    if (FSAL_TEST_MASK(attrs->mask, ATTR_SPACEUSED)) {
      sprintf(tmp_str, "%llu", (unsigned long long) attrs->spaceused);
      replace_metadata_value(metadata, SPACEUSED, tmp_str);
    }
}

static void inline _fill_attrlist(dict metadata[], struct attrlist *attrs, object_file_type_t *type)
{
    size_t metadata_count = 0;
    int i = 0;
    int mindex = 0;
    char filetype_dir[BUF_SIZE] = { 0 };

    get_metadata_count(&metadata_count);

    for (i = 0; i < metadata_count; i++) {
        sscanf(metadata[i].name, "%d", &mindex);
        switch (mindex) {
        case 0:
          sprintf(filetype_dir, "%d", DIRECTORY_T);
          (strcmp(metadata[i].value, filetype_dir) == 0) ? 
             (attrs->type = DIRECTORY) : (attrs->type = REGULAR_FILE);
          *type = attrs->type;
          FSAL_SET_MASK(attrs->mask, ATTR_TYPE); 
          break;

        case 1:
          sscanf(metadata[i].value, "%llu", (long long unsigned int *) &(attrs->filesize));
          attrs->spaceused = attrs->filesize;
          FSAL_SET_MASK(attrs->mask, ATTR_SIZE);
          FSAL_SET_MASK(attrs->mask, ATTR_SPACEUSED);
          break;

        case 2:
          attrs->fsid.major = 0;
          attrs->fsid.minor = 0;
          FSAL_SET_MASK(attrs->mask, ATTR_FSID);
          break;

        case 3:
          sscanf(metadata[i].value, "%llu", (long long unsigned int *) &(attrs->fileid));
          FSAL_SET_MASK(attrs->mask, ATTR_FILEID);
          break;

        case 4:
          sscanf(metadata[i].value, "%d", (unsigned int *) &(attrs->mode));
          FSAL_SET_MASK(attrs->mask, ATTR_MODE);
          break;

        case 5:
          sscanf(metadata[i].value, "%u", (unsigned int *) &(attrs->numlinks));
          FSAL_SET_MASK(attrs->mask, ATTR_NUMLINKS);
          break;

        case 6:
          sscanf(metadata[i].value, "%llu", (long long unsigned int *) &(attrs->owner));
          FSAL_SET_MASK(attrs->mask, ATTR_OWNER);
          break;

        case 7:
          sscanf(metadata[i].value, "%llu", (long long unsigned int *) &(attrs->group));
          FSAL_SET_MASK(attrs->mask, ATTR_GROUP);
          break;

        case 8:
           sscanf(metadata[i].value, "%ld.%ld", &(attrs->atime.tv_sec), &(attrs->atime.tv_nsec));
        FSAL_SET_MASK(attrs->mask, ATTR_ATIME);

          break;

        case 9:
           sscanf(metadata[i].value, "%ld.%ld", &(attrs->ctime.tv_sec), &(attrs->ctime.tv_nsec));
        FSAL_SET_MASK(attrs->mask, ATTR_CTIME);
          break;

        case 10:
           sscanf(metadata[i].value, "%ld.%ld", &(attrs->chgtime.tv_sec), &(attrs->chgtime.tv_nsec));
        FSAL_SET_MASK(attrs->mask, ATTR_MTIME);
          break;

        case 11:
           sscanf(metadata[i].value, "%ld.%ld", &(attrs->mtime.tv_sec), &(attrs->mtime.tv_nsec));
        FSAL_SET_MASK(attrs->mask, ATTR_MTIME);
          break;

        case 12:
            break;

        default:
          printf("Error %s", metadata[i].name);
        }
    } 
}

#endif
