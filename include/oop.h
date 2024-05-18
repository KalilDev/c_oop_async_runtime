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
#define _DECLARE_PRIMITIVE_CONSTRUCTOR_METHOD(class, name, ...) \
        class class ## $ ## name (__VA_ARGS__);

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
#define DECLARE_PRIMITIVE_CONSTRUCTOR(class, name, ...) \
    _DECLARE_PRIMITIVE_CONSTRUCTOR_METHOD(class, name, __VA_ARGS__)

#define DECLARE_SELF_CONSTRUCTOR(name, ...) \
    DECLARE_CONSTRUCTOR(Self, name __VA_OPT__(,) __VA_ARGS__)

#define DECLARE_SELF_ABSTRACT_CONSTRUCTOR(name, ...) \
    DECLARE_ABSTRACT_CONSTRUCTOR(Self, name __VA_OPT__(,) __VA_ARGS__)

#define DECLARE_SELF_PRIMITIVE_CONSTRUCTOR(name, ...) \
    DECLARE_PRIMITIVE_CONSTRUCTOR(Self, name __VA_OPT__(,) __VA_ARGS__)


#define DEFINE_ATTRIBUTE(type, attribute) type attribute;
#define DEFINE_STATIC_ATTRIBUTE(class, type, attribute) extern DEFINE_ATTRIBUTE(type, CONCAT(CONCAT(class, _), attribute))
#define DEFINE_STATIC_SELF_ATTRIBUTE(type, attribute) DEFINE_STATIC_ATTRIBUTE(Self, type, attribute)
#define DEFINE_VIRTUAL_METHOD(class, method) DEFINE_ATTRIBUTE(METHOD_SIGNATURE_TYPE_NAME(class, method), method)
#define DEFINE_SELF_VIRTUAL_METHOD(return_type, method, ...) DEFINE_VIRTUAL_METHOD(Self, method)
#define DEFINE_IMPLEMENT(class) DEFINE_ATTRIBUTE(CONCAT(class, _vtable_t), CONCAT(class, _vtable))
#define FORWARD_DECL_CLASS(class) union class; \
typedef union class class;

#define FORWARD_DECL_THROWABLE FORWARD_DECL_CLASS(Throwable)

#define THROWS Throwable* __exception__
#define THROWS_PARAM_INVOCATION __exception__

#define START_CLASS FORWARD_DECL_CLASS(Self)

#define END_CLASS

#define DECLARE_SUPER_CAST(class, Super) Super class ## _as_ ## Super(class this);
#define DECLARE_UPCAST(class, Other) Other class ## _as_ ## Other(class this);

#define _DECLARE_PARENT_CAST(Parent) Parent CONCAT(as, Parent);

#define DECLARE_PRIMITIVE_FAT_POINTER(class, type, ENUMERATE_PARENTS) \
_Static_assert(sizeof(type) <= sizeof(void*), "Invalid primitive type, it needs to fit in a pointer"); \
typedef union class { \
    struct {                                              \
        const CONCAT(class, _vtable_t)* vtable; \
        union {                                                  \
            type unwrap;                                                     \
            void* _align;                                                         \
        };                                             \
    };                                             \
    ENUMERATE_PARENTS(_DECLARE_PARENT_CAST)                                                                  \
    any asAny;\
} class;

//#define DECLARE_PRIMITIVE_CLASS(class, Super, methods, type) \
//    _Static_assert(sizeof(type) <= sizeof(void*), "Invalid primitive type, it needs to fit in a pointer");                                                                    \
//    typedef struct { \
//        Super ## _vtable_t super; \
//        struct methods; \
//    } class ## _vtable_                                                                    \                                                   t;                          \
//\
//typedef struct class { \
//    const class ## _vtable_t* vtable; \
//    union {                                                  \
//        type data;                                                     \
//        void* _align;                                                         \
//    }; \
//} class;

#define DECLARE_SELF_METHOD(return_type, method, ...) DECLARE_METHOD(return_type, Self, Self, method, __VA_ARGS__)
#define DECLARE_SELF_GETTER(type, attribute) DECLARE_SELF_METHOD(type, attribute)

#define DECLARE_SELF_FAT_POINTER(ENUMERATE_PARENTS) \
/* An fat pointer to an Object */ \
typedef union Self { \
    struct {                       \
        const CONCAT(Self, _vtable_t)* vtable; \
        CONCAT(Self, _data)* data; \
    };                             \
    any asAny;                     \
    ENUMERATE_PARENTS(_DECLARE_PARENT_CAST)                                                                  \
} Self;


#define DECLARE_SELF_VTABLE_GETTER() \
    const CONCAT(Self, _vtable_t)* CONCAT(Self, _vtable)();

