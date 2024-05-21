#include <assert.h>
#include "liboop.h"

#define Super() Object_vtable()
#define Self Map

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_LIST_PARENTS)
ENUMERATE_MAP_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    Map self = DOWNCAST(this, Map);

    StringBuffer buf = StringBuffer$make_new();
    StringBuffer_writeCharCode(buf, '{');

    //StringBuffer_writeAll(buf, self, StringRef$wrap(", ").asString);

    StringBuffer_writeCharCode(buf, '}');
    String res = StringBuffer_releaseToString(buf);
    Object_delete(buf.asObject);
    return res;
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // Map
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->toString = _Map_toString_impl;
}

IMPLEMENT_ABSTRACT_CONSTRUCTOR(new) {

}

IMPLEMENT_STATIC_METHOD(Map, new) {
    return HashMap$make_new().asMap;
}

#undef Super
#undef Self