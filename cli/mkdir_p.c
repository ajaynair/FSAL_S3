#include <unistd.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "obj_structs.h"
#include "s3_connector.h"
#include "common.h"

static void _print_help()
{
    printf("s3_mkdir_p <DirectoryName>\n");
    exit(0);
}

/* TODO: 1) Return int 
 *       2) avoid strcpy
*/
static void _mkdir_p(char *dir_path) 
{
    int ret = 0;
    int dir_index = 0;
    char name[BUF_SIZE] = {0};
    char child[BUF_SIZE] = {0};
    char parent[BUF_SIZE] = {0};
    s3_dirent *dirent = NULL;
    char dirent_str[BUF_SIZE] = {0};
    data_pointer *dp = NULL; 

    dp = calloc(1, sizeof(data_pointer));

    get_tmp_file(&(dp->fp));
    get_metadata_count(&(dp->metadata_count));
    set_default_metadata(&(dp->metadata), DIRECTORY_T, ROOTOID);

    /* TODO: Put object should check if fp is null and if it is NULL create an empty object */
    put_object(BUCKETNAME, ROOTOID, dp);
    clean_data_pointer(dp);

    strcpy(parent, ROOTOID); 

    dirent = calloc(1, sizeof(s3_dirent));

    do {
        ret = get_next_path_component(dir_path, &dir_index, name);
        create_random_string(child, OIDLEN);
        pack_dirent(name, child, &dirent);
        dirent_to_str(dirent, dirent_str);

        get_tmp_file(&(dp->fp));
        get_object(BUCKETNAME, parent, dp);

        next_dirent_to_file(dp->fp, dirent_str);
        inc_filesize_metadata(dp->metadata, strlen(dirent_str));

        put_object(BUCKETNAME, parent, dp);
        clean_data_pointer(dp);

        get_metadata_count(&(dp->metadata_count));
        set_default_metadata(&(dp->metadata), DIRECTORY_T, child);
        put_object(BUCKETNAME, child, dp);

        strcpy(parent, child);
     } while (ret);

     free(dp);
     free(dirent); 
}

int main(int argc, char **argv)
{
    char *dir_path = argv[1];

    if (argc != 2)
        _print_help();

    /* TODO: Sanity check */
    if (dir_path[0] != '/') {
        printf("Directory name should start with '/'");
        return 0;
    }

    init_s3_connector();  

    _mkdir_p(dir_path);

    deinit_s3_connector();  

    return 0;
}
