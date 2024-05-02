#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Object.h"
#include "Iterable.h"
#include "primitive/Integer.h"
#include "Interface.h"
#include "String.h"

DECLARE_PRIMITIVE_CLASS(StringRef, String, NO_METHODS, const char*)

const StringRef_vtable_t* StringRef_vtable();

DECLARE_SUPER_CAST(StringRef, String)
DECLARE_UPCAST(StringRef, Object)

StringRef StringRef_alloc();
void StringRef_construct(StringRef, const char* ref);
StringRef StringRef_new(const char* ref);
