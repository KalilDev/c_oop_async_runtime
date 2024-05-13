#define CONCAT_(A, B) A ## B
#define CONCAT(A, B) CONCAT_(A, B)


#define SUPER_INTERFACE_CAST_IMPL(class, Super) Super class ## _as_ ## Super(class this) { \
    Super super = {                                                               \
        .vtable = (CONCAT(Super, _vtable_t)*)&this.vtable,                                 \
        .data = this.data                                      \
    };                                                                            \
    return super;                                                                     \
}
#define CRASH_ON_EXCEPTION NULL
#define SUPER_INTERFACE_CAST_IMPL_(class, Super, Class) Super class ## _as_ ## Super(class this) { \
    Super super = {                                                               \
        .vtable = (CONCAT(Super, _vtable_t)*)&this.vtable,                                 \
        .data = (Class ## _data*)this.data                                      \
    };                                                                            \
    return super;                                                                     \
}


#define VIRTUAL_METHOD_IMPL(class, method, return_type, signature, params) \
    return_type class ## _ ## method signature {           \
        return this.vtable->method params;                                                       \
    };
#define OVERRIDE_IMPL(return_type, class, method, super, ...) return_type _ ## class ## _ ## method(super this __VA_OPT__(,) __VA_ARGS__)

#define METHOD_IMPL(class, method, return_type, signature, params) \
    VIRTUAL_METHOD_IMPL(class, method, return_type, signature, params)             \
    return_type _ ## class ## _ ## method signature

#define DOWNCAST(var, class) (*((class*) (&var)))
#define UPCAST(var, class) (*((class*) (&var)))

#define OBJECT_CAST_IMPL(interface, class) class interface ## _as_ ## class(interface this) { \
    size_t vtable_offset = ((Interface_vtable_t*)this.vtable)->object_vtable_offset;                                                                                          \
    class other = {                                                               \
        .vtable = (class ## _vtable_t*)(((char *)this.vtable) - vtable_offset),                                 \
        .data = (class ## _data*)this.data                                       \
    };                                                                            \
    return other;                                                                     \
}


#define INTERFACE_CAST_IMPL(class, interface, interface_base) interface class ## _as_ ## interface(class this) { \
    interface other = {                                                               \
        .vtable = &this.vtable->interface ## _vtable,                                 \
        .data = (interface_base ## _data*)this.data                                       \
    };                                                                            \
    return other;                                                                     \
}


// ok
#define _CONSTRUCTOR_METHOD_INVOCATION(class, method, ...) CONCAT(CONCAT(class, $), method)(this __VA_OPT__(,) __VA_OPT__(CONCAT(CONCAT(CONCAT(CONCAT(PARAMS_INVOCATION, _), class), $), method)))

// ok
#define _VIRTUAL_METHOD_INVOCATION(class, method, ...) this.vtable->method(this __VA_OPT__(,) __VA_OPT__(PARAMS_INVOCATION ## _ ## class ## _ ## method))
// ok
#define VIRTUAL_METHOD_IMPLEMENTATION(return_type, class, method, ...) \
return_type CONCAT(CONCAT(class, _), method)(class this __VA_OPT__(,) __VA_ARGS__) { \
    assert(this.vtable->method != NULL);                                                                   \
    return _VIRTUAL_METHOD_INVOCATION(class, method __VA_OPT__(,) __VA_ARGS__);                                                                       \
}

// ok
#define IMPLEMENT_SELF_VIRTUAL_METHOD(return_type, method, ...) VIRTUAL_METHOD_IMPLEMENTATION(return_type, Self, method __VA_OPT__(,) __VA_ARGS__)
// ok
#define IMPLEMENT_STATIC_METHOD(return_type, static_method, ...) return_type CONCAT(CONCAT(Self, _), static_method)(__VA_ARGS__)
// ok
#define IMPLEMENT_METHOD(return_type, class, Self, method, ...) return_type CONCAT(_, CONCAT(CONCAT(CONCAT(class, _), method), _impl))(Self this __VA_OPT__(,) __VA_ARGS__)
// ok
#define IMPLEMENT_GETTER(return_type, class, getter) return_type CONCAT(CONCAT(class, _), getter)(class this)
// ok
#define IMPLEMENT_SELF_GETTER(return_type, getter) IMPLEMENT_GETTER(return_type, Self, getter)
// ok
#define IMPLEMENT_OVERRIDE_METHOD(return_type, class, method, ...) IMPLEMENT_METHOD(return_type, Self, class, method __VA_OPT__(,) __VA_ARGS__)
// ok
#define IMPLEMENT_SELF_METHOD(return_type, method, ...) IMPLEMENT_METHOD(return_type, Self, Self, method __VA_OPT__(,) __VA_ARGS__)

// ok
#define IMPLEMENT_SELF_VTABLE() \
    /* forward decl */                            \
    static void CONCAT(_make_, CONCAT(Self, _vtable))(CONCAT(Self, _vtable_t)*vtable); \
    static CONCAT(CONCAT(Self, _vtable), _t) CONCAT(_, CONCAT(Self, _vtable)) = {0}; \
    const CONCAT(Self, _vtable_t)* CONCAT(Self, _vtable)() {                    \
        CONCAT(CONCAT(Self, _vtable), _t) *vtable = &CONCAT(_, CONCAT(Self, _vtable)); \
        if (((Object_vtable_t*)vtable)->object_vtable_tag != OBJECT_VTABLE_TAG) {      \
            /* The vtable was not initialized yet */                          \
            /* initialize it*/                    \
            CONCAT(_make_, CONCAT(Self, _vtable))(vtable);                     \
            /* it must have been initialized correctly */                     \
            assert(((Object_vtable_t*)vtable)->object_vtable_tag == OBJECT_VTABLE_TAG);                        \
        }                       \
        return vtable;                            \
    }\
    static void CONCAT(_make_, CONCAT(Self, _vtable))(CONCAT(Self, _vtable_t)*vtable)


// ok
#define IMPLEMENT_SELF_INTERFACE_VTABLE() \
    /* forward decl */                            \
    static void CONCAT(_make_, CONCAT(Self, _vtable))(CONCAT(Self, _vtable_t)*vtable); \
    static CONCAT(CONCAT(Self, _vtable), _t) CONCAT(_, CONCAT(Self, _vtable)) = {0}; \
    const CONCAT(Self, _vtable_t)* CONCAT(Self, _vtable)() {                    \
        CONCAT(CONCAT(Self, _vtable), _t) *vtable = &CONCAT(_, CONCAT(Self, _vtable)); \
        if (((Interface_vtable_t*)vtable)->interface_vtable_tag != INTERFACE_VTABLE_TAG) {      \
            /* The vtable was not initialized yet */                          \
            /* initialize it*/                    \
            CONCAT(_make_, CONCAT(Self, _vtable))(vtable);                     \
            /* it must have been initialized correctly */                     \
            assert(((Interface_vtable_t*)vtable)->interface_vtable_tag == INTERFACE_VTABLE_TAG);                        \
        }                       \
        return vtable;                            \
    }\
    static void CONCAT(_make_, CONCAT(Self, _vtable))(CONCAT(Self, _vtable_t)*vtable)



#define IMPLEMENT_OPERATOR_NEW() Self CONCAT(Self, _operator_new)() { \
    CONCAT(Self, _data)* data = (CONCAT(Self, _data)*)Object_allocate_memory(sizeof(CONCAT(Self, _data))); \
    if (data == NULL) {                                          \
        return DOWNCAST(null, Self);                                      \
    }                                                             \
    Self this = {                                                \
        .vtable = CONCAT(Self, _vtable)(),                       \
        .data = data\
    };                                                           \
    return this; \
}

