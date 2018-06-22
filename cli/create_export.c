#include <unistd.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "cloud_structures.h"
#include "cloud_connector.h"
#include "common.h"

static void _print_help()
{
    printf("create_export \n");
    exit(0);
}

/* TODO: 1) Return int 
 *       2) avoid strcpy
*/
static void _create_export() 
{
    data_pointer *dp = NULL; 

    dp = calloc(1, sizeof(data_pointer));

    get_tmp_file(&(dp->fp));
    get_metadata_count(&(dp->metadata_count));
    set_default_metadata(&(dp->metadata), DIRECTORY_T, ROOTOID);

    /* TODO: Put object should check if fp is null and if it is NULL create an empty object */
    put_object(BUCKETNAME, ROOTOID, dp);
    free(dp);
}

int main(int argc, char **argv)
{
    if (argc != 1)
        _print_help();

    cloud_init();  

    _create_export();

    cloud_deinit();  

    return 0;
}
