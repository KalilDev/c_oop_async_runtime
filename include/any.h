#pragma once
#include <stddef.h>
#include "rtti.h"

typedef union object_or_interface_vtable {
    size_t tag;
    struct {
        size_t interface_vtable_tag;
        size_t object_vtable_offset;
    };
    struct {
        size_t object_vtable_tag;
        #ifdef WITH_RTTI
        const runtime_type_information_t object_runtime_type_information;
        #endif
    };
} object_or_interface_vtable;

typedef struct any {
    object_or_interface_vtable* vtable;
    union {
        size_t primitive_value;
        void* object_data;
    };
} any;


#ifdef WITH_RTTI
const runtime_type_information_t* any_runtimeTypeInformation(any any);
#endif

bool any_isNull(any any);
Object any_asObject(any any);