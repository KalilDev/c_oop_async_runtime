#include "Iterator.h"
#include "oop.h"
#include "FileSystemEntity.h"
#include "Directory.h"
#include "Directory.h"
#include "List.h"
#include <assert.h>
#include "primitive/StringRef.h"
#include "String.h"
#include "File.h"
#include "Throwable.h"
#include "Link.h"
#include "IOException.h"

#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <dirent.h>

#define Self Directory
#define Super() FileSystemEntity_vtable()
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_DIRECTORY_PARENTS)
ENUMERATE_DIRECTORY_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_METHOD(List, list, THROWS)     {
    List children = List_new();
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);


    // Open the directory
    DIR *directory = opendir(pathCstring);

    // Read directory entries
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        String childPath = String_format_c("{}/{}", path, StringRef$wrap(entry->d_name));
        switch (entry->d_type) {
            case DT_REG: {
                File f = File$make_new(childPath);
                List_add(children, f.asObject, EXCEPTION);
                if (HAS_EXCEPTION) {
                    closedir(directory);
                    Object_delete(children.asObject);
                    RETHROW(DOWNCAST(null, List));
                }
                break;
            }
            case DT_DIR: {
                Directory d = Directory$make_new(childPath);
                List_add(children, d.asObject, EXCEPTION);
                if (HAS_EXCEPTION) {
                    closedir(directory);
                    Object_delete(children.asObject);
                    RETHROW(DOWNCAST(null, List));
                }
                break;
            }
            case DT_LNK:{
                Link l = Link$make_new(childPath);
                List_add(children, l.asObject, EXCEPTION);
                if (HAS_EXCEPTION) {
                    closedir(directory);
                    Object_delete(children.asObject);
                    RETHROW(DOWNCAST(null, List));
                }
                break;
            }
            // Handle other types as needed
            default:{
                printf("Unknown type\n");
            }
        }
    }

    // Close the directory
    closedir(directory);
    return children;
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
//    int fd = open(pathCstring, O_CREAT | O_TRUNC, 0644);
//    close(fd);
}
IMPLEMENT_SELF_METHOD(void, deleteSync)           {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
//    unlink(pathCstring);
}
IMPLEMENT_SELF_METHOD(bool, existsSync)           {
    String path = this.data->super.path;
    const char* pathCstring = String_cStringView(path);
    return access(pathCstring, F_OK) != -1;
}

IMPLEMENT_OVERRIDE_METHOD(FileSystemEntity, FileSystemEntity, absolute, THROWS) {
    Directory self = DOWNCAST(this, Directory);
    String path = self.data->super.path;
    const char* pathCstring = String_cStringView(path);
    char *absolute_path = realpath(pathCstring, NULL);
    if (absolute_path == NULL) {
        THROW(IOException$make_fromErrno(), DOWNCAST(null, FileSystemEntity))
    }
    String absolutePath = String$make_own(absolute_path);
    return Directory$make_new(absolutePath).asFileSystemEntity;
}

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    Directory self = DOWNCAST(this, Directory);
    String path = self.data->super.path;
    return String_format_c("Directory({})", path);
}

IMPLEMENT_STATIC_METHOD(bool, isDirectorySync, String path) {
    struct stat path_stat;
    stat(String_cStringView(path), &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

IMPLEMENT_CONSTRUCTOR(new, String path) {
    FileSystemEntity$new(this.asFileSystemEntity, path);
}

IMPLEMENT_STATIC_METHOD(Directory, current) {
    return Directory$make_new(StringRef$wrap(".").asString);
}

IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_VTABLE() {
    initVtable(
    (Object_vtable_t *) vtable,
    (Object_vtable_t *) Super(),
    sizeof(*Super()),
    STR(Self),
    0);
    // Directory
    vtable->list = _Directory_list_impl;
    vtable->parent = _Directory_parent_impl;
    vtable->copy = _Directory_copy_impl;
    vtable->createSync = _Directory_createSync_impl;
    vtable->deleteSync = _Directory_deleteSync_impl;
    vtable->existsSync = _Directory_existsSync_impl;
    // FileSystemEntity
    FileSystemEntity_vtable_t *fs_entity_vtable = (FileSystemEntity_vtable_t *)vtable;
    fs_entity_vtable->absolute = _Directory_absolute_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->toString = _Directory_toString_impl;
}

#undef Self