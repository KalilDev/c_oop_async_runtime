#pragma once
#define CONCAT_(A, B) A ## B
#define CONCAT(A, B) CONCAT_(A, B)

#define METHOD_SIGNATURE_TYPE_NAME(class, method) _ ## class ## _ ## method ##  _signature
#define METHOD_SIGNATURE(class, method, return_type, signature) typedef return_type (*METHOD_SIGNATURE_TYPE_NAME(class, method))signature;
#define _DECLARE_METHOD(class, method, return_type, signature) METHOD_SIGNATURE(class, method, return_type, signature) \
    return_type class ## _ ## method signature;

#define _DECLARE_STATIC_METHOD(class, method, return_type, signature) return_type class ## _ ## method signature; \

#define _DECLARE_ABSTRACT_CONSTRUCTOR_METHOD(class, name, class_type, ...) \
        void class ## $ ## name (class_type this __VA_OPT__(,) __VA_ARGS__);

#define _DECLARE_CONSTRUCTOR_METHOD(class, name, class_type, ...) \
        class_type class ## $ ## make_ ## name (__VA_ARGS__); \
        _DECLARE_ABSTRACT_CONSTRUCTOR_METHOD(class, name, class_type __VA_OPT__(,) __VA_ARGS__); \



#define DECLARE_METHOD(return_type, class, class_type, method, ...) \
    _DECLARE_METHOD(class, method, return_type, (class_type this __VA_OPT__(,) __VA_ARGS__))


#define DECLARE_STATIC_METHOD(return_type, class, method, ...) \
    _DECLARE_STATIC_METHOD(class, method, return_type, (__VA_ARGS__))

#define DECLARE_SELF_STATIC_METHOD(return_type, method, ...) \
    DECLARE_STATIC_METHOD(return_type, Self, method __VA_OPT__(,) __VA_ARGS__)

#define DECLARE_CONSTRUCTOR(class, name, ...) \
    _DECLARE_CONSTRUCTOR_METHOD(class, name, Self, __VA_ARGS__)
#define DECLARE_ABSTRACT_CONSTRUCTOR(class, name, ...) \
    _DECLARE_ABSTRACT_CONSTRUCTOR_METHOD(class, name, Self, __VA_ARGS__)

#define DECLARE_SELF_CONSTRUCTOR(name, ...) \
    DECLARE_CONSTRUCTOR(Self, name __VA_OPT__(,) __VA_ARGS__)

#define DECLARE_SELF_ABSTRACT_CONSTRUCTOR(name, ...) \
    DECLARE_ABSTRACT_CONSTRUCTOR(Self, name __VA_OPT__(,) __VA_ARGS__)


#define DEFINE_VIRTUAL_METHOD(class, method) METHOD_SIGNATURE_TYPE_NAME(class, method) method;
#define DEFINE_SELF_VIRTUAL_METHOD(return_type, method, ...) DEFINE_VIRTUAL_METHOD(Self, method)
#define FORWARD_DECL_CLASS(class) struct class; \
typedef struct class class;

#define START_CLASS FORWARD_DECL_CLASS(Self)

#define END_CLASS

#define DECLARE_SUPER_CAST(class, Super) Super class ## _as_ ## Super(class this);
#define DECLARE_UPCAST(class, Other) Other class ## _as_ ## Other(class this);

#define NO_DATA {}
#define NO_METHODS {}
#define DECLARE_CLASS(class, Super, methods, data_body) \
    typedef struct { \
        Super ## _vtable_t super; \
        struct methods; \
    } class ## _vtable_t;                          \
                                                   \
typedef struct { \
    Super ## _data super;                          \
    struct data_body;                                                  \
} class ## _data;\
\
\
typedef struct class { \
    const class ## _vtable_t* vtable; \
    class ## _data* data;\
} class;

#define DECLARE_PRIMITIVE_CLASS(class, Super, methods, type) \
    _Static_assert(sizeof(type) <= sizeof(void*), "Invalid primitive type, it needs to fit in a pointer");                                                                    \
    typedef struct { \
        Super ## _vtable_t super; \
        struct methods; \
    } class ## _vtable_t;                          \
\
typedef struct class { \
    const class ## _vtable_t* vtable; \
    union {                                                  \
        type data;                                                     \
        void* _align;                                                         \
    }; \
} class;

#define DECLARE_SELF_METHOD(return_type, method, ...) DECLARE_METHOD(return_type, Self, Self, method, __VA_ARGS__)