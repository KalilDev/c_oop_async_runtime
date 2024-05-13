#define WITH_RTTI

#include <stdarg.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include "rtti.h"
#include "Object.h"
#include <math.h>

#ifdef WITH_RTTI
bool _isTerminator(char c) {
    return c == '\0' || c == ';';
}
long _sepIndex(const char* chain) {
    for (long i = 0; ; i++) {
        if (chain[i] == '\0') {
            return -1;
        }
        if (chain[i] == ';') {
            return i;
        }
    }
}
bool Object_isObjectTypeAssignable(Object obj, const char* class_name) {
    const char* inheritance_chain = obj.vtable->runtime_type_information.inheritance_chain;
    size_t chainLength = strlen(inheritance_chain);
    size_t classLength = strlen(class_name);

    while (chainLength > classLength) {
        if (strncmp(inheritance_chain, class_name, classLength) == 0 && _isTerminator(inheritance_chain[classLength])) {
            return true;
        }
        long sepIndex = _sepIndex(inheritance_chain);
        if (sepIndex < 0) {
            return false;
        }
        inheritance_chain += sepIndex + 1;
        chainLength -= sepIndex + 1;
    }
}

void initRtti(runtime_type_information_t* selfRtti, runtime_type_information_t *superRtti, const char* class_name, size_t implemented_interface_count, ...) {
    va_list args;
    va_start(args, implemented_interface_count);
    initRtti_va(selfRtti, superRtti, class_name, implemented_interface_count, args);
    va_end(args);
}

void initRtti_va(runtime_type_information_t* selfRtti, runtime_type_information_t *superRtti, const char* class_name, size_t implemented_interface_count, va_list args) {
    char *resultClassName = strdup(class_name);
    assert(resultClassName != NULL);
    char *resultInheritanceChain = NULL;
    if (superRtti != NULL) {
        const char* super = superRtti->inheritance_chain;
        assert(super != NULL);
        size_t superLen = strlen(super);
        size_t selfLen = strlen(class_name);
        resultInheritanceChain = malloc(superLen + 1 + selfLen + 1);
        size_t cursor = 0;
        assert(resultInheritanceChain != NULL);
        memcpy(resultInheritanceChain + cursor, super, superLen);
        cursor += superLen;
        resultInheritanceChain[cursor] = _INHERITANCE_CHAIN_SEPARATOR;
        cursor++;
        memcpy(resultInheritanceChain + cursor, class_name, selfLen);
        cursor += selfLen;
        resultInheritanceChain[cursor] = '\0';
    } else {
        resultInheritanceChain = strdup(class_name);
        assert(resultInheritanceChain != NULL);
    }
    char* resultInterfaceTable = NULL;
    if (implemented_interface_count == 0) {
        if (superRtti != NULL) {
            resultInterfaceTable = superRtti->interface_table == NULL ? NULL : strdup(superRtti->interface_table);
        } else {
            resultInterfaceTable = NULL;
        }
    } else {
        size_t capacity = 0;
        size_t length = 0;
        if (superRtti == NULL || superRtti->interface_table == NULL) {
            capacity = 16;
            resultInterfaceTable = malloc(capacity);
            assert(resultInterfaceTable != NULL);
        } else {
            length = strlen(superRtti->interface_table);
            capacity = length + 16;
            resultInterfaceTable = malloc(capacity);
            assert(resultInterfaceTable != NULL);
            memcpy(resultInterfaceTable, superRtti->interface_table, length);
        }
        do {
            char* interface_name = va_arg(args, char*);
            size_t interface_name_length = strlen(interface_name);
            size_t offset = va_arg(args, size_t);
            size_t neededBytes = interface_name_length;
            size_t offset_string_size = snprintf(NULL, 0,"%ld", offset);
            neededBytes += offset_string_size;
            if (implemented_interface_count != 0) {
                neededBytes++;
            }
            if (length + neededBytes >= capacity) {
                capacity = length + neededBytes + 16;
                resultInterfaceTable = realloc(resultInterfaceTable, capacity);
                assert(resultInterfaceTable != NULL);
            }

            memcpy(resultInterfaceTable + length, interface_name, interface_name_length);
            length += interface_name_length;
            resultInterfaceTable[length] = ':';
            length++;
            snprintf(resultInterfaceTable, offset_string_size, "%ld", offset);
            length+=offset_string_size;
            implemented_interface_count--;
        } while (implemented_interface_count != 0);
    }


    selfRtti->class_name = resultClassName;
    selfRtti->inheritance_chain = resultInheritanceChain;
    selfRtti->interface_table = resultInterfaceTable;
    for (size_t i = 0; i < implemented_interface_count; i++) {

    }
}

#endif