#include "liboop.h"
#include <assert.h>

#define Self ListIterator
#define Super() Object_vtable()
IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_LIST_ITERATOR_PARENTS)
IMPLEMENT_OVERRIDE_METHOD(bool, Iterator, moveNext) {
    ListIterator self = DOWNCAST(this, ListIterator);
    long long *i = &self.data->i;
    if (*i >= (long long)List_length(self.data->list) - 1) {
        return false;
    }
    *i += 1;
    return true;
}
IMPLEMENT_OVERRIDE_METHOD(Object, Iterator, current) {
    ListIterator self = DOWNCAST(this, ListIterator);
    return List_at(self.data->list, self.data->i);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
    (Object_vtable_t*)vtable,
    (Object_vtable_t*)Super(),
    sizeof(*Super()),
    STR(Self),
    0
    );
    // init the vtable
    vtable->super.current = _ListIterator_current_impl;
    vtable->super.moveNext = _ListIterator_moveNext_impl;
}


IMPLEMENT_CONSTRUCTOR(new, List list) {
    this.data->i = -1ll;
    this.data->list = list;
}

#undef Super