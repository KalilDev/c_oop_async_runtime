#include <stdlib.h>

#include "../oop.h"
#include "Object.h"
#include "primitive/Double.h"

METHOD_IMPL(Double, unbox_f, float, (Double this), (this)) {
    return (float)this.data;
}

METHOD_IMPL(Double, unbox_d, double, (Double this), (this)) {
    return this.data;
}

void _Double_delete(Object this) {
    return;
}

bool _Double_equals(Object this, Object other) {
    Double a = DOWNCAST(this, Double);
    Double b = DOWNCAST(this, Double);
    return Double_unbox_d(a) == Double_unbox_d(b);
}

long _Double_getHashCode(Object this) {
    Double self = DOWNCAST(this, Double);
    return self.data;
}

static const Double_vtable_t _Double_vtable = {
        // Number
        .super = {
                // Object
                .super = {
                        .delete = _Double_delete,
                        .equals = _Double_equals,
                        .getHashCode = _Double_getHashCode,
                }
        },
        .unbox_d = _Double_unbox_d,
        .unbox_f = _Double_unbox_f,
};

const Double_vtable_t* Double_vtable() {
    return &_Double_vtable;
}

PRIMITIVE_SUPER_CAST_IMPL(Double, Number)
PRIMITIVE_UPCAST_IMPL(Double, Object)
Double Double_box_f(float unboxed) {
    Double box = {
            .vtable = &_Double_vtable,
            .data = (float)unboxed
    };
    return box;
}
Double Double_box_d(double unboxed) {
    Double box = {
            .vtable = &_Double_vtable,
            .data = unboxed
    };
    return box;
}