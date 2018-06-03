# FSAL-S3 - A NFS-Ganesha FSAL for Amazon S3 (v0.1)

## NFS-Ganesha with FSAL-S3 uses Amazon S3 as a backed for NFS.

### Explanation

1. Each NFS filetype has a corresponding Amazon S3 object to store its data.
  * Regular file data is stored as it is to an object correspoding to it.
  * Directory entries are stored as a list of dirents(Explained below) in an object corresponding to it.

#### Note that the current implementation only supports regular files and directories

Directory entry data structure:
```c
typedef struct  {
  char *filename;
  char *oid;
} s3_dirent;
```

A directory entry in an object is stored in string format as:
__<magic-string><separator><filename><separator><object-id><separator><magic-string><separator>__

Where:
  magic-string is "s3magic"
  separator is "@"

Detailed example
Consider the following directory structure:
    
                        / 
                      /   \
                     /     \
                 dir11     dir12 
                 / \    
                /   \    
            dir21  file22 

The Amazon S3 will contain 5 objects, 1 each for '/', 'dir11', 'dir12', dir21' and 'file22'

For the example let's assume the following mapping:
 ---------------------------
|     file     | object-id  |
 --------------|------------
|      /       |      0     |
|   /dir11     |     1111   |
|   /dir12     |     1112   |
| /dir11/dir21 |     1113   |
|/dir11/file22 |     1114   |
 ---------------------------

Data in object with object-id 0 (i.e. object corresponding to file '/'):
  __ s3magic@dir11@1111@s3magic@s3magic@dir12@1112@s3magic@ __

Data in object with object-id 1111:
  **s3magic@dir21@1113@s3magic@s3magic@file22@1114@s3magic@**

#### Note that data in object with object-id 1114 will be the same as '/dir11/file22' file content.
