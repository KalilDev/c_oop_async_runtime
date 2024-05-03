#pragma once
#include <stdlib.h>
#include "oop.h"
#include "Object.h"

#define Self Interface
START_CLASS

typedef struct {
    size_t object_vtable_offset;
} Interface_vtable_t;

// An fat pointer to an interface
typedef struct Interface {
    const Interface_vtable_t* vtable;
    Object_data* data;
} Interface;

DECLARE_SELF_VTABLE_GETTER()

DECLARE_OBJECT_CAST(Interface, Object)
END_CLASS
#undef Self