#include "liboop.h"
#include <assert.h>
#include <string.h>
#define Super() List_vtable()
#define Self GrowableList
IMPLEMENT_OPERATOR_NEW()

ENUMERATE_GROWABLE_LIST_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_GROWABLE_LIST_PARENTS)

IMPLEMENT_SELF_METHOD(void, ensure, size_t min_capacity, THROWS) {
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

IMPLEMENT_OVERRIDE_METHOD(Object, List, at, size_t i) {
    GrowableList self = DOWNCAST(this, GrowableList);
    size_t length = self.data->length;
    if (i >= length) {
        return null;
    }
    Object* elements = self.data->elements;
    return elements[i];
}

IMPLEMENT_OVERRIDE_METHOD(void, List, setAt, size_t i, Object obj) {
    GrowableList self = DOWNCAST(this, GrowableList);
    size_t length = self.data->length;
    if (i >= length) {
        return;
    }
    Object* elements = self.data->elements;
    elements[i] = obj;
}
IMPLEMENT_OVERRIDE_METHOD(Object, List, removeAt, size_t i, THROWS) {
    GrowableList self = DOWNCAST(this, GrowableList);
    size_t length = self.data->length;
    if (i >= length) {
        // TODO: Throw range exception
        THROW(Exception$make_new(StringRef$wrap("Index out of range").asString), null)
    }
    Object *elements = self.data->elements;
    Object deleted = elements[i];
    if (i == 0) {
        memmove(elements, elements + 1, (length - 1) * sizeof(Object));
    } else if (i == length - 1) {
        // do nothing
    } else {
        memmove(elements + i, elements + i + 1, (length - 1 - i) * sizeof(Object));
    }
    self.data->length--;
    return deleted;
}
IMPLEMENT_OVERRIDE_METHOD(size_t, List, length) {
    GrowableList self = DOWNCAST(this, GrowableList);
    size_t length = self.data->length;
    return length;
}
IMPLEMENT_OVERRIDE_METHOD(void, List, setLength, size_t newLength, THROWS) {
    GrowableList self = DOWNCAST(this, GrowableList);
    size_t length = self.data->length;
    if (newLength == length) {
        return;
    }
    if (newLength > length) {
        GrowableList_ensure(self, newLength, EXCEPTION);
        if (HAS_EXCEPTION) {
            RETHROW()
        }
        Object *elements = self.data->elements;
        for (size_t i = length; i < newLength; i++) {
            elements[i] = null;
        }
    } else {
        Object *elements = self.data->elements;
        for (size_t i = length; i > newLength; i--) {
            Object_delete(elements[i - 1]);
        }
    }
    self.data->length = newLength;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    GrowableList self = DOWNCAST(this, GrowableList);
    size_t capacity = self.data->capacity;
    Object* elements = self.data->elements;
    Super()->super.delete(this);
    if (capacity != 0 || elements != NULL) {
        free(elements);
    }
}


IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // GrowableList
    vtable->ensure = _GrowableList_ensure_impl;
    // List
    List_vtable_t *list_vtable = (List_vtable_t *)vtable;
    list_vtable->at = _GrowableList_at_impl;
    list_vtable->setAt = _GrowableList_setAt_impl;
    list_vtable->length = _GrowableList_length_impl;
    list_vtable->setLength = _GrowableList_setLength_impl;
    list_vtable->removeAt = _GrowableList_removeAt_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _GrowableList_delete_impl;
}

OBJECT_CAST_IMPL(Iterable, GrowableList)

IMPLEMENT_CONSTRUCTOR(new) {
    List$new(this.asList);
}

IMPLEMENT_SELF_GETTER(size_t, length) {
    return this.data->length;
}

#undef Super
#undef Self