#include "Object.h"
#include "List.h"
#include "ListIterator.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "string.h"
#include <strings.h>
#define Super() Object_vtable()

const List_vtable_t* List_vtable();

METHOD_IMPL(List, add, void, (List this, Object e), (this, e)) {
    size_t *length = &this.data->length;
    List_ensure(this, *length + 1);
    Object* elements = this.data->elements;
    // TODO: Document this behavior! (pass by reference)
    memcpy(elements + *length, &e, sizeof(Object));
    *length += 1;
}
METHOD_IMPL(List, ensure, void, (List this, size_t min_capacity), (this, min_capacity)) {
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

METHOD_IMPL(List, at, Object, (List this, size_t i), (this, i)) {
    size_t length = this.data->length;
    if (i >= length) {
        return Object_null;
    }
    Object* elements = this.data->elements;
    return elements[i];
}

OVERRIDE_IMPL(void , List, delete, Object) {
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

Iterator _List_iterator(Iterable this) {
    List self = Iterable_as_List(this);
    return ListIterator_as_Iterator(ListIterator_new(self));
}

static List_vtable_t _List_vtable = {0};

const List_vtable_t* List_vtable() {
    if (_List_vtable.add == NULL) {
        // List
        _List_vtable.add = _List_add;
        _List_vtable.ensure = _List_ensure;
        _List_vtable.at = _List_at;
        // Iterable
        _List_vtable.Iterable_vtable.super.vtable_offset = offsetof(List_vtable_t, Iterable_vtable);
        _List_vtable.Iterable_vtable.iterator = _List_iterator;
        // Object
        memmove(&_List_vtable.super, Super(), sizeof(*Super()));
        _List_vtable.super.delete = _List_delete;
    }
    return &_List_vtable;
}

OBJECT_CAST_IMPL(Iterable, List)
INTERFACE_CAST_IMPL(List, Iterable, Object)

SUPER_CAST_IMPL(List, Object)

List List_new() {
    List ptr = {
            .vtable = List_vtable(),
            .data = Object_allocate(sizeof(List_data))
    };
    return ptr;
}

size_t List_length(List this) {
    return this.data->length;
}

#undef Super