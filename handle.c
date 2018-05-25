#include <libgen.h>
#include "fsal_s3.h"

static char *_s3_mydirname(const char* path)
{
  if(!path || '\0' == path[0]){
    return NULL;
  }

  return dirname((char *) path);
}

static fsal_status_t _s3_check_object_access(const char *path, int mask)
{
    fsal_status_t st; 
    char attr[BUF_SIZE][BUF_SIZE];

}

static fsal_status_t _s3_check_parent_object_access(const char* path, int mask)
{
  char *parent = NULL;
  fsal_status_t st;

  if(0 == strcmp(path, "/") || 0 == strcmp(path, ".")){
    // path is mount point.
    fsalstat(ERR_FSAL_NO_ERROR, 0);
  }

  if(X_OK == (mask & X_OK)){
    for(parent = _s3_mydirname(path); ; parent = _s3_mydirname(parent)){
      if(strcmp(parent, ".") == 0){
        parent = "/";
      }
      /*
      if(st == _s3_check_object_access(parent, X_OK)){
        return st;
      } 
      */
      if(strcmp(parent, "/") == 0 || strcmp(parent, ".") == 0){
        break;
      }
    }
  }

  mask = (mask & ~X_OK);
  if(0 != mask){
    parent = _s3_mydirname(path);
    if(strcmp(parent, ".") == 0){
      parent = "/";
    }
    /*
    if(0 != _s3_check_object_access(parent, mask)){
      return st;
    }
    */
  }
  return st;
}

/*
 * Function: s3_lookup
 * Definition: Function to lookup a file
 * Calls gvfs_lookup
 * @param[in]   fparent   : fsal filehandle of parent
 * @param[in]   name      : name of file
 * @param[out]  ffile     : fsal filehandle
 * Return Value: fsal status
 */
fsal_status_t s3_lookup(struct fsal_obj_handle *fparent, const char *name, struct fsal_obj_handle **ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);

}

/*
 * Function: s3_create
 * Definition: Function to create a file
 * Calls gvfs_create
 * @param[in] fparent   : fsal filehandle of parent
 * @param[in] name      : name of file
 * @param[in] fattr     : fsal attributes of the file
 * @param[out] ffile     : fsal filehandle
 * Return Value: fsal status
 */
fsal_status_t s3_create(struct fsal_obj_handle *fparent,
                const char *name, struct attrlist *fattr, struct fsal_obj_handle **ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);

}

/*
 * Function: s3_makedir
 * Definition: Function to create a dir
 * Calls gvfs_makedir
 * @param[in]   fparent   : fsal filehandle of parent
 * @param[in]   name      : name of file
 * @param[in]   fattr     : fsal attributes of the file
 * @param[out]  ffile     : fsal filehandle
 * Return Value: fsal status
 */
fsal_status_t s3_makedir(struct fsal_obj_handle *fparent, const char *name, struct attrlist *fattr, struct fsal_obj_handle **ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);

}

fsal_status_t s3_makenode(struct fsal_obj_handle *dir_hdl,
                const char *name, object_file_type_t nodetype,
                fsal_dev_t *dev, struct attrlist *attrib, struct fsal_obj_handle **handle)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}

fsal_status_t s3_makesymlink(struct fsal_obj_handle *dir_hdl,
                const char *name, const char *link_path,
                struct attrlist *attrib,
                struct fsal_obj_handle **handle)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}

fsal_status_t s3_readsymlink(struct fsal_obj_handle *obj_hdl,
                struct gsh_buffdesc *link_content,
                bool refresh)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}

fsal_status_t s3_linkfile(struct fsal_obj_handle *obj_hdl,
                struct fsal_obj_handle *destdir_hdl,
                const char *name)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}

/*
 * Function: s3_read_dirents
 * Definition: Function to read a dir
 * Calls gvfs_readdir
 * @param[in] ffile     : fsal filehandle
 * @param[in] whence    : offset of dir to be read from
 * @param[in] dir_state : ?
 * @param[in] cb        : Callback function
 * @param[out] eof      : if EOF of dir then True
 * Return Value: fsal status
 * TODO: FIXME remove lseek, Change buf struct
*/
fsal_status_t s3_read_dirents(struct fsal_obj_handle *ffile,
                fsal_cookie_t *whence, void *dir_state,
                fsal_readdir_cb cb, bool *eof)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);

}

