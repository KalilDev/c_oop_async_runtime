#include "../oop.h"
#include <stdlib.h>
#include <tgmath.h>

#include "Object.h"
#include "String.h"
#include "primitive/Integer.h"

METHOD_IMPL(Integer, unbox_c, char, (Integer this), (this)) {
    return (char)this.data;
}

METHOD_IMPL(Integer, unbox_i, int, (Integer this), (this)) {
    return (int)this.data;
}

METHOD_IMPL(Integer, unbox_l, long, (Integer this), (this)) {
    return (long)this.data;
}

void _Integer_delete(Object this) {
    return;
}

bool _Integer_equals(Object this, Object other) {
    Integer a = DOWNCAST(this, Integer);
    Integer b = DOWNCAST(this, Integer);
    return Integer_unbox_l(a) == Integer_unbox_l(b);
}

long _Integer_getHashCode(Object this) {
    Integer self = DOWNCAST(this, Integer);
    return self.data;
}

OVERRIDE_IMPL(String, Integer, toString, Object) {
    Integer self = DOWNCAST(this, Integer);
    long num = self.data;
    double log = log10((double)num);
}

static const Integer_vtable_t _Integer_vtable = {
        // Number
        .super = {
                // Object
                .super = {
                        .delete = _Integer_delete,
                        .equals = _Integer_equals,
                        .getHashCode = _Integer_getHashCode,
                        .toString = _Integer_toString,
                }
        },
        .unbox_i = _Integer_unbox_i,
        .unbox_l = _Integer_unbox_l,
};

const Integer_vtable_t* Integer_vtable() {
    return &_Integer_vtable;
}
PRIMITIVE_SUPER_CAST_IMPL(Integer, Number)
PRIMITIVE_UPCAST_IMPL(Integer, Object)
Integer Integer_box_c(char unboxed) {
    Integer box = {
            .vtable = Integer_vtable(),
            .data = (long)unboxed
    };
    return box;
}
Integer Integer_box_i(int unboxed) {
    Integer box = {
            .vtable = Integer_vtable(),
            .data = (long)unboxed
    };
    return box;
}
Integer Integer_box_l(long unboxed) {
    Integer box = {
            .vtable = Integer_vtable(),
            .data = unboxed
    };
    return box;
}