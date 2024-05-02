#pragma once
#include <stdlib.h>
#include "oop.h"
#include "Object.h"

#define IMPLEMENTS(interface) \
    interface ## _vtable_t interface ## _vtable;

#define DECLARE_INTERFACE_CAST(class, interface) DECLARE_UPCAST(class, interface)

#define DECLARE_OBJECT_CAST(interface, class) DECLARE_UPCAST(interface, class)

#define DECLARE_INTERFACE(interface, Super, On, methods) \
    typedef struct { \
        Super ## _vtable_t super; \
        struct methods; \
    } interface ## _vtable_t;                          \
\
\
typedef struct interface { \
    const interface ## _vtable_t* vtable; \
    On ## _data* data;\
} interface;

struct Interface;

typedef struct {
    size_t vtable_offset;
} Interface_vtable_t;

// An fat pointer to an object
typedef struct Interface {
    const Interface_vtable_t* vtable;
    Object_data* data;
} Interface;

// The default impls for this interface
const Interface_vtable_t* Interface_vtable();
DECLARE_OBJECT_CAST(Interface, Object)