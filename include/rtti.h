#pragma once

#ifdef WITH_RTTI
#define _INHERITANCE_CHAIN_SEPARATOR ";"
#define _INTERFACE_TABLE_SEPARATOR ";"
#define _INTERFACE_TABLE_ENTRY_SEPARATOR ":"
#define APPEND_INHERITANCE_CHAIN(chain, class) chain _INHERITANCE_CHAIN_SEPARATOR #class
#define APPEND_INTERFACE(interface_table, interface, offset) interface_table  #interface _INTERFACE_TABLE_ENTRY_SEPARATOR #offset _INTERFACE_TABLE_SEPARATOR

typedef struct runtime_type_information_t {
    const char* class_name;
    const char* inheritance_chain;
    const char* interface_table;
} runtime_type_information_t;
#endif