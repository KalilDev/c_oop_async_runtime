#include "Object.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "primitive/Integer.h"
#include "primitive/Double.h"
#include "../oop.h"
#include "stddef.h"
#include "../oop.h"
#include <assert.h>
#include <stdio.h>

#define Self Double
#define Super() Number_vtable()

ENUMERATE_DOUBLE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_GETTER(float, unbox_f) {
    return (float) this.data;
}

IMPLEMENT_SELF_GETTER(double, unbox_d) {
    return (double) this.data;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {

}

IMPLEMENT_OVERRIDE_METHOD(bool, Object, equals, Object other) {
    Double a = DOWNCAST(this, Double);
    // is double
    if (other.vtable != Double_vtable()) {
        return false;
    }
    Double b = DOWNCAST(other, Double);
    return a.data == b.data;
}
IMPLEMENT_OVERRIDE_METHOD(long, Object, getHashCode) {
    Double self = DOWNCAST(this, Double);
    return *((long*)&self.data);
}

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    Double self = DOWNCAST(this, Double);
    double num = self.data;
    size_t length = snprintf(NULL, 0, "%lf", num);
    char *str = malloc(length + 1);
    snprintf(str, length + 1,"%lf", num);
    return String$make_own_len(str, length);
}

IMPLEMENT_OVERRIDE_METHOD(Number, Number, abs) {
    Double self = DOWNCAST(this, Double);
    if (self.data == 0) {
        return this;
    }
    if (self.data > 0) {
        return this;
    }
    return Double_as_Number(Double$box(-self.data));
}

IMPLEMENT_OVERRIDE_METHOD(Number, Number, clamp, Number lower, Number upper) {
    Double self = DOWNCAST(this, Double);
    Double l = Number_toDouble(lower);
    Double u = Number_toDouble(upper);
    if (self.data > u.data) {
        return Double_as_Number(u);
    }
    if (self.data < l.data) {
        return Double_as_Number(l);
    }
    return this;
}

IMPLEMENT_OVERRIDE_METHOD(Integer, Number, toInteger) {
    Double self = DOWNCAST(this, Double);
    return Integer$box_l((long)self.data);
}

IMPLEMENT_OVERRIDE_METHOD(Double, Number, toDouble) {
    Double self = DOWNCAST(this, Double);
    return self;
}
IMPLEMENT_OVERRIDE_METHOD(int, Comparable, compareTo, Object other) {
    Double thisD = DOWNCAST(this, Double);
    Double otherD = Number_toDouble(DOWNCAST(other, Number));
    double selfData = Double_unbox_d(thisD);
    double otherData = Double_unbox_d(otherD);
    if (selfData > otherData) {
        return 1;
    }
    if (selfData == otherData) {
        return 0;
    }
    return -1;
}

IMPLEMENT_SELF_VTABLE() {
    // Init the vtable
    initVtable(
            (Object_vtable_t *) vtable,
            (Object_vtable_t *) Super(),
            sizeof(*Super()),
            STR(Self),
            0
    );

    // Number
    Number_vtable_t *number_vtable = (Number_vtable_t *) vtable;
    number_vtable->abs = _Double_abs_impl;
    number_vtable->clamp = _Double_clamp_impl;
    number_vtable->toDouble = _Double_toDouble_impl;
    number_vtable->toDouble = _Double_toDouble_impl;
    // Comparable
    Comparable_vtable_t *comparable_vtable = &number_vtable->Comparable_vtable;
    comparable_vtable->compareTo = _Double_compareTo_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t *) vtable;
    object_vtable->delete = _Double_delete_impl;
    object_vtable->equals = _Double_equals_impl;
    object_vtable->getHashCode = _Double_getHashCode_impl;
    object_vtable->toString = _Double_toString_impl;
}

PRIMITIVE_SUPER_CAST_IMPL(Double, Number)

PRIMITIVE_UPCAST_IMPL(Double, Object)

// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(box_f, float unboxed) {
    Double this = {
            .vtable = Double_vtable(),
            .data = (float) unboxed
    };
    return this;
}

// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(box_d, double unboxed) {
    Double this = {
            .vtable = Double_vtable(),
            .data = (double) unboxed
    };
    return this;
}

#undef Super
#undef Self