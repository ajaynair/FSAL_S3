#ifndef _S3_FSAL_H
#define _S3_FSAL_H

#include "fsal.h"
#include "fsal_convert.h"

typedef struct s3_fsal_export {
    struct fsal_export export;
} s3_fsal_export_t;

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
#endif