/*
 * Function: s3_renamefile
 * Definition: Function to rename a file
 * Calls gvfs_rename
 * @param[in] fparent_old   : fsal filehandle of old parent
 * @param[in] name_old      : Name of file to be renamed
 * @param[in] fparent_new   : fsal filehandle of new parent
 * @param[in] name_new      : New name of file
 * Return Value: fsal status
 */

fsal_status_t s3_renamefile(struct fsal_obj_handle *fparent_old,
                const char *name_old,
                struct fsal_obj_handle *fparent_new,
                const char *name_new)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

/*
 * Function: s3_getattrs
 * Definition: Function to get attrs of a file
 * Calls gvfs_getattrs
 * @param[in] ffile   : fsal filehandle of file
 * Return Value: fsal status
 */
fsal_status_t s3_getattrs(struct fsal_obj_handle *ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);

}

/*
 * Function: s3_setattrs
 * Definition: Function to set attrs of a file
 * Calls gvfs_setattrs
 * @param[in] ffile   : fsal filehandle of file
 * @param[in] fattrs  : fsal attrs
 * Return Value: fsal status
 */
fsal_status_t s3_setattrs(struct fsal_obj_handle *ffile, struct attrlist *fattrs)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);

}

/*
 * Function: s3_file_unlink
 * Definition: Function to unlink file from parent
 * Calls gvfs_node_unlink
 * @param[in] fparent   : fsal filehandle of parent dir
 * @param[in] name      : name of file
 * Return Value: fsal status
 */
fsal_status_t s3_file_unlink(struct fsal_obj_handle *fparent, const char *name)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

static fsal_status_t s3_handle_digest(const struct fsal_obj_handle *ffile,
                        fsal_digesttype_t output_type, struct gsh_buffdesc *fh_desc)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

static void s3_handle_to_key(struct fsal_obj_handle *ffile, struct gsh_buffdesc *fh_desc)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);


}

/*
 * Function: s3_lookup_path
 * Definition: Function to lookup root directory
 * Calls gvfs_lookup_path
 * @param[in] exp_hdl   : fsal export object
 * @param[in] ffile     : fsal filehandle of file
 * Return Value: fsal status
 */
fsal_status_t s3_lookup_path(struct fsal_export *exp_hdl,
                const char *path, struct fsal_obj_handle **ffile)
{
    LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    fsal_status_t st;
  
    st = _s3_check_parent_object_access(path, X_OK);
    if (FSAL_IS_ERROR(st)) {
        goto err;
    }

    st = _s3_check_object_access(path, X_OK);
    if (FSAL_IS_ERROR(st)) {
        goto err;
    }
 
    return fsalstat(ERR_FSAL_NO_ERROR, 0);

err:
    return st;
}

fsal_status_t s3_create_handle(struct fsal_export *exp_hdl,
                struct gsh_buffdesc *hdl_desc, struct fsal_obj_handle **handle)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);

    return fsalstat(ERR_FSAL_NO_ERROR, 0);
}


void s3_handle_ops_init(struct fsal_obj_ops *ops)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    ops->lookup         = s3_lookup;
    ops->readdir        = s3_read_dirents;
    ops->create         = s3_create;
    ops->mkdir          = s3_makedir;
    ops->mknode         = s3_makenode;
    ops->symlink        = s3_makesymlink;
    ops->readlink       = s3_readsymlink;
    ops->getattrs       = s3_getattrs;
    ops->setattrs       = s3_setattrs;
    ops->link           = s3_linkfile;
    ops->rename         = s3_renamefile;
    ops->unlink         = s3_file_unlink;
    ops->open           = s3_open;
    ops->status         = s3_status;
    ops->read           = s3_read;
    ops->write          = s3_write;
    ops->commit         = s3_commit;
    ops->lock_op        = s3_lock_op;
    ops->close          = s3_close;
    ops->lru_cleanup    = s3_lru_cleanup;
    ops->handle_digest  = s3_handle_digest;
    ops->handle_to_key  = s3_handle_to_key;
}
