#pragma once

#include "bits/oop.h"
#include <stdarg.h>
#include <stdbool.h>
#ifdef WITH_RTTI

#define _INHERITANCE_CHAIN_SEPARATOR ';'
#define _INTERFACE_TABLE_SEPARATOR ';'
#define _INTERFACE_TABLE_ENTRY_SEPARATOR ':'
#define APPEND_INHERITANCE_CHAIN(chain, class) chain _INHERITANCE_CHAIN_SEPARATOR #class

#define ______STR(e) #e
#define _____STR(e) ______STR(e)
#define ____STR(e) _____STR(e)
#define ___STR(e) ____STR(e)
#define __STR(e) ___STR(e)
#define _STR(e) __STR(e)
#define STR(e) _STR(e)
#define APPEND_INTERFACE(interface_table, interface, offset) interface_table  #interface _INTERFACE_TABLE_ENTRY_SEPARATOR STR(offset) _INTERFACE_TABLE_SEPARATOR

typedef struct runtime_type_information_t {
    const char* class_name;
    const char* inheritance_chain;
    const char* interface_table;
} runtime_type_information_t;

FORWARD_DECL_CLASS(Object)
FORWARD_DECL_CLASS(Interface)
struct any; typedef struct any any;
bool Object_isObjectTypeAssignable(Object object, const char* class_name);
bool Object_isInterfaceTypeAssignable(Object object, const char* interface_name);
bool Object_isExactType(Object object, const char* class_name);
bool Interface_isObjectTypeAssignable(Interface interface, const char* class_name);
bool Interface_isInterfaceTypeAssignable(Interface interface, const char* interface_name);
bool Interface_isExactType(Interface interface, const char* class_name);
bool any_isObjectTypeAssignable(any any, const char* class_name);
bool any_isInterfaceTypeAssignable(any any, const char* interface_name);
bool any_isExactType(any any, const char* class_name);

// The var parameters are the interfaces
void initRtti(runtime_type_information_t* selfRtti, runtime_type_information_t *superRtti, const char* class_name, size_t implemented_interface_count, ...);
void initRtti_va(runtime_type_information_t* selfRtti, runtime_type_information_t *superRtti, const char* class_name, size_t implemented_interface_count, va_list args);


#endif