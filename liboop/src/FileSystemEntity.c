#include "Iterator.h"
#include "oop.h"
#include "FileSystemEntity.h"
#include <assert.h>
#include <string.h>
#include <libgen.h>

#define Self FileSystemEntity
#define Super() Object_vtable()

ENUMERATE_FILE_SYSTEM_ENTITY_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_FILE_SYSTEM_ENTITY_PARENTS)

IMPLEMENT_ABSTRACT_CONSTRUCTOR(new, String path) {
    this.data->path = path;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    FileSystemEntity self = DOWNCAST(this, FileSystemEntity);
    Object_delete(self.data->path.asObject);
    Super()->delete(this);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
    (Object_vtable_t *) vtable,
    (Object_vtable_t *) Super(),
    sizeof(*Super()),
    STR(Self),
    0);
    // FileSystemEntity
    // todo

    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _FileSystemEntity_delete_impl;
}

IMPLEMENT_SELF_GETTER(String, path) {
    return this.data->path;
}
IMPLEMENT_SELF_GETTER(String, name) {
    char *pathcstring = strdup(String_cStringView(FileSystemEntity_path(this)));
    // messy api holy shit
    char *name = basename(pathcstring);
    char *res = strdup(name);
    free(pathcstring);
    return String$make_own(res);
}

#undef Self