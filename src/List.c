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

#define Super() Object_vtable()
#define Self List
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_LIST_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_METHOD(String, join, String sep) {
    StringBuffer buf = StringBuffer$make_new();
    Object sepObj = String_as_Object(sep);
    if (Object_isNull(sepObj)) {
        sep = StringRef_as_String(StringRef$wrap(", "));
    }
    StringBuffer_writeAll(buf, this, sep);

    String res = StringBuffer_releaseToString(buf);
    Object_delete(StringBuffer_as_Object(buf));
    return res;
}
IMPLEMENT_SELF_METHOD(void, add, Object e) {
    size_t *length = &this.data->length;
    List_ensure(this, *length + 1);
    Object* elements = this.data->elements;
    // TODO: Document this behavior! (pass by reference)
    memcpy(elements + *length, &e, sizeof(Object));
    *length += 1;
}

IMPLEMENT_SELF_METHOD(void, ensure, size_t min_capacity) {
    size_t capacity = this.data->capacity;
    if (capacity >= min_capacity) {
        return;
    }
    // Assume the initial capacity to be 4
    size_t new_capacity = capacity == 0 ? 4 : capacity;
    while (new_capacity < min_capacity) {
        new_capacity <<= 1;
    }
    Object* old_elements = this.data->elements;
    Object* new_elements = realloc(old_elements, new_capacity * sizeof(Object));
    if (new_elements == NULL) {
        // TODO: throw
    }
    this.data->elements = new_elements;
    this.data->capacity = new_capacity;
}

IMPLEMENT_SELF_METHOD(Object, at, size_t i) {
    size_t length = this.data->length;
    if (i >= length) {
        return null;
    }
    Object* elements = this.data->elements;
    return elements[i];
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    List self = DOWNCAST(this, List);
    size_t capacity = self.data->capacity;
    size_t length = self.data->length;
    Object* elements = self.data->elements;
    if (capacity == 0) {
        Super()->delete(this);
        return;
    }
    // If there are objects, delete them
    if (length != 0) {
        for (size_t i = 0; i < length; i++) {
            Object element = elements[i];
            Object_delete(element);
        }
    }

    // Delete the array
    free(elements);
    Super()->delete(this);
}

IMPLEMENT_OVERRIDE_METHOD(Iterator, Iterable, iterator) {
    List self = Iterable_as_List(this);
    return ListIterator_as_Iterator(ListIterator$make_new(self));
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
    vtable->add = _List_add_impl;
    vtable->ensure = _List_ensure_impl;
    vtable->at = _List_at_impl;
    vtable->join = _List_join_impl;
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
}

OBJECT_CAST_IMPL(Iterable, List)
INTERFACE_CAST_IMPL(List, Iterable, Object)

SUPER_CAST_IMPL(List, Object)

IMPLEMENT_CONSTRUCTOR(new) {

}

IMPLEMENT_SELF_GETTER(size_t, length) {
    return this.data->length;
}

#undef Super
#undef Self