#include "Object.h"
#include "StringIterator.h"
#include "oop.h"
#include "primitive/Integer.h"
#include "stdbool.h"
#include <assert.h>
#define Self StringIterator
#define Super() Object_vtable()
IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_STRING_ITERATOR_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(bool, Iterator, moveNext) {
    StringIterator self = DOWNCAST(this, StringIterator);
    String string = self.data->string;
    if (self.data->started) {
        size_t i = ++self.data->i;
        return i < String_length(string);
    }
    self.data->started = true;
    return String_length(string) != 0;
}

IMPLEMENT_OVERRIDE_METHOD(Object, Iterator, current) {
    StringIterator self = DOWNCAST(this, StringIterator);
    String string = self.data->string;
    Integer charAt = String_at(string, self.data->i);
    return charAt.asObject;
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
    vtable->super.current = _StringIterator_current_impl;
    vtable->super.moveNext = _StringIterator_moveNext_impl;
}

IMPLEMENT_CONSTRUCTOR(new, String string) {
    this.data->i = 0;
    this.data->started = false;
    this.data->string = string;
}

#undef Super