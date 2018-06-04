# FSAL-S3 - A NFS-Ganesha FSAL for Amazon S3 (v0.1)

## NFS-Ganesha with FSAL-S3 uses Amazon S3 as a backed for NFS.

### Explanation

1. Each NFS filetype has a corresponding Amazon S3 object to store its data.
  * Regular file data is stored as it is to an object correspoding to it.
  * Directory entries are stored as a list of dirents(Explained below) in an object corresponding to it.

##### Note that the current implementation only supports regular files and directories

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
  magic-string is `"s3magic"`
  separator is `"@"`

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

##### Note that data in object with object-id 1114 will be the same as '/dir11/file22' file content.

### Source code directory structure

A very very brief introduction to the source code directory structures:

 -------------------------------------------------------------------------------------------------------
|     file                     |                              explanation                               |
 ------------------------------|------------------------------------------------------------------------
| README.md                    | This file!                                                             |
| fsal_s3.h                    | Defines FSAL-S3 related data structures.                               |
| main.c                       | Defines {Cons}/{Des}tructor and other FSAL_S3 static data              |
| export.c                     | FSAL-S3 export related functions (For e.g. create_export)              |
| handle.c                     | FSAL-S3 handle related functions (For e.g. lookup)                     |
| file.c                       | FSAL-S3 file related functions (For e.g. read)                         |
| Makefile                     | Makefile used to recursively compile the source code                   |
| s3_conector/                 | Enables FSAL-S3 to connect to the Amazon S3 (Currently uses libs3)     |
| s3_conector/s3_connector.h   | Defines data structures used to connect to Amazon S3                   |
| s3_conector/s3_connector.c   | Defines functions to talk to Amazon S3 (For e.g. get, put, delete)     |
| s3_conector/Makefile         | Makefile used to recursively compile the source code                   |
| s3_structures/               | Defines the structures of data to be stored in Amazon S3               |
| s3_structures/s3_structures.h| Defines data structures to store data in Amazon S3 (For e.g. dirent)   |
| s3_structures/s3_structures.c| Defines functions to manipulate data structures(For e.g. pack_dirent)  |
| s3_structures/Makefile       | Makefile used to recursively compile the source code                   |
| cli/                         | Defines several CLI used manipulate objects in Amazon S3               |
| cli/mkdir_p.c                | Used to create directory in Amazon S3 that can be used as an export    |
| cli/Makefile                 | Makefile used to recursively compile the source code                   |
| common/                      | Defines functions and other information used by all other source files |
| common/common.h              | Defines generic data structures that are used by all other source files|
| common/common.c              | Defines functions that are used by all other source files              |
| common/Makefile              | Makefile used to recursively compile the source code                   |
 ------------------------------------------------------------------------------------------------------

### s3-connector

1) s3-connector can be used by its caller (FSAL-S3 in our case) to connect to Amazon S3 and manipulate S3 object data.

2) s3-connector uses a local file to:
  . Store object data during Get operation
  . Retrieve object data during Put operation

3) When the caller wants to Put(create) an object on Amazon S3 it should:
  . Create a temporary file
  . Write the intended object data to the file
  . Pass the file pointer to s3-connector with read access
s3-connector will read data from the file and store it in the Amazon S3 object

3) When the caller wants to Get(read) an object from Amazon S3 it should:
  . Create a temporary file
  . Pass the file pointer to s3-connector with write access
s3-connector will write data from the object to the file

#### Data structures
. s3-connector uses the `data_pointer` data structure to store data.
```c
typedef struct {
  FILE   *fp;
  dict   *metadata;
  size_t metadata_count;
} data_pointer;
```
fp - Stores the local-file pointer which stores the data of an object
metadata - A dictionary (name-value pair) which stores the attributes of an object
metadata_count - Number of metadata that are stored
Note that `data_pointer` does not store file_size as file size can be calculated using fp.

#### APIs
`int get_object(char *bucketName, char *objectName, data_pointer *data)`
Gets an object(with its data and metadata) from Amazon S3.
  bucketName [in]:  Name of the bucket in which the object to be retrieved is present
  objectName [in]:  Name of the object to be retrieved
  data       [out]: data_pointer in which the data of the object will be stored  

`int put_object(char *bucketName, char *objectName, data_pointer *data)`
Puts an object(with its data and metadata) on Amazon S3.
  bucketName [in]: Name of the bucket in which the object to be retrieved is present
  objectName [in]: Name of the object to be retrieved
  data:      [in]: data_pointer in which the data of the object to be put is stored  

`int delete_object(char *bucketName, char *objName)`
Delete an object from Amazon S3
  bucketName [in]: Name of the bucket in which the object to be deleted is present
  objectName [in]: Name of the object to be deleted

`int get_object_metadata(char *bucketName, char *objName, data_pointer *data)`
Gets object metadata from Amazon S3.
  bucketName [in]:  Name of the bucket in which the object whose metadata is to be retrieved is present
  objectName [in]:  Name of the object whose metadata is to be retrieved
  data       [out]: data_pointer in which the metadata of the object will be stored  

`int put_object_metadata(char *bucketName, char *objName, data_pointer *data)`
Put object metadata from Amazon S3.
  bucketName [in]:  Name of the bucket in which the object whose metadata is to be put is present
  objectName [in]:  Name of the object whose metadata is to be put
  data       [in]: data_pointer in which the metadata of the object to be put is stored 

### s3-structures

s3-structures defines the structure of the data to be stored in Amazon S3

#### Data structures

Directory entry data structure. 
Each directory has a list of dirent (in string format) stored in it, one for each file within the directory:
```c
typedef struct  {
  char *filename;
  char *oid;
} dirent;
```

Metadata name data structure:
Metadata are stored in an object in name-value pairs. The metadata_names define the names of the metadata. Note that each object has exactly `METADATA_COUNT` count of metadata.

```c
typedef enum {
  FILETYPE,
  FILESIZE,
  FSID,   
  FILEID, 
  MODE,   
  NUMLINKS,
  OWNER,   
  GROUP,   
  ATIME,   
  CTIME,   
  CHGTIME, 
  MTIME,   
  SPACEUSED,
  METADATA_COUNT
} metadata_name;
```

FILETYPE metadata name can only have a finite metadata value. filetype defined the list of values it can have. Note that when other filetypes will be supported, the list will increase.
Filetype data structure:
```c
typedef enum {
  REGULARFILE,
  DIRECTORY 
} filetype;
```

All other metadata does not require a data structure for its metadata value

```c
#define OIDLEN        20               /* Maximum length of the object ID */
#define ROOTOID       "0"              /* OID of object corresponding to '/' */
#define BUCKETNAME    "fsals3bucket"   /* Name of the Amazon S3 bucket to be used */
#define MAGIC         "FSALS3MAGICSTR" /* A string used in dirents in string format */
#define SEPARATOR     '@'              /* Separator to separate members of dirent in string format */
#define S3MAXOBJSIZE  999999           /* Maximum object size supported. Current a large random value */
```

#### APIs


TODO:
1) OID generator
2) 
