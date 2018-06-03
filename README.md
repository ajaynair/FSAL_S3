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
`<magic-string><separator><filename><separator><object-id><separator><magic-string><separator>`

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
  `s3magic@dir11@1111@s3magic@s3magic@dir12@1112@s3magic@`

Data in object with object-id 1111:
  `s3magic@dir21@1113@s3magic@s3magic@file22@1114@s3magic@`

#### Note that data in object with object-id 1114 will be the same as '/dir11/file22' file content.

### Source code directory structure

A very very brief introduction to the source code directory structures:

 ------------------------------------------------------------------------------------------------------
|     file                    |                              explanation                               |
 -----------------------------|------------------------------------------------------------------------
| README.md                   | This file!                                                             |
| fsal_s3.h                   | Defines FSAL-S3 related data structures.                               |
| main.c                      | Defines {Cons}/{Des}tructor and other FSAL_S3 static data              |
| export.c                    | FSAL-S3 export related functions (For e.g. create_export)              |
| handle.c                    | FSAL-S3 handle related functions (For e.g. lookup)                     |
| file.c                      | FSAL-S3 file related functions (For e.g. read)                         |
| Makefile                    | Makefile used to recursively compile the source code                   |
| s3_conector/                | Enables FSAL-S3 to connect to the Amazon S3 (Currently uses libs3)     |
| s3_conector/s3_connector.h  | Defines data structures used to connect to Amazon S3                   |
| s3_conector/s3_connector.c  | Defines functions to talk to Amazon S3 (For e.g. get, put, delete)     |
| s3_conector/Makefile        | Makefile used to recursively compile the source code                   |
| obj_structs/                | Defines the data to be stored in Amazon S3                             |
| obj_structs/obj_structs.h   | Defines data structure to store data in Amazon S3 (For e.g. dirent)    |
| obj_structs/obj_structs.c   | Defines functions to manipulate object data (For e.g. pack_dirent)     |
| obj_structs/Makefile        | Makefile used to recursively compile the source code                   |
| cli/                        | Defines several CLI used manipulate objects in Amazon S3               |
| cli/mkdir_p.c               | Used to create directory in Amazon S3 that can be used as an export    |
| cli/Makefile                | Makefile used to recursively compile the source code                   |
| common/                     | Defines functions and other information used by all other source files |
| common/common.h             | Defines generic data structures that are used by all other source files|
| common/common.c             | Defines functions that are used by all other source files              |
| common/Makefile             | Makefile used to recursively compile the source code                   |
 -----------------------------|------------------------------------------------------------------------