#define IMPLEMENT_ABSTRACT_CONSTRUCTOR(name, ...) void CONCAT(CONCAT(Self, $), name) (Self this __VA_OPT__(,) __VA_ARGS__)

// ok
#define IMPLEMENT_CONSTRUCTOR(name, ...) \
        Self CONCAT(CONCAT(CONCAT(Self, $), make_), name)(__VA_ARGS__) { \
            Self this = CONCAT(Self, _operator_new)();                         \
            if (Object_isNull(this.asObject)) {               \
                return this;\
            }                            \
            _CONSTRUCTOR_METHOD_INVOCATION(Self, name __VA_OPT__(,) __VA_ARGS__);\
            return this;\
        } \
        IMPLEMENT_ABSTRACT_CONSTRUCTOR(name __VA_OPT__(,) __VA_ARGS__)

#define IMPLEMENT_PRIMITIVE_CONSTRUCTOR(name, ...) \
        Self CONCAT(CONCAT(Self, $), name)(__VA_ARGS__)



#define DECLARE_METHOD(return_type, class, class_type, method, ...) \
    _DECLARE_METHOD(class, method, return_type, (class_type this __VA_OPT__(,) __VA_ARGS__))


#define DECLARE_STATIC_METHOD(return_type, class, method, ...) \
    _DECLARE_STATIC_METHOD(class, method, return_type, (__VA_ARGS__))

#define DECLARE_SELF_STATIC_METHOD(return_type, method, ...) \
    DECLARE_STATIC_METHOD(return_type, Self, method __VA_OPT__(,) __VA_ARGS__)

#define DECLARE_CONSTRUCTOR(class, name, ...) \
    _DECLARE_CONSTRUCTOR_METHOD(class, name, Self, __VA_ARGS__)

#define DECLARE_SELF_CONSTRUCTOR(name, ...) \
    DECLARE_CONSTRUCTOR(Self, name __VA_OPT__(,) __VA_ARGS__)

#ifdef WITH_RTTI
#define IS_OBJECT_ASSIGNABLE(obj, class) (Object_isObjectTypeAssignable(obj, STR(class)))
#else
#define IS_OBJECT_ASSIGNABLE(obj, class) (1)
#endif

#define _IMPLEMENT_SELF_DOWNCAST(Parent) \
    Self CONCAT(CONCAT(Self, $$from), Parent)(Parent parent) { \
        Self this = {0};                 \
        if (!IS_OBJECT_ASSIGNABLE(UPCAST(parent, Object), Self)) {     \
            return DOWNCAST(null, Self);                             \
        }                                 \
        this.CONCAT(as, Parent) = parent;\
        return this;\
    }

#define IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_PARENTS) \
    ENUMERATE_PARENTS(_IMPLEMENT_SELF_DOWNCAST)

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION __FILE__ ":" S2(__LINE__)

#define THROW(e, ...) \
    *__exception__ = e.asThrowable;\
    Throwable_addStackFrame(*__exception__, StringRef$wrap(LOCATION));                  \
    return __VA_ARGS__;


#define RETHROW(...) \
    Throwable_addStackFrame(*__exception__, StringRef$wrap(LOCATION));                  \
    return __VA_ARGS__;


#define EXCEPTION __exception__
#define HAS_EXCEPTION (__exception__ != NULL && !Object_isNull((*__exception__).asObject))
