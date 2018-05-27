#include <stdio.h>
#include <stdlib.h>

#include "dirent.h"

void print_help()
{
    printf("s3_mkdir_p <DirectoryName>\n");
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        print_help();
    }
    if (argv[1][0] != '/') {
        printf("Directory name should start with '/'");
    }

    return 0;
}
