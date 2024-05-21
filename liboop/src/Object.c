#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include "liboop.h"

#define Self Object

long CONCAT(CONCAT(Object, _), getHashCode)(Object this) {
    assert(this.vtable != NULL);
    assert(this.vtable->getHashCode != NULL);
    return _VIRTUAL_METHOD_INVOCATION(Object, getHashCode);
}

_Bool CONCAT(CONCAT(Object, _), equals)(Object this, Object other) {
    assert(this.vtable != NULL);
    assert(this.vtable->equals != NULL);
    return _VIRTUAL_METHOD_INVOCATION(Object, equals, Object other);
}

void CONCAT(CONCAT(Object, _), delete)(Object this) {
    assert(this.vtable != NULL);
    assert(this.vtable->delete != NULL);
    return _VIRTUAL_METHOD_INVOCATION(Object, delete);
}

String CONCAT(CONCAT(Object, _), toString)(Object this) {
    assert(this.vtable != NULL);
    assert(this.vtable->toString != NULL);
    return _VIRTUAL_METHOD_INVOCATION(Object, toString);
}

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_OBJECT_PARENTS)

IMPLEMENT_OPERATOR_NEW()


Object Object$$fromObject(Object this) {
    return this;
}

static void _Null_delete_impl(Object this) {

}

static bool _Null_equals_impl(Object this, Object other) {
    return Object_isNull(other);
}

static long _Null_getHashCode_impl(Object this) {
    return 0;
}

static const char *_NullString = "null";

static String _Null_toString_impl(Object this) {
    return StringRef$wrap(_NullString).asString;
}

typedef struct Null_vtable_t {
    Object_vtable_t super;

    void (*empty[1024])();
} Null_vtable_t;
static const Null_vtable_t _Null_vtable = {
        .super = {
                .object_vtable_tag = OBJECT_VTABLE_TAG,
#ifdef WITH_RTTI
                .runtime_type_information = {
                        .class_name = "Null",
                        .interface_table = NULL,
                        .inheritance_chain = "Null"
                },
#endif
                .toString = _Null_toString_impl,
                .getHashCode = _Null_getHashCode_impl,
                .equals = _Null_equals_impl,
                .delete = _Null_delete_impl
        },
        .empty = {0}
};

const Object null = {
        .vtable = &_Null_vtable.super,
        .data = NULL
};


IMPLEMENT_SELF_METHOD(long, getHashCode) {
return (long)this.
data;
}
IMPLEMENT_SELF_METHOD(bool, equals, Object other) {
return this.data == other.
data;
}
IMPLEMENT_SELF_METHOD(void, delete) {
return
Object_free_memory(this
.data);
}
IMPLEMENT_SELF_METHOD(String, toString) {
const char *class_name = "Object";
#ifdef WITH_RTTI
runtime_type_information_t *object_rtti = Object_runtimeTypeInformation(this);
class_name = object_rtti->class_name;
#endif
long hashCode = Object_getHashCode(this);
Integer hash = Integer$box(hashCode);
char *string = NULL;
size_t len = strlen(class_name);
String className = StringRef$wrap(class_name).asString;
// #
len += 1;

// \0
len += 1;
return String_format_c("{}#{}", className,
Integer_toRadixString(hash,
16));
}

IMPLEMENT_STATIC_METHOD(void*, allocate_memory, size_t size) {
    void *mem = malloc(size);
    memset(mem, 0, size);
    return mem;
}

IMPLEMENT_STATIC_METHOD(void, free_memory, void *memory) {
    return free(memory);
}

IMPLEMENT_STATIC_METHOD(void, delete_ptr, Object *pointer) {
    if (pointer == NULL) {
        return;
    }
    Object_delete(*pointer);
}

IMPLEMENT_STATIC_METHOD(void, delete_any_ptr, void *pointer) {
    if (pointer == NULL) {
        return;
    }
    Object_delete_ptr((Object *) pointer);
}

IMPLEMENT_STATIC_METHOD(bool, isNull, Object object) {
    return object.vtable == null.vtable && object.data == null.data;
}

#define ENUMERATE_OBJECT_CONSTRUCTORS(CONSTRUCTOR) \
    /* returns the hash code of the object */       \
    CONSTRUCTOR(new)

static const Object_vtable_t _Object_vtable = {
        .object_vtable_tag = OBJECT_VTABLE_TAG,
#ifdef WITH_RTTI
        .runtime_type_information = {
                .class_name = "Object",
                .inheritance_chain = "Object",
                .interface_table = "",
        },
#endif
        .getHashCode = _Object_getHashCode_impl,
        .equals = _Object_equals_impl,
        .delete = _Object_delete_impl,
        .toString = _Object_toString_impl
};


const Object_vtable_t *Object_vtable() {
    return &_Object_vtable;
}

Self CONCAT(CONCAT(CONCAT(Self, $), make_), new)() {
    Self this = CONCAT(Self, _operator_new)();
    if (Object_isNull(this)) { return this; }
    _CONSTRUCTOR_METHOD_INVOCATION(Self, new);
    return this;
}

IMPLEMENT_ABSTRACT_CONSTRUCTOR(new) {

}

#ifdef WITH_RTTI

const runtime_type_information_t *Object_runtimeTypeInformation(Object object) {
    return &object.vtable->runtime_type_information;
}

#endif


// The var parameters are the interfaces implemented
void
initVtable(Object_vtable_t *selfVtable, Object_vtable_t *superVtable, size_t superVtableSize, const char *class_name,
           size_t implemented_interface_count, ...) {
    assert(selfVtable != NULL);
    assert(superVtable != NULL);
    assert(selfVtable->object_vtable_tag != OBJECT_VTABLE_TAG);
    assert(superVtable->object_vtable_tag == OBJECT_VTABLE_TAG);
    if (superVtable != NULL) {
        memcpy(selfVtable, superVtable, superVtableSize);
    }
    selfVtable->object_vtable_tag = OBJECT_VTABLE_TAG;
#ifdef WITH_RTTI
    va_list args;
    va_start(args, implemented_interface_count);
    initRtti_va((runtime_type_information_t *) &selfVtable->runtime_type_information,
                (runtime_type_information_t *) &superVtable->runtime_type_information, class_name,
                implemented_interface_count, args);
    va_end(args);
#endif
}

bool any_isNull(any any) {
    return any_isExactType(any, "Null");
}

Object any_asObject(any any) {
    switch (any.vtable->tag) {
        case OBJECT_VTABLE_TAG:
            return DOWNCAST(any, Object);
        case INTERFACE_VTABLE_TAG: {
            size_t vtable_offset = any.vtable->object_vtable_offset;
            return Interface_as_Object(DOWNCAST(any, Interface));
        }
        default: {
            assert(!"FAILURE: Any is not an object or interface.");
        }
    }
}