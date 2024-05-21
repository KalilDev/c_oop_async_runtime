#include "liboop.h"
#include "bits/oop.h"

#define Self Bool
#define Super() Object_vtable()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_BOOL_PARENTS)

IMPLEMENT_SELF_GETTER(bool, unbox) {
    return this.unwrap;
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
    return a.unwrap == b.unwrap;
}
IMPLEMENT_OVERRIDE_METHOD(long, Object, getHashCode) {
    Bool self = DOWNCAST(this, Bool);
    return self.unwrap;
}
static const char* _TrueString = "true";
static const char* _FalseString = "false";
IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    Bool self = DOWNCAST(this, Bool);
    if (self.unwrap) {
        return StringRef$wrap(_TrueString).asString;
    }
    return StringRef$wrap(_FalseString).asString;
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
        .unwrap = true
};
const Bool False =  {
        .vtable = &_Bool_vtable,
        .unwrap = false
};

// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(box, bool unboxed) {
    if (unboxed) {
        return True;
    }
    return False;
}

#undef Super
#undef Self