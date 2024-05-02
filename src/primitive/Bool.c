#include <stdlib.h>

#include "../oop.h"
#include "Object.h"
#include "primitive/Bool.h"

METHOD_IMPL(Bool, unbox, bool, (Bool this), (this)) {
    return this.data;
}

void _Bool_delete(Object this) {
    return;
}

bool _Bool_equals(Object this, Object other) {
    Bool a = DOWNCAST(this, Bool);
    Bool b = DOWNCAST(this, Bool);
    return Bool_unbox(a) == Bool_unbox(b);
}

long _Bool_getHashCode(Object this) {
    Bool self = DOWNCAST(this, Bool);
    return self.data;
}


static const Bool_vtable_t _Bool_vtable = {
        // Object
        .super = {
                .delete = _Bool_delete,
                .equals = _Bool_equals,
                .getHashCode = _Bool_getHashCode,
        },
        .unbox = _Bool_unbox
};

const Bool_vtable_t* Bool_vtable() {
    return &_Bool_vtable;
}

const Bool True =  {
        .vtable = &_Bool_vtable,
        .data = true
};
const Bool False;

PRIMITIVE_SUPER_CAST_IMPL(Bool, Object)
Bool Bool_box(bool unboxed) {
    Bool box = {
            .vtable = Bool_vtable(),
            .data = unboxed
    };
    return box;
}