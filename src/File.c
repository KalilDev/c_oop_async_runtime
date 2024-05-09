#include "Iterator.h"
#include "oop.h"
#include "FileSystemEntity.h"
#include "File.h"
#include "Directory.h"
#include "RandomAccessFile.h"
#include <assert.h>

#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>

#define Self File
#define Super() FileSystemEntity_vtable()
SUPER_CAST_IMPL(File, FileSystemEntity)
UPCAST_IMPL(File, Object)

ENUMERATE_FILE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_METHOD(String, readStringSync)     {
    RandomAccessFile raf = File_openSync(this, FileMode$read);
    size_t len = RandomAccessFile_length(raf);
    String s = RandomAccessFile_readString(raf, len);
    Object_delete(RandomAccessFile_as_Object(raf));
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
IMPLEMENT_SELF_METHOD(void, copy, String newPath) {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    int source_fd = open(pathCstring, O_RDONLY);
    struct stat stat;
    fstat(source_fd, &stat);
    size_t size = stat.st_size;
    int target_fd = open(pathCstring, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    long offset = 0;
    size_t outSize = sendfile(target_fd, source_fd, &offset, size);
    (void)outSize;
    close(target_fd);
    close(source_fd);

}
IMPLEMENT_SELF_METHOD(void, createSync)           {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    int fd = open(pathCstring, O_CREAT | O_TRUNC, 0644);
    close(fd);
}
IMPLEMENT_SELF_METHOD(void, deleteSync)           {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    unlink(pathCstring);
}
IMPLEMENT_SELF_METHOD(bool, existsSync)           {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    return access(pathCstring, F_OK) != -1;
}
IMPLEMENT_SELF_METHOD(size_t, lengthSync)         {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    struct stat stat_;
    stat(pathCstring, &stat_);
    size_t size = stat_.st_size;
    return size;
}
IMPLEMENT_SELF_METHOD(RandomAccessFile, openSync, FileMode mode){
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
    return RandomAccessFile$make_open(path, modes);
}

IMPLEMENT_OVERRIDE_METHOD(FileSystemEntity, FileSystemEntity, absolute) {
    File self = DOWNCAST(this, File);
    String path = self.data->super.path;
    const char* pathCstring = String_cStringView(path);
    char *absolute_path = realpath(pathCstring, NULL);
    String absolutePath = String$make_own(absolute_path);
    return File_as_FileSystemEntity(File$make_new(absolutePath));
}

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    File self = DOWNCAST(this, File);
    String path = self.data->super.path;
    return String_format_c("File({})", path);
}

IMPLEMENT_CONSTRUCTOR(new, String path) {
    FileSystemEntity$new(File_as_FileSystemEntity(this), path);
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
    vtable->openSync = _File_openSync_impl;
    // FileSystemEntity
    FileSystemEntity_vtable_t *fs_entity_vtable = (FileSystemEntity_vtable_t *)vtable;
    fs_entity_vtable->absolute = _File_absolute_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->toString = _File_toString_impl;
}

#undef Self