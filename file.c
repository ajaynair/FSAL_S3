#include "fsal_s3.h"

/*
 * Function: s3_open
 * Definition: Function to open a file
 * Calls gvfs_open
 * @param[in] ffile:  fsal filehandle
 * @param[in] fflags: fsal openflags(readonly, readwrite etc)
 * Return Value: fsal status
 */
fsal_status_t s3_open(struct fsal_obj_handle *ffile, fsal_openflags_t fflags)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);
}

/*
 * Function: s3_status
 * Definition: Function to get status of a file (opened/closed etc)
 * Calls gvfs_status
 * @param[in] ffile: fsal filehandle
 * Return Value: fsal status
 */
fsal_openflags_t s3_status(struct fsal_obj_handle *ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    fsal_openflags_t flags = 0;
    return flags;
}

/*
 * Function: s3_read
 * Definition: Function to read from a file
 * Calls gvfs_read
 * @param[in] ffile         : fsal filehandle
 * @param[in] offset        : offset to start reading from
 * @param[in] buffer_size   : size of buffer
 * @param[in] buffer        : buffer to fill the read data
 * @param[out] read_amount  : amount of data read
 * @param[out] end_of_file  : if EOF then True
 * Return Value: fsal status
 */
fsal_status_t s3_read(struct fsal_obj_handle *ffile,
                uint64_t offset, size_t buffer_size, void *buffer,
                size_t *read_amount, bool *end_of_file)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

/*
 * Function: s3_write
 * Definition: Function to write from a file
 * Calls gvfs_write
 * @param[in] ffile         : fsal filehandle
 * @param[in] offset        : offset to start writing to
 * @param[in] buffer_size   : size of buffer
 * @param[in] buffer        : data to be written
 * @param[out] write_amount : amount of data written
 * @param[out] fsal_stable  : ?
 * Return Value: fsal status
 * TODO: Find what is fsal_stable
 */
fsal_status_t s3_write(struct fsal_obj_handle *ffile,
                uint64_t offset, size_t buffer_size, void *buffer,
                size_t *write_amount, bool *fsal_stable)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

fsal_status_t s3_commit(struct fsal_obj_handle *obj_hdl,    /* sync */
                off_t offset, size_t len)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

fsal_status_t s3_lock_op(struct fsal_obj_handle *ffile,
                void *p_owner, fsal_lock_op_t lock_op,
                fsal_lock_param_t *request_lock, fsal_lock_param_t *conflicting_lock)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}


/*
 * Function: s3_close
 * Definition: Function to close a file
 * Calls gvfs_close
 * @param[in] ffile: fsal filehandle
 * Return Value: fsal status
 */
fsal_status_t s3_close(struct fsal_obj_handle *ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

fsal_status_t s3_lru_cleanup(struct fsal_obj_handle *ffile,
                 lru_actions_t requests)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}
