#pragma once
#include "../oop.h"
#include <stdlib.h>
#include "../Object.h"
#include "Number.h"

#pragma once
#include "oop.h"
#include <stdlib.h>

#define Super Number
#define Self Double

#define ENUMERATE_DOUBLE_PARENTS(PARENT) \
    ENUMERATE_NUMBER_PARENTS(PARENT)                                     \
    PARENT(Number)

START_CLASS

FORWARD_DECL_CLASS(String)

#define ENUMERATE_DOUBLE_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(box_f, float f)                       \
    CONSTRUCTOR(box_d, double d)

#define ENUMERATE_DOUBLE_GETTERS(GETTER) \
    GETTER(float, unbox_f)                 \
    GETTER(double, unbox_d)


#define ENUMERATE_DOUBLE_METHODS(METHOD) \
    METHOD(String, toStringFixed)

DEFINE_SELF_PRIMITIVE_CLASS(
        ENUMERATE_DOUBLE_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_DOUBLE_METHODS,
        double,
        ENUMERATE_DOUBLE_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_DOUBLE_GETTERS
)

DECLARE_SUPER_CAST(Double, Number)
DECLARE_UPCAST(Double, Object)

#define Double$box(x) _Generic((x), \
    float: Double$box_f(x),           \
    double: Double$box_d(x),           \
    default: DOWNCAST(null, Double))

END_CLASS
#undef Super
#undef Self

