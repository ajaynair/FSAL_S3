#include <libgen.h>
#include "fsal_cloud.h"

/*
static char *_cloud_mydirname(const char* path)
{
  if(!path || '\0' == path[0]){
    return NULL;
  }

  return dirname((char *) path);
}

static fsal_status_t _cloud_check_object_access(const char *path, int mask)
{
    // fsal_status_t st; 
    // char attr[BUF_SIZE][BUF_SIZE];
    return fsalstat(ERR_FSAL_NO_ERROR, 0);
}

static fsal_status_t _cloud_check_parent_object_access(const char* path, int mask)
{
  char *parent = NULL;
  fsal_status_t st;

  if(0 == strcmp(path, "/") || 0 == strcmp(path, ".")){
    // path is mount point.
    fsalstat(ERR_FSAL_NO_ERROR, 0);
  }

  if(X_OK == (mask & X_OK)){
    for(parent = _cloud_mydirname(path); ; parent = _cloud_mydirname(parent)){
      if(strcmp(parent, ".") == 0){
        parent = "/";
      }
      if(st == _cloud_check_object_access(parent, X_OK)){
        return st;
      } 
      if(strcmp(parent, "/") == 0 || strcmp(parent, ".") == 0){
        break;
      }
    }
  }

  mask = (mask & ~X_OK);
  if(0 != mask){
    parent = _cloud_mydirname(path);
    if(strcmp(parent, ".") == 0){
      parent = "/";
    }

    if(0 != _cloud_check_object_access(parent, mask)){
      return st;
    }
  }
  return st;
}
*/

/*
 * Function: cloud_lookup
 * Definition: Function to lookup a file
 * Calls gvfs_lookup
 * @param[in]   fparent   : fsal filehandle of parent
 * @param[in]   name      : name of file
 * @param[out]  ffile     : fsal filehandle
 * Return Value: fsal status
 */
fsal_status_t cloud_lookup(struct fsal_obj_handle *fparent, const char *name, struct fsal_obj_handle **ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    data_pointer *dp = NULL;
    cloud_fsal_obj_handle *sparent = NULL;
    cloud_fsal_obj_handle *schild = NULL;
    char oid[BUF_SIZE] = { 0 };

   sparent = container_of(fparent, cloud_fsal_obj_handle, obj_handle);
   sprintf(oid, "%s%s", sparent->oid, name);

    dp = calloc(1, sizeof(data_pointer));
    get_metadata_count(&(dp->metadata_count));
    dp->metadata = calloc(dp->metadata_count, sizeof(dict));   
 
    get_tmp_file(&(dp->fp)); 
    get_object(BUCKETNAME, oid, dp);

   schild = calloc(1, sizeof(cloud_fsal_obj_handle));
    schild->oid = strdup(oid);
    schild->obj_handle.ops = sparent->obj_handle.ops;
    schild->obj_handle.fsal = sparent->obj_handle.fsal;
    _fill_attrlist(dp->metadata, &(schild->obj_handle.attributes), &(schild->obj_handle.type));

   schild->obj_handle.fs = sparent->obj_handle.fs;
    *ffile = &(schild->obj_handle);

    return fsalstat(ERR_FSAL_NO_ERROR, 0);

}

/*
 * Function: cloud_create
 * Definition: Function to create a file
 * Calls gvfs_create
 * @param[in] fparent   : fsal filehandle of parent
 * @param[in] name      : name of file
 * @param[in] fattr     : fsal attributes of the file
 * @param[out] ffile     : fsal filehandle
 * Return Value: fsal status
 */
