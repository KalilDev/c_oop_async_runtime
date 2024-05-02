#pragma once
#include "../oop.h"
#include <stdlib.h>
#include "../Object.h"

#define ENUMERATE_NUMBER_METHODS(METHOD) \
    METHOD(toRadixString)

struct Number;
struct String;
_DECLARE_METHOD(Number, toRadixString, struct String, (struct Number))

#define METHOD(method) DEFINE_VIRTUAL_METHOD(Number, method)

DECLARE_CLASS(Number, Object, {
ENUMERATE_NUMBER_METHODS(METHOD)\
}, NO_DATA)

#undef METHOD

const Number_vtable_t* Number_vtable();
DECLARE_SUPER_CAST(Number, Object)