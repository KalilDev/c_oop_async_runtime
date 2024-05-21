#include <assert.h>
#include "liboop.h"

#include <string.h>

#define Self Link
#define Super() FileSystemEntity_vtable()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_LINK_PARENTS)
ENUMERATE_LINK_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

//IMPLEMENT_SELF_METHOD(List, list)     {
//}
IMPLEMENT_OVERRIDE_METHOD(FileSystemEntity, FileSystemEntity, absolute) {
    Link self = DOWNCAST(this, Link);
    String path = self.data->super.path;
    const char* pathCstring = String_cStringView(path);
    char *absolute_path = realpath(pathCstring, NULL);
    String absolutePath = String$make_own(absolute_path);
    return Link$make_new(absolutePath).asFileSystemEntity;
}

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    Link self = DOWNCAST(this, Link);
    String path = self.data->super.path;
    return String_format_c("Link({})", path);
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
    // Link
    //vtable->list = _Link_list_impl;
    // FileSystemEntity
    FileSystemEntity_vtable_t *fs_entity_vtable = (FileSystemEntity_vtable_t *)vtable;
    fs_entity_vtable->absolute = _Link_absolute_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->toString = _Link_toString_impl;
}

#undef Self