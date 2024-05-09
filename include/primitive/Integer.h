#pragma once
#include "../oop.h"
#include <stdlib.h>
#include "../Object.h"
#include "Number.h"

#pragma once
#include "oop.h"
#include <stdlib.h>

#define Super Number
#define Self Integer

START_CLASS

FORWARD_DECL_CLASS(String)

#define ENUMERATE_INTEGER_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(box_i, int i)                       \
    CONSTRUCTOR(box_l, long l)                      \
    CONSTRUCTOR(box_ll, long long ll)                      \
    CONSTRUCTOR(box_c, char c)

#define ENUMERATE_INTEGER_GETTERS(GETTER) \
    GETTER(int, unbox_i)                  \
    GETTER(long, unbox_l)                 \
    GETTER(long long, unbox_ll)                 \
    GETTER(char, unbox_c)

#define PARAMS_INVOCATION_Integer_toRadixString radix
#define ENUMERATE_INTEGER_METHODS(METHOD) \
    METHOD(String, toRadixString, unsigned char radix)

DEFINE_SELF_PRIMITIVE_CLASS(
        NO_IMPLEMENTS,
        ENUMERATE_INTEGER_METHODS,
        long long,
        ENUMERATE_INTEGER_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_INTEGER_GETTERS
)

DECLARE_SUPER_CAST(Integer, Number)
DECLARE_UPCAST(Integer, Object)

#define Integer$box(x) _Generic((x), \
    int: Integer$box_i(x),           \
    char: Integer$box_c(x),           \
    long: Integer$box_l(x),          \
    long long: Integer$box_ll(x),             \
    default: DOWNCAST(null, Integer))

END_CLASS
#undef Super
#undef Self