#define DEFINE_SELF_VTABLE(ENUMERATE_IMPLEMENENTS, ENUMERATE_METHODS) \
typedef struct CONCAT(Self, _vtable_t) { \
    CONCAT(Super, _vtable_t) super; \
    ENUMERATE_IMPLEMENENTS(DEFINE_IMPLEMENT) \
    ENUMERATE_METHODS(DEFINE_SELF_VIRTUAL_METHOD) \
} CONCAT(Self, _vtable_t);

#define DEFINE_SELF_DATA(Super, ENUMERATE_ATTRIBUTES) \
typedef struct CONCAT(Self, _data) {                  \
    CONCAT(Super, _data) super;                                                  \
    ENUMERATE_ATTRIBUTES(DEFINE_ATTRIBUTE) \
} CONCAT(Self, _data);


#define NO_IMPLEMENTS(IMPLEMENTS)
#define NO_STATIC_METHODS(METHOD)
#define NO_ATTRIBUTES(ATTRIBUTE)
#define NO_STATIC_ATTRIBUTES(ATTRIBUTE)
#define NO_METHODS(METHOD)
#define NO_GETTERS(ATTRIBUTE)

#define _DEFINE_DELF_DOWNCAST(Parent) \
    Self CONCAT(CONCAT(Self, $$from), Parent)(Parent parent);

#define DEFINE_SELF_DOWNCASTS(ENUMERATE_PARENTS) \
    ENUMERATE_PARENTS(_DEFINE_DELF_DOWNCAST)


#define DEFINE_SELF_ABSTRACT(ENUMERATE_PARENTS, ENUMERATE_IMPLEMENENTS, ENUMERATE_METHODS, ENUMERATE_ATTRIBUTES, ENUMERATE_CONSTRUCTORS, ENUMERATE_STATIC_METHODS, ENUMERATE_STATIC_ATTRIBUTES, ENUMERATE_GETTERS) \
ENUMERATE_METHODS(DECLARE_SELF_METHOD) \
DEFINE_SELF_VTABLE(ENUMERATE_IMPLEMENENTS, ENUMERATE_METHODS) \
DEFINE_SELF_DATA(Super, ENUMERATE_ATTRIBUTES) \
DECLARE_SELF_FAT_POINTER(ENUMERATE_PARENTS) \
DECLARE_SELF_VTABLE_GETTER()                                                                                                                                                   \
ENUMERATE_GETTERS(DECLARE_SELF_GETTER) \
ENUMERATE_STATIC_METHODS(DECLARE_SELF_STATIC_METHOD) \
ENUMERATE_CONSTRUCTORS(DECLARE_SELF_ABSTRACT_CONSTRUCTOR) \
DEFINE_SELF_DOWNCASTS(ENUMERATE_PARENTS) \
ENUMERATE_STATIC_ATTRIBUTES(DEFINE_STATIC_SELF_ATTRIBUTE)

#define DECLARE_SELF_PRIMITIVE_FAT_POINTER(primitiveType, ENUMERATE_PARENTS) DECLARE_PRIMITIVE_FAT_POINTER(Self, primitiveType, ENUMERATE_PARENTS)

#define DEFINE_SELF_PRIMITIVE_CLASS(ENUMERATE_PARENTS, ENUMERATE_IMPLEMENENTS, ENUMERATE_METHODS, primitiveType, ENUMERATE_CONSTRUCTORS, ENUMERATE_STATIC_METHODS, ENUMERATE_STATIC_ATTRIBUTES, ENUMERATE_GETTERS) \
ENUMERATE_METHODS(DECLARE_SELF_METHOD) \
DEFINE_SELF_VTABLE(ENUMERATE_IMPLEMENENTS, ENUMERATE_METHODS) \
DECLARE_SELF_PRIMITIVE_FAT_POINTER(primitiveType, ENUMERATE_PARENTS) \
DECLARE_SELF_VTABLE_GETTER() \
ENUMERATE_GETTERS(DECLARE_SELF_GETTER) \
ENUMERATE_STATIC_METHODS(DECLARE_SELF_STATIC_METHOD) \
ENUMERATE_CONSTRUCTORS(DECLARE_SELF_PRIMITIVE_CONSTRUCTOR)                                                                                                                                                         \
DEFINE_SELF_DOWNCASTS(ENUMERATE_PARENTS) \
ENUMERATE_STATIC_ATTRIBUTES(DEFINE_STATIC_SELF_ATTRIBUTE)

#define DECLARE_SELF_OPERATOR_NEW() \
    Self CONCAT(Self, _operator_new)();

