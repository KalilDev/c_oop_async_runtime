#include "Object.h"
#include "List.h"
#include "ListIterator.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "string.h"
#include "StringBuffer.h"
#include <strings.h>
#include <assert.h>
#include "String.h"
#include "primitive/StringRef.h"
#include "foreach.h"
#include "GrowableList.h"
#include "Throwable.h"

#define Super() Object_vtable()
#define Self List

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_LIST_PARENTS)
ENUMERATE_LIST_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_METHOD(void, add, Object e, THROWS) {
    size_t length = List_length(this);
    List_setLength(this, length + 1, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW()
    }
    List_setAt(this, length, e);
}

IMPLEMENT_SELF_METHOD(String, join, String sep) {
    StringBuffer buf = StringBuffer$make_new();
    Object sepObj = sep.asObject;
    if (Object_isNull(sepObj)) {
        sep = StringRef$wrap(", ").asString;
    }
    StringBuffer_writeAll(buf, this, sep);

    String res = StringBuffer_releaseToString(buf);
    Object_delete(buf.asObject);
    return res;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    List self = DOWNCAST(this, List);
    if (List_length(self) == 0) {
        Super()->delete(this);
        return;
    }
    // If there are objects, delete them
    foreach (Object, e, List_as_Iterable(self), {
        Object_delete(e);
    })

    // Delete the array
    Super()->delete(this);
}


IMPLEMENT_SELF_METHOD(Object, remove, Object obj, THROWS) {
    size_t i = 0;
    Object result = null;
    foreach(Object, e, List_as_Iterable(this), {
        if (Object_equals(e, obj)) {
            result = List_removeAt(this, i, EXCEPTION);
            break;
        }
        i++;
    })
    return result;
}

IMPLEMENT_SELF_METHOD(void, clear, THROWS) {
    List_setLength(this, 0, EXCEPTION);
}

IMPLEMENT_OVERRIDE_METHOD(Iterator, Iterable, iterator) {
    List self = Iterable_as_List(this);
    return ListIterator$make_new(self).asIterator;
}

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    List self = DOWNCAST(this, List);

    StringBuffer buf = StringBuffer$make_new();
    StringBuffer_writeCharCode(buf, '[');

    StringBuffer_writeAll(buf, self, StringRef$wrap(", ").asString);

    StringBuffer_writeCharCode(buf, ']');
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
        1,
        "Iterable", offsetof(List_vtable_t , Iterable_vtable));
    // List
    vtable->join = _List_join_impl;
    vtable->add = _List_add_impl;
    vtable->clear = _List_clear_impl;
    vtable->remove = _List_remove_impl;
    // Iterable
    Iterable_vtable_t *iterable_vtable = &vtable->Iterable_vtable;
    initImplementedInterfaceVtable(
        (Interface_vtable_t*)iterable_vtable,
        (Interface_vtable_t*)Iterable_vtable(),
        sizeof(*Iterable_vtable()),
        offsetof(List_vtable_t, Iterable_vtable)
    );
    iterable_vtable->iterator = _List_iterator_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _List_delete_impl;
    object_vtable->toString = _List_toString_impl;
}

OBJECT_CAST_IMPL(Iterable, List)
INTERFACE_CAST_IMPL(List, Iterable, Object)


IMPLEMENT_ABSTRACT_CONSTRUCTOR(new) {

}

IMPLEMENT_STATIC_METHOD(List, new) {
    return GrowableList$make_new().asList;
}

#undef Super
#undef Self