fsal_status_t cloud_create(struct fsal_obj_handle *fparent,
                const char *name, struct attrlist *fattr, struct fsal_obj_handle **ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
   char dir_oid[BUF_SIZE] = { 0 };
   cloud_fsal_obj_handle *sparent = NULL;
   cloud_fsal_obj_handle *schild = NULL;
   data_pointer *dp = NULL;

   sparent = container_of(fparent, cloud_fsal_obj_handle, obj_handle);
   sprintf(dir_oid, "%s%s", sparent->oid, name);

   dp = calloc(1, sizeof(data_pointer));
   get_metadata_count(&(dp->metadata_count));
   set_default_metadata(&(dp->metadata), DIRECTORY_T, dir_oid);
   _refill_metadata(fattr, dp->metadata);

   put_object(BUCKETNAME, dir_oid, dp);  

   get_object_metadata(BUCKETNAME, dir_oid, dp);
   
   schild = calloc(1, sizeof(cloud_fsal_obj_handle));
   _fill_attrlist(dp->metadata, &(schild->obj_handle.attributes), &(schild->obj_handle.type));
   schild->oid = strdup(dir_oid);
   schild->obj_handle.ops = sparent->obj_handle.ops;
   schild->obj_handle.fsal = sparent->obj_handle.fsal;  
   schild->obj_handle.fs = sparent->obj_handle.fs;
 
   *ffile = &(schild->obj_handle);
   return fsalstat(ERR_FSAL_NO_ERROR, 0);
}

/*
 * Function: cloud_makedir
 * Definition: Function to create a dir
 * Calls gvfs_makedir
 * @param[in]   fparent   : fsal filehandle of parent
 * @param[in]   name      : name of file
 * @param[in]   fattr     : fsal attributes of the file
 * @param[out]  ffile     : fsal filehandle
 * Return Value: fsal status
 */
fsal_status_t cloud_makedir(struct fsal_obj_handle *fparent, const char *name, struct attrlist *fattr, struct fsal_obj_handle **ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
   char dir_oid[BUF_SIZE] = { 0 };
   cloud_fsal_obj_handle *sparent = NULL;
   cloud_fsal_obj_handle *schild = NULL;
   data_pointer *dp = NULL;

   sparent = container_of(fparent, cloud_fsal_obj_handle, obj_handle);
   sprintf(dir_oid, "%s%s/", sparent->oid, name);
 
   dp = calloc(1, sizeof(data_pointer));
   get_metadata_count(&(dp->metadata_count));
   set_default_metadata(&(dp->metadata), DIRECTORY_T, dir_oid);
   _refill_metadata(fattr, dp->metadata);

   put_object(BUCKETNAME, dir_oid, dp);  

   get_object_metadata(BUCKETNAME, dir_oid, dp);
   
   schild = calloc(1, sizeof(cloud_fsal_obj_handle));
   _fill_attrlist(dp->metadata, &(schild->obj_handle.attributes), &(schild->obj_handle.type));
   schild->oid = strdup(dir_oid);
   schild->obj_handle.ops = sparent->obj_handle.ops;
   schild->obj_handle.fsal = sparent->obj_handle.fsal;  
   schild->obj_handle.fs = sparent->obj_handle.fs;
 
   *ffile = &(schild->obj_handle);
   return fsalstat(ERR_FSAL_NO_ERROR, 0);

}

fsal_status_t cloud_makenode(struct fsal_obj_handle *dir_hdl,
                const char *name, object_file_type_t nodetype,
                fsal_dev_t *dev, struct attrlist *attrib, struct fsal_obj_handle **handle)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}

fsal_status_t cloud_makesymlink(struct fsal_obj_handle *dir_hdl,
                const char *name, const char *link_path,
                struct attrlist *attrib,
                struct fsal_obj_handle **handle)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}

fsal_status_t cloud_readsymlink(struct fsal_obj_handle *obj_hdl,
                struct gsh_buffdesc *link_content,
                bool refresh)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}

fsal_status_t cloud_linkfile(struct fsal_obj_handle *obj_hdl,
                struct fsal_obj_handle *destdir_hdl,
                const char *name)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    LogCrit(COMPONENT_FSAL, "Not implemented");
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}

