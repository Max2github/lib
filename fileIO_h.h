#ifndef FILEIO_H_H
#define FILEIO_H_H

#include "templates/def.h"
#include "templates/simple_list.h"

#ifndef NO_STD_LIB
    #include <stdio.h>
    #define FILEIO_H_MALLOC SIMPLE_LIST_H_MALLOC
    #define FILEIO_H_FREE SIMPLE_LIST_H_FREE

    #if DEF_PF_WINDOWS
        //#include <minwinbase.h>
        #include <windows.h>
        #include <direct.h>
        #include <io.h>
        #define GetCurrentDir _getcwd
        #define PATH_SEP_CHAR '\\'
        #define PATH_SEP_STRING "\\"
        #define File_open(fileStruct, fileName, mode) fopen_s(fileStruct, fileName, mode)
        #define File_truncate(fd, size) _chsize_s(fd, size);
        #define Fileno(file) _fileno(file)
        #define PATH_MAXLEN MAX_PATH
        #define File_readDir_pre(newlist, path) { \
            WIN32_FIND_DATA FindFileData; \
            HANDLE hFind = FindFirstFile(path, &FindFileData); \
            if (hFind != INVALID_HANDLE_VALUE) { \
                do { \
                    if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {\
                        struct File newF; \
                        newF.fname = *((struct File_name *) &(FindFileData.cFileName)); \
                        newF.type = File_type_Directory; \
                        SIMPLE_LIST_ADDFIRST(newlist, newF);\
                    } else { \
                        struct File newF; \
                        newF.fname = *((struct File_name *) &(FindFileData.cFileName)); \
                        newF.type = File_type_File; \
                        SIMPLE_LIST_ADDFIRST(newlist, newF);\
                    } \
                } while (FindNextFile(hFind, &FindFileData) != 0); \
            } \
            FindClose(hFind); \
        }
    #elif DEF_PF_UNIX
        #include <unistd.h>
        #include <dirent.h>
        #define GetCurrentDir getcwd
        #define PATH_SEP_CHAR '/'
        #define PATH_SEP_STRING "/"
        #define File_open(fileStruct, fileName, mode) *(fileStruct) = fopen(fileName, mode)
        #define File_truncate(fd, size) ftruncate(fd, size);
        #define Fileno(file) fileno(file)
        #if defined(__DARWIN_MAXPATHLEN)
            #define PATH_MAXLEN __DARWIN_MAXPATHLEN
        //#elif

        #else
            #define PATH_MAXLEN 256
        #endif
        #define File_getType_pre(entryType, out) { \
            switch (entryType) { \
                case DT_DIR: { out = File_type_Directory; break; } \
                case DT_REG: { out = File_type_File; break; } \
                case DT_LNK: { out = File_type_Symlink; break; } \
                 \
                case DT_BLK: { out = File_type_block_device; break; } \
                case DT_CHR: { out = File_type_character_device; break; } \
                 \
                case DT_SOCK: { out = File_type_Socket; break; } \
                case DT_FIFO: { out = File_type_named_pipe; break; } \
                 \
                case DT_UNKNOWN: \
                default: { out = File_type_Unknown; break; } \
            } \
        }
        #define File_readDir_pre(newlist, path) { \
            struct dirent * entry = NULL; \
            DIR * dp = NULL; \
            dp = opendir(path); \
            if (dp != NULL) { \
                while ((entry = readdir(dp)) != NULL) { \
                    struct File newF; \
                    newF.fname = *((struct File_name *) &(entry->d_name)); \
                    File_getType_pre(entry->d_type, newF.type) \
                    SIMPLE_LIST_ADDFIRST(newlist, newF);\
                } \
            } \
            closedir(dp); \
        }

    #endif

    #define File_getc(file) fgetc(file)
    #define File_putc(file, ch) fputc(ch, file)
    #define File_tell(file) ftell(file)
    #define File_seek(file, offset, option) fseek(file, offset, option)
    #define File_write(file, data, size, amount_items) fwrite(data, size, amount_items, file)
    #define File_close(file) fclose(file)
    #define FILE_SEEK_SET SEEK_SET
    #define FILE_SEEK_CUR SEEK_CUR
    #define FILE_SEEK_END SEEK_END

#else

#endif

enum File_type {
    File_type_Unknown = 0,
    File_type_Directory,
    File_type_File,
    File_type_Symlink,

    File_type_block_device,
    File_type_character_device,

    File_type_Socket,
    File_type_named_pipe
};

struct File_name {
    char str[PATH_MAXLEN];
};
struct File {
    union {
        struct File_name fname;
        char name[PATH_MAXLEN];
    };
    enum File_type type;
};

typedef SIMPLE_LIST(struct File) File_list;

char File_getCurrentByte(FILE * file);

void File_setCurrentByte(FILE * file, char b);

bool File_copy(const char * tar, const char * src);

bool File_insertBytes(const char * path, long startPos, const char * byteStream, int bytes);

// move all bytes after the bytes to delete forward by one position
bool File_deleteBytes(const  char * path, long startPos, int bytes);

File_list File_readDir(const char *);

#endif