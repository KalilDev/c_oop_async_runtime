#pragma once
#include "../oop.h"
#include <stdlib.h>
#include "../Object.h"

#define ENUMERATE_BOOL_METHODS(METHOD) \
    METHOD(unbox)

struct Bool;

_DECLARE_METHOD(Bool, unbox, bool, (struct Bool))

#define METHOD(method) DEFINE_VIRTUAL_METHOD(Bool, method)

DECLARE_PRIMITIVE_CLASS(Bool, Object, {
ENUMERATE_BOOL_METHODS(METHOD)
}, bool)

#undef METHOD

const Bool_vtable_t* Bool_vtable();

const Bool True;
const Bool False;

DECLARE_SUPER_CAST(Bool, Object)
Bool Bool_box(bool);