/*
 * Function: cloud_read_dirents
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
fsal_status_t cloud_read_dirents(struct fsal_obj_handle *ffile,
                fsal_cookie_t *whence, void *dir_state,
                fsal_readdir_cb cb, bool *eof)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
   printf("dirent");
   char prefix[BUF_SIZE] = { 0 };
   int i = 0;

   cloud_fsal_obj_handle *shandle = NULL;
   
   shandle = container_of(ffile, cloud_fsal_obj_handle, obj_handle);
   if (shandle->oid[strlen(shandle->oid) - 1] == '/') {
      strcpy(prefix, shandle->oid);
   }
   else {
      strncpy(prefix, shandle->oid, strlen(shandle->oid) - 1);
      prefix[strlen(shandle->oid) - 1] = '\0';
   }
 
   object_list *objects = calloc(1, sizeof(object_list));
   list_objects(BUCKETNAME, prefix, objects);

  for (i = 0; i < objects->object_count; i++)
  {
    printf("dirent: %s\n", objects->objects[i]);
    if (!cb(objects->objects[i], dir_state, objects->object_count)) goto out;
  }

out:
   *eof = true;
   return fsalstat(ERR_FSAL_NO_ERROR, 0);
}

/*
 * Function: cloud_renamefile
 * Definition: Function to rename a file
 * Calls gvfs_rename
 * @param[in] fparent_old   : fsal filehandle of old parent
 * @param[in] name_old      : Name of file to be renamed
 * @param[in] fparent_new   : fsal filehandle of new parent
 * @param[in] name_new      : New name of file
 * Return Value: fsal status
 */

fsal_status_t cloud_renamefile(struct fsal_obj_handle *fparent_old,
                const char *name_old,
                struct fsal_obj_handle *fparent_new,
                const char *name_new)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

/*
 * Function: cloud_getattrs
 * Definition: Function to get attrs of a file
 * Calls gvfs_getattrs
 * @param[in] ffile   : fsal filehandle of file
 * Return Value: fsal status
 */
fsal_status_t cloud_getattrs(struct fsal_obj_handle *ffile)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
   cloud_fsal_obj_handle *shandle = NULL;
   data_pointer *dp = NULL;

   shandle = container_of(ffile, cloud_fsal_obj_handle, obj_handle);

   dp = calloc(1, sizeof(data_pointer));

   get_metadata_count(&(dp->metadata_count));
   dp->metadata = calloc(dp->metadata_count, sizeof(dict));

   get_object_metadata(BUCKETNAME, shandle->oid, dp);

    _fill_attrlist(dp->metadata, &(ffile->attributes), &(ffile->type));
   return fsalstat(ERR_FSAL_NO_ERROR, 0);
}

/*
 * Function: cloud_setattrs
 * Definition: Function to set attrs of a file
 * Calls gvfs_setattrs
 * @param[in] ffile   : fsal filehandle of file
 * @param[in] fattrs  : fsal attrs
 * Return Value: fsal status
 */
fsal_status_t cloud_setattrs(struct fsal_obj_handle *ffile, struct attrlist *fattrs)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
   cloud_fsal_obj_handle *shandle = NULL;
   data_pointer *dp = NULL;

   shandle = container_of(ffile, cloud_fsal_obj_handle, obj_handle);

   dp = calloc(1, sizeof(data_pointer));
   get_metadata_count(&(dp->metadata_count));
   dp->metadata = calloc(dp->metadata_count, sizeof(dict));

   get_object_metadata(BUCKETNAME, shandle->oid, dp);
   _refill_metadata(fattrs, dp->metadata);

   put_object_metadata(BUCKETNAME, shandle->oid, dp);
   return fsalstat(ERR_FSAL_NO_ERROR, 0);

}

/*
 * Function: cloud_file_unlink
 * Definition: Function to unlink file from parent
 * Calls gvfs_node_unlink
 * @param[in] fparent   : fsal filehandle of parent dir
 * @param[in] name      : name of file
 * Return Value: fsal status
 */
