#include "Object.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "primitive/Integer.h"
#include "primitive/Double.h"
#include "../oop.h"
#include "stddef.h"
#include "../oop.h"
#include "string.h"
#include <strings.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define Self Integer
#define Super() Number_vtable()

ENUMERATE_INTEGER_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_GETTER(int, unbox_i) {
    return (int)this.data;
}

IMPLEMENT_SELF_GETTER(long, unbox_l) {
    return (long)this.data;
}
IMPLEMENT_SELF_GETTER(long long, unbox_ll) {
    return (long long)this.data;
}
IMPLEMENT_SELF_GETTER(char, unbox_c) {
    return (char)this.data;
}


IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {

}

IMPLEMENT_OVERRIDE_METHOD(bool, Object, equals, Object other) {
    Integer a = DOWNCAST(this, Integer);
    // is integer
    if (other.vtable != Integer_vtable()) {
        return false;
    }
    Integer b = DOWNCAST(other, Integer);
    return a.data == b.data;
}
IMPLEMENT_OVERRIDE_METHOD(long, Object, getHashCode) {
    Integer self = DOWNCAST(this, Integer);
    return self.data;
}

IMPLEMENT_OVERRIDE_METHOD(String, Object, toString) {
    Integer self = DOWNCAST(this, Integer);
    long num = self.data;
    size_t length = snprintf(NULL, 0,"%ld", num);
    char *str = malloc(length + 1);
    int res = snprintf(str, length + 1, "%ld", num);
    return String$make_own_len(str, length);
}

IMPLEMENT_OVERRIDE_METHOD(Number, Number, abs) {
    Integer self = DOWNCAST(this, Integer);
    if (self.data == 0) {
        return this;
    }
    if (self.data > 0) {
        return this;
    }
    return Integer_as_Number(Integer$box(-self.data));
}

IMPLEMENT_OVERRIDE_METHOD(Number, Number, clamp, Number lower, Number upper) {
    Integer self = DOWNCAST(this, Integer);
    Integer l = Number_toInteger(lower);
    Integer u = Number_toInteger(upper);
    if (self.data > u.data) {
        return Integer_as_Number(u);
    }
    if (self.data < l.data) {
        return Integer_as_Number(l);
    }
    return this;
}

IMPLEMENT_OVERRIDE_METHOD(Double, Number, toDouble) {
    Integer self = DOWNCAST(this, Integer);
    return Double$box_d((double)self.data);
}

IMPLEMENT_OVERRIDE_METHOD(Integer, Number, toInteger) {
    Integer self = DOWNCAST(this, Integer);
    return self;
}
IMPLEMENT_OVERRIDE_METHOD(int, Comparable, compareTo, Object other) {
    Double thisD = Number_toDouble(DOWNCAST(this, Number));
    Double otherD = Number_toDouble(DOWNCAST(other, Number));
    double selfData = Double_unbox_d(thisD);
    double otherData = Double_unbox_d(otherD);
    if (selfData > otherData) {
        return 1;
    }
    if (selfData == otherData) {
        return 0;
    }
    return -1;
}
// Calculate the number of digits needed to represent an integer in an arbitrary base
size_t num_digits(long long value, unsigned int radix) {
    return (value == 0) ? 1 : (size_t)(log((double)value) / log((double)radix)) + 1;
}
IMPLEMENT_SELF_METHOD(String, toRadixString, unsigned char radix) {
    assert(radix <= 'z' - 'a' + 10);
    long long value = this.data;
    size_t num_digits_needed = num_digits(value, radix);
    char* buffer = (char*)malloc(num_digits_needed + 1); // Allocate buffer (+1 for null terminator)
    size_t index = num_digits_needed; // Start filling the buffer from the end

    // Convert the integer to string in the specified base
    do {
        char remainder = (char)(value % radix);
        char strChar;
        if (remainder < 10) {
            strChar = (char)('0' + remainder);
        } else {
            strChar = (char)('A' + remainder);
        }
        buffer[--index] = strChar;
        value /= radix;
    } while (value != 0);

    // Null-terminate the string
    buffer[num_digits_needed] = '\0';

    return String$make_own(buffer);
}

IMPLEMENT_SELF_VTABLE() {
    // Init the vtable
    initVtable(
            (Object_vtable_t*)vtable,
            (Object_vtable_t*)Super(),
            sizeof(*Super()),
            STR(Self),
            0
    );
    // Integer
    vtable->toRadixString = _Integer_toRadixString_impl;
    // Number
    Number_vtable_t *number_vtable = (Number_vtable_t *)vtable;
    number_vtable->abs = _Integer_abs_impl;
    number_vtable->clamp = _Integer_clamp_impl;
    number_vtable->toDouble = _Integer_toDouble_impl;
    number_vtable->toInteger = _Integer_toInteger_impl;
    // Comparable
    Comparable_vtable_t *comparable_vtable = &number_vtable->Comparable_vtable;
    comparable_vtable->compareTo = _Integer_compareTo_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _Integer_delete_impl;
    object_vtable->equals = _Integer_equals_impl;
    object_vtable->getHashCode = _Integer_getHashCode_impl;
    object_vtable->toString = _Integer_toString_impl;
}

PRIMITIVE_SUPER_CAST_IMPL(Integer, Number)
PRIMITIVE_UPCAST_IMPL(Integer, Object)

// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(box_c, char unboxed) {
    Integer this = {
        .vtable = Integer_vtable(),
        .data = (long)unboxed
    };
    return this;
}

// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(box_i, int unboxed) {
    Integer this = {
            .vtable = Integer_vtable(),
            .data = (long)unboxed
    };
    return this;
}


// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(box_l, long unboxed) {
    Integer this = {
            .vtable = Integer_vtable(),
            .data = (long)unboxed
    };
    return this;
}

// TODO: revisit this
IMPLEMENT_PRIMITIVE_CONSTRUCTOR(box_ll, long long unboxed) {
    Integer this = {
            .vtable = Integer_vtable(),
            .data = (long)unboxed
    };
    return this;
}

#undef Super
#undef Self