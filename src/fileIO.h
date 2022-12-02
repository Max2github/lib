#include "../fileIO_h.h"

char File_getCurrentByte(FILE * file) {
    char b = File_getc(file);
    // file->_offset--; // do not advance offset
    File_seek(file, -1, FILE_SEEK_CUR);
    return b;
}

void File_setCurrentByte(FILE * file, char b) {
    File_putc(file, b);
    // file->_offset--; // do not advance offset
    File_seek(file, -1, FILE_SEEK_CUR);
}

bool File_copy(const char * tar, const char * src) {
    FILE * srcFile = NULL;
    File_open(&srcFile, src, "r");
    FILE * tarAlready = NULL;
    File_open(&tarAlready, tar, "r");
    if(tarAlready == NULL && srcFile != NULL) {
        FILE* tarFile = NULL;
        File_open(&tarFile, tar, "w");
        int ch;
        while((ch = File_getc(srcFile)) != EOF) { File_putc(tarFile, ch); };
        File_close(tarFile);
    } else  {
        File_close(tarAlready);
    }
    File_close(srcFile);
    return true;
}

bool File_insertBytes(const char * path, long startPos, const char * byteStream, int bytes) {
    FILE* file = NULL;
    File_open(&file, path, "r+");
    if (file == NULL) { return false; }

    File_seek(file, 0, FILE_SEEK_END);
    long endPos = File_tell(file);
    if (startPos > endPos) { startPos = endPos; }
    long bytesWritten = endPos - startPos;

    // set dummy at the end of the file if needed
    for (int i = 0; i < bytes; i++) {
        File_putc(file, -1);
    }

    File_seek(file, 0, FILE_SEEK_END);
    long endPosOld = endPos;
    endPos = File_tell(file);
    printf("%ld %ld\n", endPosOld, endPos);
    long readPos = endPos - bytes - 1;
    long targetPos = endPos - 1;
    if (readPos < startPos) { readPos = startPos; }

    File_seek(file, readPos, FILE_SEEK_SET);
    printf("w: %ld, n: %d, s:%c\n", bytesWritten, bytes, File_getCurrentByte(file));

    // move all backwards
    while(readPos >= startPos) {
        // read
        File_seek(file, readPos, FILE_SEEK_SET);
        char byte = File_getCurrentByte(file);
        readPos--;
        // write
        File_seek(file, targetPos, FILE_SEEK_SET);
        File_setCurrentByte(file, byte);
        // go back (we are reading backwards)
        targetPos--;
    }
    // write our new content
    File_seek(file, startPos, FILE_SEEK_SET);
    File_write(file, byteStream, bytes, 1);
    File_close(file);
    return true;
}

// move all bytes after the bytes to delete forward by one position
bool File_deleteBytes(const  char * path, long startPos, int bytes) {
    char byte = 'n'; // our dummy buffer
    long readPos = startPos + bytes;
    FILE* file = NULL;
    File_open(&file, path, "r+");
    if (file == NULL) { return false; }

    File_seek(file, 0, FILE_SEEK_END);
    const long fileEndPos = File_tell(file);

    // read
    File_seek(file, readPos, FILE_SEEK_SET);
    while((byte = File_getc(file)) != EOF /*fread(&byte, sizeof(byte), 1, file)*/) {
        readPos++;
        // readPos = ftell(file);

        // write
        File_seek(file, startPos, FILE_SEEK_SET);
        // fwrite(&byte, sizeof(byte), 1, file);
        File_putc(file, byte);
        startPos++;
        // startPos = ftell(file);
        // switch to reading
        File_seek(file, readPos, FILE_SEEK_SET);
    }
    File_truncate(Fileno(file), fileEndPos - bytes);
    File_close(file);
    return true;
}

#define File_readeDir_add_codegen1(char1, char2) *it_to++ = char1;
#define File_readeDir_add_codegen2(char1, char2) File_readeDir_add_codegen1(char1, '-') File_readeDir_add_codegen1(char2, '-')
#define File_readeDir_add_codegen(amount, char1, char2) File_readeDir_add_codegen##amount(char1, char2)
#define File_readDir_add(len, path, amount, char1, char2) { \
    char * temp = (char *) FILEIO_H_MALLOC(len + amount + 1); \
    char * it_to = temp; \
    while ((*it_to++ = *path++) != '\0') {}; \
    File_readeDir_add_codegen(amount, char1, char2) \
    *it_to = '\0'; \
    path = temp; \
}
#define File_readDir_remove(len, path, amount) { \
    char * temp = (char *) FILEIO_H_MALLOC(len + 1 - amount); \
    char * it_to = temp; \
    for (int i = 0; i < (len - amount) && *path != '\0'; i++) { *it_to++ = *path++; } \
    *it_to = '\0'; \
    path = temp; \
}
File_list File_readDir(const char * dirpath) {
    // check for path ending - accept e.g name/ name name/*
    if (*dirpath == '\0') { return NULL; }
    const char * last = dirpath;
    int len = 1;
    while (*(++last) != '\0') { len++; }; // go to last element
    last--;
    #if OS_WINDOWS
        if (*last == '*') {}
        else if (*last == PATH_SEP_CHAR) File_readDir_add(len, dirpath, 1, '*', '-')
        else File_readDir_add(len, dirpath, 2, PATH_SEP_CHAR, '*')
    #elif OS_UNIX
        if (*last == '*') File_readDir_remove(len, dirpath, 1)
        else if (*last == PATH_SEP_CHAR) {  }
        else File_readDir_add(len, dirpath, 1, PATH_SEP_CHAR, '-')
    #endif
    puts(last); puts(dirpath); putchar('\n');

    File_list filesInDir = NULL;
    File_readDir_pre(filesInDir, dirpath);
    return filesInDir;
}