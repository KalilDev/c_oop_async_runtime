#pragma once
#include "../oop.h"
#include <stdlib.h>
#include "../Object.h"
#include "Number.h"

#define ENUMERATE_INTEGER_METHODS(METHOD) \
    METHOD(unbox_i)                          \
    METHOD(unbox_l)                       \
    METHOD(toRadixString)

#define ENUMERATE_INTEGER_ATTRIBUTES(ATTRIBUTE) \

struct Integer;

_DECLARE_METHOD(Integer, unbox_i, int, (struct Integer))
_DECLARE_METHOD(Integer, unbox_l, long, (struct Integer))
_DECLARE_METHOD(Integer, unbox_c, char, (struct Integer))

#define METHOD(method) DEFINE_VIRTUAL_METHOD(Integer, method)

DECLARE_PRIMITIVE_CLASS(Integer, Number, {
ENUMERATE_INTEGER_METHODS(METHOD)
}, long)

#undef METHOD

const Integer_vtable_t* Integer_vtable();

DECLARE_SUPER_CAST(Integer, Number)
DECLARE_UPCAST(Integer, Object)
Integer Integer_box_c(char);
Integer Integer_box_i(int);
Integer Integer_box_l(long);


#define Integer_box(x) _Generic((x), \
    int: Integer_box_i(x),           \
    char: Integer_box_c(x),           \
    long: Integer_box_l(x),             \
    default: DOWNCAST(Object_null, Integer))
