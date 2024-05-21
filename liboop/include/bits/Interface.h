#pragma once
#include <stdlib.h>
#include "bits/oop.h"
#include "bits/Object.h"

#define Self Interface
START_CLASS

typedef struct {
    size_t interface_vtable_tag;
    size_t object_vtable_offset;
} Interface_vtable_t;

// An fat pointer to an interface
typedef union Interface {
    struct {
        const Interface_vtable_t* vtable;
        Object_data* data;
    };
    any asAny;
} Interface;

DECLARE_SELF_VTABLE_GETTER()

void initInterfaceVtable(Interface_vtable_t * selfVtable, const Interface_vtable_t *superVtable, size_t superVtableSize);

void initImplementedInterfaceVtable(Interface_vtable_t * implementedVtable, const Interface_vtable_t *baseVtable, size_t implementedVtableSize, size_t offset);

DECLARE_OBJECT_CAST(Interface, Object)
END_CLASS
#undef Self