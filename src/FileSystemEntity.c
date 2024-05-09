#include "Iterator.h"
#include "oop.h"
#include "FileSystemEntity.h"
#include <assert.h>

#define Self FileSystemEntity
#define Super() Object_vtable()
SUPER_CAST_IMPL(FileSystemEntity, Object)

ENUMERATE_FILE_SYSTEM_ENTITY_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_ABSTRACT_CONSTRUCTOR(new, String path) {
    this.data->path = path;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    FileSystemEntity self = DOWNCAST(this, FileSystemEntity);
    if (!Object_isNull(String_as_Object(self.data->path))) {
        Object_delete(String_as_Object(self.data->path));
    }
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

#undef Self