#define DEFINE_SELF_CLASS(ENUMERATE_PARENTS, ENUMERATE_IMPLEMENENTS, ENUMERATE_METHODS, ENUMERATE_ATTRIBUTES, ENUMERATE_CONSTRUCTORS, ENUMERATE_STATIC_METHODS, ENUMERATE_STATIC_ATTRIBUTES, ENUMERATE_GETTERS) \
ENUMERATE_METHODS(DECLARE_SELF_METHOD) \
DEFINE_SELF_VTABLE(ENUMERATE_IMPLEMENENTS, ENUMERATE_METHODS) \
DEFINE_SELF_DATA(Super, ENUMERATE_ATTRIBUTES) \
DECLARE_SELF_FAT_POINTER(ENUMERATE_PARENTS) \
DECLARE_SELF_VTABLE_GETTER()                                                                                                                                                                 \
DECLARE_SELF_OPERATOR_NEW()                                                                                                                                                                                              \
ENUMERATE_GETTERS(DECLARE_SELF_GETTER) \
ENUMERATE_STATIC_METHODS(DECLARE_SELF_STATIC_METHOD) \
ENUMERATE_CONSTRUCTORS(DECLARE_SELF_CONSTRUCTOR) \
DEFINE_SELF_DOWNCASTS(ENUMERATE_PARENTS) \
ENUMERATE_STATIC_ATTRIBUTES(DEFINE_STATIC_SELF_ATTRIBUTE)

#define OBJECT_VTABLE_TAG 0xBABACA0000BABACA
#define INTERFACE_VTABLE_TAG 0x00BABACABABACA00

// Interface

#define IMPLEMENTS(interface) \
    interface ## _vtable_t interface ## _vtable;

#define DEFINE_SELF_INTERFACE_VTABLE(On, ENUMERATE_METHODS) \
typedef struct CONCAT(Self, _vtable_t) { \
    Interface_vtable_t _interface; \
    ENUMERATE_METHODS(DEFINE_SELF_VIRTUAL_METHOD) \
} CONCAT(Self, _vtable_t);
#define DEFINE_SELF_EXTENDED_INTERFACE_VTABLE(On, ENUMERATE_METHODS, SuperInterface) \
typedef struct CONCAT(Self, _vtable_t) { \
    CONCAT(SuperInterface, _vtable_t) super; \
    ENUMERATE_METHODS(DEFINE_SELF_VIRTUAL_METHOD) \
} CONCAT(Self, _vtable_t);

#define DECLARE_SELF_INTERFACE_FAT_POINTER(On, ...) \
/* An fat pointer to an Object */ \
typedef union Self { \
    struct {                       \
        const CONCAT(Self, _vtable_t)* vtable; \
        On ## _data* data;\
    };                             \
    any asAny;                                      \
    __VA_OPT__(__VA_ARGS__ CONCAT(as, __VA_ARGS__);)                                                \
    Interface asInterface;\
} Self;


#define DECLARE_INTERFACE_CAST(class, interface) DECLARE_UPCAST(class, interface)
#define DECLARE_OBJECT_CAST(interface, class) DECLARE_UPCAST(interface, class)
#define DEFINE_SELF_INTERFACE(On, ENUMERATE_METHODS, ENUMERATE_STATIC_METHODS, ENUMERATE_STATIC_ATTRIBUTES) \
ENUMERATE_METHODS(DECLARE_SELF_METHOD) \
DEFINE_SELF_INTERFACE_VTABLE(On, ENUMERATE_METHODS) \
DECLARE_SELF_INTERFACE_FAT_POINTER(On) \
DECLARE_SELF_VTABLE_GETTER() \
ENUMERATE_STATIC_METHODS(DECLARE_SELF_STATIC_METHOD) \
ENUMERATE_STATIC_ATTRIBUTES(DEFINE_STATIC_SELF_ATTRIBUTE)

#define DEFINE_SELF_EXTENDED_INTERFACE(SuperInterface, On, ENUMERATE_METHODS, ENUMERATE_STATIC_METHODS, ENUMERATE_STATIC_ATTRIBUTES) \
ENUMERATE_METHODS(DECLARE_SELF_METHOD) \
DEFINE_SELF_EXTENDED_INTERFACE_VTABLE(On, ENUMERATE_METHODS, SuperInterface) \
DECLARE_SELF_INTERFACE_FAT_POINTER(On, SuperInterface) \
DECLARE_SELF_VTABLE_GETTER() \
ENUMERATE_STATIC_METHODS(DECLARE_SELF_STATIC_METHOD) \
ENUMERATE_STATIC_ATTRIBUTES(DEFINE_STATIC_SELF_ATTRIBUTE)