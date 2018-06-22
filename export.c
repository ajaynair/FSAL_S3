#include "fsal_cloud.h"
#include "FSAL/fsal_commonlib.h"
#include "FSAL/fsal_config.h"

/* TODO What is this structure? */
static struct config_item export_params[] = {
        CONF_ITEM_NOOP("name"),
        CONFIG_EOL
};

/* TODO What is this structure? */
static struct config_block export_param = {
        .dbus_interface_name = "org.ganesha.nfsd.config.fsal.cloud-export%d",
        .blk_desc.name = "FSAL",
        .blk_desc.type = CONFIG_BLOCK,
        .blk_desc.u.blk.init = noop_conf_init,
        .blk_desc.u.blk.params = export_params,
        .blk_desc.u.blk.commit = noop_conf_commit
};

/**
 * extract_handle: wire handle -> key handle
 * handle_digest : fsal handle -> wire handle
 * handle_to_key : fsal handle -> key handle
 */
static fsal_status_t cloud_extract_handle(struct fsal_export *exp_hdl,
                        fsal_digesttype_t in_type, struct gsh_buffdesc *fh_desc)
{
    LogCrit(COMPONENT_FSAL, ":Entry Exit Not implemented");
    return fsalstat(ERR_FSAL_NO_ERROR, 0);
}

static bool cloud_fs_supports(struct fsal_export *exp_hdl, fsal_fsinfo_options_t option)
{
    struct fsal_staticfsinfo_t *info = NULL;
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);

    LogFullDebug(COMPONENT_FSAL, ":Entry Exit");
    info = cloud_staticinfo(exp_hdl->fsal);
    return fsal_supports(info, option);

}

void cloud_export_ops_init(struct export_ops *ops) 
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    ops->lookup_path = cloud_lookup_path;
    ops->extract_handle = cloud_extract_handle;
    ops->create_handle = cloud_create_handle;
    ops->fs_supports = cloud_fs_supports;
}

/* create_export
 * Create an export point and return a handle to it to be kept
 * in the export list.
 * First lookup the fsal, then create the export and then put the fsal back.
 * returns the export with one reference taken.
 */
fsal_status_t cloud_create_export(struct fsal_module *fsal_hdl,
                               void *parse_node,
                               const struct fsal_up_vector *up_ops)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
        struct cloud_fsal_export *myself;
        struct config_error_type err_type;
        int retval = 0;
        fsal_errors_t fsal_error = ERR_FSAL_NO_ERROR;

        myself = gsh_calloc(1, sizeof(struct cloud_fsal_export));
        if (myself == NULL) {
                LogMajor(COMPONENT_FSAL,
                         "out of memory for object");
                return fsalstat(posix2fsal_error(errno), errno);
        }

        retval = fsal_export_init(&myself->export);
        if (retval != 0) {
                LogMajor(COMPONENT_FSAL,
                         "out of memory for object");
                gsh_free(myself);
                return fsalstat(posix2fsal_error(retval), retval);
        }

        cloud_export_ops_init(myself->export.ops);
        cloud_handle_ops_init(myself->export.obj_ops);
        myself->export.up_ops = up_ops;

        retval = load_config_from_node(parse_node,
                                       &export_param,
                                       myself,
                                       true,
                                       &err_type);
        if (retval != 0)
                return fsalstat(ERR_FSAL_INVAL, 0);

        retval = fsal_attach_export(fsal_hdl, &myself->export.exports);
        if (retval != 0) {
                fsal_error = posix2fsal_error(retval);
                goto errout;    /* seriously bad */
        }
        myself->export.fsal = fsal_hdl;

        op_ctx->fsal_export = &myself->export;
        return fsalstat(ERR_FSAL_NO_ERROR, 0);

 errout:

        free_export_ops(&myself->export);
        gsh_free(myself);
        return fsalstat(fsal_error, retval);
}