fsal_status_t cloud_file_unlink(struct fsal_obj_handle *fparent, const char *name)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);

   return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

static fsal_status_t cloud_handle_digest(const struct fsal_obj_handle *ffile,
                        fsal_digesttype_t output_type, struct gsh_buffdesc *fh_desc)
{
   cloud_fsal_obj_handle *myself = NULL;
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);

   myself = container_of(ffile, cloud_fsal_obj_handle, obj_handle);
   memcpy(fh_desc->addr, myself->oid, strlen(myself->oid));
   fh_desc->len = strlen(myself->oid);

   return fsalstat(ERR_FSAL_NO_ERROR, 0);


}

static void cloud_handle_to_key(struct fsal_obj_handle *ffile, struct gsh_buffdesc *fh_desc)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
   cloud_fsal_obj_handle *myself = container_of(ffile, cloud_fsal_obj_handle, obj_handle);
   fh_desc->addr = myself->oid;
   fh_desc->len = strlen(myself->oid);
}

/*
 * Function: cloud_lookup_path
 * Definition: Function to lookup root directory
 * Calls gvfs_lookup_path
 * @param[in] exp_hdl   : fsal export object
 * @param[in] ffile     : fsal filehandle of file
 * Return Value: fsal status
 */
fsal_status_t cloud_lookup_path(struct fsal_export *exp_hdl,
                const char *path, struct fsal_obj_handle **ffile)
{
    LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    data_pointer *dp = NULL;
    cloud_fsal_obj_handle *shandle = NULL;

    /* TODO Check access */
    dp = calloc(1, sizeof(data_pointer));
    get_metadata_count(&(dp->metadata_count));
    dp->metadata = calloc(dp->metadata_count, sizeof(dict));   
 
    get_tmp_file(&(dp->fp)); 
    get_object(BUCKETNAME, ROOTOID, dp);

    shandle = calloc(1, sizeof(cloud_fsal_obj_handle));
    shandle->oid = strdup(ROOTOID);
    shandle->obj_handle.ops = exp_hdl->obj_ops;

    shandle->obj_handle.fsal = exp_hdl->fsal;
    _fill_attrlist(dp->metadata, &(shandle->obj_handle.attributes), &(shandle->obj_handle.type));

    *ffile = &(shandle->obj_handle);

    return fsalstat(ERR_FSAL_NO_ERROR, 0);
}

fsal_status_t cloud_create_handle(struct fsal_export *exp_hdl,
                struct gsh_buffdesc *hdl_desc, struct fsal_obj_handle **handle)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    return fsalstat(ERR_FSAL_INVAL, EINVAL);
}


void cloud_handle_ops_init(struct fsal_obj_ops *ops)
{
   LogCrit(COMPONENT_FSAL, "%s", __FUNCTION__);
    ops->lookup         = cloud_lookup;
    ops->readdir        = cloud_read_dirents;
    ops->create         = cloud_create;
    ops->mkdir          = cloud_makedir;
    ops->mknode         = cloud_makenode;
    ops->symlink        = cloud_makesymlink;
    ops->readlink       = cloud_readsymlink;
    ops->getattrs       = cloud_getattrs;
    ops->setattrs       = cloud_setattrs;
    ops->link           = cloud_linkfile;
    ops->rename         = cloud_renamefile;
    ops->unlink         = cloud_file_unlink;
    ops->open           = cloud_open;
    ops->status         = cloud_status;
    ops->read           = cloud_read;
    ops->write          = cloud_write;
    ops->commit         = cloud_commit;
    ops->lock_op        = cloud_lock_op;
    ops->close          = cloud_close;
    ops->lru_cleanup    = cloud_lru_cleanup;
    ops->handle_digest  = cloud_handle_digest;
    ops->handle_to_key  = cloud_handle_to_key;
}
