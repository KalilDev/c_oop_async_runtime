#include "Object.h"
#include "List.h"
#include "ListIterator.h"
#include "oop.h"
#include "string.h"
#include "stdbool.h"

#define Super() Object_vtable()

bool _ListIterator_moveNext(Iterator this) {
    ListIterator self = DOWNCAST(this, ListIterator);
    long long *i = &self.data->i;
    if (*i >= (long long)List_length(self.data->list) - 1) {
        return false;
    }
    *i += 1;
    return true;
}

Object _ListIterator_current(Iterator this) {
    ListIterator self = DOWNCAST(this, ListIterator);
    return List_at(self.data->list, self.data->i);
}

static ListIterator_vtable_t _ListIterator_vtable = {0};

const ListIterator_vtable_t* ListIterator_vtable() {
    if (_ListIterator_vtable.super.current == NULL) {
        memmove(&_ListIterator_vtable.super, Super(), sizeof(*Super()));
        // init the vtable
        _ListIterator_vtable.super.current = _ListIterator_current;
        _ListIterator_vtable.super.moveNext = _ListIterator_moveNext;
    }
    return &_ListIterator_vtable;
}

SUPER_CAST_IMPL(ListIterator, Iterator)
UPCAST_IMPL(ListIterator, Object)
ListIterator ListIterator_new(List list) {
    ListIterator obj = {
            .vtable = ListIterator_vtable(),
            .data = Object_allocate(sizeof(ListIterator_data)),
    };
    if (obj.data == NULL) {
        return obj;
    }
    obj.data->i = -1ll;
    obj.data->list = list;
    return obj;
}

#undef Super