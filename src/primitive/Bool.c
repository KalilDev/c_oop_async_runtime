#include "Object.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "primitive/Bool.h"
#include "../oop.h"
#include "stddef.h"
#include "../oop.h"

#define Self Bool
#define Super() Object_vtable()

IMPLEMENT_SELF_GETTER(bool, unbox) {
    return this.data;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {

}

IMPLEMENT_OVERRIDE_METHOD(bool, Object, equals, Object other) {
    Bool a = DOWNCAST(this, Bool);
    // is bool
    if (other.vtable != Bool_vtable()) {
    return false;
    }
    Bool b = DOWNCAST(other, Bool);
    return a.data == b.data;
}
IMPLEMENT_OVERRIDE_METHOD(long, Object, getHashCode) {
    Bool self = DOWNCAST(this, Bool);
    return self.data;
}
static const char* _TrueString = "true";
static const char* _FalseString = "false";
IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    Bool self = DOWNCAST(this, Bool);
    if (this.data) {
        return StringRef_as_String(StringRef$wrap(_TrueString));
    }
    return StringRef_as_String(StringRef$wrap(_FalseString));
}


static const Bool_vtable_t _Bool_vtable = {
        .super = {
                .object_vtable_tag = OBJECT_VTABLE_TAG,
                .runtime_type_information = {
                        .class_name = "Bool",
                        .interface_table = NULL,
                        .inheritance_chain = "Object;Bool"
                },
                .toString = _Bool_toString_impl,
                .getHashCode = _Bool_getHashCode_impl,
                .equals = _Bool_equals_impl,
                .delete = _Bool_delete_impl
        }
};

const Bool_vtable_t* Bool_vtable() {
    return &_Bool_vtable;
}

const Bool True =  {
        .vtable = &_Bool_vtable,
        .data = true
};
const Bool False =  {
        .vtable = &_Bool_vtable,
        .data = false
};


PRIMITIVE_UPCAST_IMPL(Bool, Object)

// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(box, bool unboxed) {
    Bool this = {
            .vtable = Bool_vtable(),
            .data = unboxed
    };
    return this;
}

#undef Super
#undef Self