#pragma once
#include "../oop.h"
#include <stdlib.h>
#include "../Object.h"
#include "Number.h"

#define ENUMERATE_DOUBLE_METHODS(METHOD) \
    METHOD(unbox_d)                          \
    METHOD(unbox_f)

#define ENUMERATE_DOUBLE_ATTRIBUTES(ATTRIBUTE) \

struct Double;

_DECLARE_METHOD(Double, unbox_f, float, (struct Double))
_DECLARE_METHOD(Double, unbox_d, double, (struct Double))

#define METHOD(method) DEFINE_VIRTUAL_METHOD(Double, method)

DECLARE_PRIMITIVE_CLASS(Double, Number, {
    ENUMERATE_DOUBLE_METHODS(METHOD)
}, double)

#undef METHOD

const Double_vtable_t* Double_vtable();

DECLARE_SUPER_CAST(Double, Number)
DECLARE_UPCAST(Double, Object)
Double Double_box_f(float);
Double Double_box_d(double);

#define _NotAnDouble(x) Object_null
#define Double_box(x) _Generic((x), \
    double: Double_box_d,           \
    float: Double_box_f,             \
    default: _NotAnDouble,)
