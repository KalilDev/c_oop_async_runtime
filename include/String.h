#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Object.h"
#include "Iterable.h"
#include "Interface.h"

#define ENUMERATE_STRING_METHODS(METHOD) \
    METHOD(at)                           \
    METHOD(concat)                       \
    METHOD(length)                       \
    METHOD(cStringView)

#define ENUMERATE_LIST_ATTRIBUTES(ATTRIBUTE) \

struct String;
struct Integer;
_DECLARE_METHOD(String, at, struct Integer, (struct String, size_t))
_DECLARE_METHOD(String, concat, struct String, (struct String, struct String))
_DECLARE_METHOD(String, length, size_t, (struct String))
_DECLARE_METHOD(String, cStringView, const char*, (struct String))

#define METHOD(method) DEFINE_VIRTUAL_METHOD(String, method)

DECLARE_CLASS(String, Object, {
//    IMPLEMENTS(Iterable)
    ENUMERATE_STRING_METHODS(METHOD)
}, {
    size_t length;
    char* c_string;
})

#undef METHOD

const String_vtable_t* String_vtable();

//DECLARE_OBJECT_CAST(Iterable, String)
//DECLARE_INTERFACE_CAST(String, Iterable)

DECLARE_SUPER_CAST(String, Object)

String String_alloc();
void String_construct(String, const char* ref);
void String_construct_own(String, char* cString);
void String_construct_own_len(String, char* cString, size_t len);
String String_new(const char* ref);
String String_own(char* cString);
String String_own_len(char* cString, size_t len);
String String_from_ref(const char* ref);