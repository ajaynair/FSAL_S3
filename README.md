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
  char *fileName;
  char *objectName;
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

The attributes of a file is stored as user-defined metadatas of its corresponding object.

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
  char *fileName;
  char *objectName;
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

Data structure for metadata value of 'FILETYPE' metadata
Filetype data structure:
```c
typedef enum {
  REGULARFILE,
  DIRECTORY 
} filetype;
```

No other metadata require a data structure for its metadata value

Other static values
```c
#define OIDLEN        20               /* Maximum length of the object ID */
#define ROOTOID       "0"              /* OID of object corresponding to '/' */
#define BUCKETNAME    "fsals3bucket"   /* Name of the Amazon S3 bucket to be used */
#define MAGIC         "FSALS3MAGICSTR" /* A string used in dirents in string format */
#define SEPARATOR     '@'              /* Separator to separate members of dirent in string format */
#define S3MAXOBJSIZE  999999           /* Maximum object size supported. Current a large random value */
```

#### APIs
Pack file name and object name to create a dirent
`int pack_dirent(char fileName[], char objectName[], s3_dirent **dirent)`
  fileName   [in]:  Name of the file  
  objectName [in]:  Name of the object which stores the data of the file 
  dirent     [out]: dirent structure to be packed 

Unpack a dirent to get file name and object name
`int unpack_dirent(s3_dirent *dirent, char fileName[], char objectName[])`
  dirent     [in]:   dirent to unpack 
  fileName   [out]:  Name of the file
  objectName [out]:  Name of the object which stores the data of the file  

Converts the dirent to string and appends it to the file
`int append_dirent_to_file(FILE *fp, struct *dirent)`
  dirent  [in]:   dirent to unpack 
  fp      [in]:   File pointer of file on which the dirent is to be appended 

Read next dirent from the file starting from the current fp position
`int read_next_dirent_from_file(FILE *fp, s3_dirent **dirent)`
  fp       [in]:  File pointer of file on which the the next dirent is to be read
  dirent   [out]: dirent strucute in which the next dirent will be filled

Search the file to find the dirent with particular filename
`int find_dirent_in_file(FILE *fp, char filename[], s3_dirent **dirent)`
  fp       [in]:  File pointer of file which is to be searched 
  fileName [in]:  Filename to be searched in the dirent
  dirent   [out]: dirent structure to be filled

Set dict with default metadata names and values. 
Note that the values to be filled for Filetype and FileId are provided as parameter. For all other metadata default values are used.
`int set_default_metadata(char fileType[], char fileId[], dict metadata[])`
  fileType [in]:   Value of file type to be stored in the metadata array
  fileId   [in]:   Value of File Id to be stored in the metadata arrat
  dict     [out]:  An array of metadata on which the default metadata name and value will be stored 

Replace the metadata of name 'mname' with 'mvalue'
`void replace_metadata_value(dict metadata[], char mname[], char mvalue[])`
  metadata [in]: The array of metadata
  mname    [in]: The name of metadata whose value is to be changed
  mvalue   [in]: The new value of the metadata

Get the metadata value of metadata name 'mname'
`void get_metadata_value(dict metadata[], char mname[], char mvalue[])`
  metadata [in]:  The array of metadata
  mname    [in]:  The name of metadata whose value is to be retrieved
  mvalue   [out]: The value of the metadata

Get the count of metadata
`void get_metadata_count(size_t *count)`
count [out]: The number of metadata supported.

Increment filesize metadata by the given value
`void inc_filesize_metadata(dict metadata[], size_t inc_val)`
  metadata [in]:  The array of metadata
  inc_val  [in]:  The value by which the filesize is to be increased 

#### Important static(non-API) function
Converts dirent structure to the string format
`static int _dirent_to_str(s3_dirent *dirent, char dirent_str[])`
  dirent     [in]:  dirent structure to be converted
  dirent_str [out]: dirent in string format

Converts dirent in string format to s3-dirent structure
`static int _str_to_dirent(char dirent_str[], s3_dirent *dirent)`
  dirent_str [in]:  dirent in string format
  dirent     [out]: dirent structure to be created

### CLI
Before a directory is exported by an NFS server it needs to be created. i.e. creation of the directory to be exported is to be handled outside of NFS-Ganesha. The CLIs provide a way to manipulate files and directories with the backend as Amazon S3. 

Current CLIs supported are:
1) mkdir_p
Format: `mkdir_p <directory path>
Functionally mkdir_p is similar to mkdir with the '-p' option. It creates directories recursively on Amazon S3.

### Common
common provides generic structures and functions that are shared by multiple modules.

struct dict is defined in common
Data structures
```c
typedef struct {
  char *name;
  char *value;
} dict;
```

## Architecture Diagram

             -------                     -----
            |FSAL-S3|                   | CLI |
             -------\                   /-----
                |     -------      ------   |
                |            \    /         |
                |             \  /          |
                |              \/           | 
                |              /\           |
                |   ----------/  \------    |
                |   |                   |   |
                |   |                   |   |
               \|/ \|/                 \|/ \|/
          -------------               ------------
         |S3-Structures|             |S3-Connector|
          -------------               ------------
                                            |
                                            |
                                           \|/
                                          -----
                                         |libs3|
                                          -----
                                            |  
                                            |  
                                           \|/  
                                          __   _
                                        _(  )_( )_
                                       (AmazonS3 _)
                                         (_) (__)

TODO:  
1) Collision free random object name generator
2) 
