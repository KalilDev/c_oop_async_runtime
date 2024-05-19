#include "Iterator.h"
#include "oop.h"
#include "FileSystemEntity.h"
#include "File.h"
#include "Future.h"
#include "Stream.h"
#include "Directory.h"
#include "RandomAccessFile.h"
#include "primitive/StringRef.h"
#include "IOException.h"
#include "Future.h"
#include "FileReadStream.h"
#include <assert.h>

#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>

#define Self File
#define Super() FileSystemEntity_vtable()

ENUMERATE_FILE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_FILE_PARENTS)

IMPLEMENT_SELF_METHOD(String, readStringSync, THROWS)     {
    RandomAccessFile raf = File_openSync(this, FileMode$read, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(DOWNCAST(null, String))
    }
    size_t len = RandomAccessFile_length(raf, EXCEPTION);
    if (HAS_EXCEPTION) {
        Object_delete(raf.asObject);
        RETHROW(DOWNCAST(null, String))
    }
    String s = RandomAccessFile_readString(raf, len, EXCEPTION);
    if (HAS_EXCEPTION) {
        Object_delete(raf.asObject);
        RETHROW(DOWNCAST(null, String))
    }
    Object_delete(raf.asObject);
    return s;
}
IMPLEMENT_SELF_METHOD(Directory, parent)          {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    char *dirpath = strdup(pathCstring);
    dirpath = dirname(dirpath);
    String dirPath = String$make_own(dirpath);
    return Directory$make_new(dirPath);
}
IMPLEMENT_SELF_METHOD(void, copy, String newPath, THROWS) {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    int source_fd = open(pathCstring, O_RDONLY);
    if (source_fd < 0) {
        THROW(IOException$make_fromErrno())
    }
    struct stat stat;
    int stat_res = fstat(source_fd, &stat);
    if (stat_res < 0) {
        close(source_fd);
        THROW(IOException$make_fromErrno())
    }
    size_t size = stat.st_size;
    int target_fd = open(pathCstring, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (target_fd < 0) {
        close(source_fd);
        THROW(IOException$make_fromErrno())
    }
    long offset = 0;
    ssize_t outSize = sendfile(target_fd, source_fd, &offset, size);
    if (outSize < 0) {
        close(target_fd);
        close(source_fd);
        THROW(IOException$make_fromErrno())
    }
    close(target_fd);
    close(source_fd);

}
IMPLEMENT_SELF_METHOD(void, createSync, THROWS)           {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    int fd = open(pathCstring, O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        THROW(IOException$make_fromErrno())
    }
    close(fd);
}
IMPLEMENT_SELF_METHOD(void, deleteSync, THROWS)           {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    int res = unlink(pathCstring);
    if (res < 0) {
        THROW(IOException$make_fromErrno())
    }
}
IMPLEMENT_SELF_METHOD(bool, existsSync, THROWS)           {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    return access(pathCstring, F_OK) != -1;
}
IMPLEMENT_SELF_METHOD(size_t, lengthSync, THROWS)         {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    struct stat stat_;
    int stat_result = stat(pathCstring, &stat_);
    if (stat_result < 0) {
        THROW(IOException$make_fromErrno(), 0)
    }
    size_t size = stat_.st_size;
    return size;
}

#define ENUMERATE_FILE_OPEN_ASYNC_CAPTURES(CAPTURE) \
    CAPTURE(File, file)                                                \
    CAPTURE(FileMode, mode)

IMPLEMENT_LAMBDA(FileOpenAsync, ENUMERATE_FILE_OPEN_ASYNC_CAPTURES, NO_OWNED_CAPTURES, File file, FileMode mode) {
    Lambda_FileOpenAsync self = DOWNCAST(this, Lambda_FileOpenAsync);
    File file = self.data->file;
    FileMode mode = self.data->mode;
    THROWS = va_arg(args, Throwable*);
    RandomAccessFile raf = File_openSync(file, mode, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    return raf.asObject;
}

IMPLEMENT_SELF_METHOD(Future, open, FileMode mode){
    Lambda_FileOpenAsync lambda = Lambda_FileOpenAsync$make_new(this, mode);
    return Future_computation(lambda.asFunction);
}

IMPLEMENT_SELF_METHOD(Stream, openRead){
    return FileReadStream_as_Stream(FileReadStream$make_new(this));
}

IMPLEMENT_SELF_METHOD(RandomAccessFile, openSync, FileMode mode, THROWS){
    String path = this.data->super.path;
    const char* modes;
    switch (mode) {
        case FileMode$append:
            modes = "a+";
            break;
        case FileMode$read:
            modes = "r";
            break;
        case FileMode$write:
            modes = "w+";
            break;
        case FileMode$writeOnly:
            modes = "w";
            break;
        case FileMode$writeOnlyAppend:
            modes = "a";
            break;
    }
    RandomAccessFile raf = RandomAccessFile$make_open(path, modes, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(raf)
    }
    return raf;
}

IMPLEMENT_OVERRIDE_METHOD(FileSystemEntity, FileSystemEntity, absolute, THROWS) {
    File self = DOWNCAST(this, File);
    String path = self.data->super.path;
    const char* pathCstring = String_cStringView(path);
    char *absolute_path = realpath(pathCstring, NULL);
    if (absolute_path == NULL) {
        THROW(IOException$make_fromErrno(), DOWNCAST(null, FileSystemEntity))
    }
    String absolutePath = String$make_own(absolute_path);
    return File$make_new(absolutePath).asFileSystemEntity;
}

IMPLEMENT_STATIC_METHOD(bool, isFileSync, String path) {
    struct stat path_stat;
    stat(String_cStringView(path), &path_stat);
    return S_ISREG(path_stat.st_mode);
}

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    File self = DOWNCAST(this, File);
    String path = self.data->super.path;
    return String_format_c("File({})", path);
}

IMPLEMENT_CONSTRUCTOR(new, String path) {
    FileSystemEntity$new(this.asFileSystemEntity, path);
}
IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_VTABLE() {
    initVtable(
    (Object_vtable_t *) vtable,
    (Object_vtable_t *) Super(),
    sizeof(*Super()),
    STR(Self),
    0);
    // File
    vtable->readStringSync = _File_readStringSync_impl;
    vtable->parent = _File_parent_impl;
    vtable->copy = _File_copy_impl;
    vtable->createSync = _File_createSync_impl;
    vtable->deleteSync = _File_deleteSync_impl;
    vtable->existsSync = _File_existsSync_impl;
    vtable->lengthSync = _File_lengthSync_impl;
    vtable->open = _File_open_impl;
    vtable->openSync = _File_openSync_impl;
    vtable->openRead = _File_openRead_impl;
    // FileSystemEntity
    FileSystemEntity_vtable_t *fs_entity_vtable = (FileSystemEntity_vtable_t *)vtable;
    fs_entity_vtable->absolute = _File_absolute_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->toString = _File_toString_impl;
}

#undef Self