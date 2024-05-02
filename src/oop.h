#define CONCAT_(A, B) A ## B
#define CONCAT(A, B) CONCAT_(A, B)

#define SUPER_CAST_IMPL(class, Super) Super class ## _as_ ## Super(class this) { \
    Super super = {                                                               \
        .vtable = &this.vtable->super,                                 \
        .data = &this.data->super                                       \
    };                                                                            \
    return super;                                                                     \
}

#define SUPER_INTERFACE_CAST_IMPL(class, Super) Super class ## _as_ ## Super(class this) { \
    Super super = {                                                               \
        .vtable = &this.vtable->super,                                 \
        .data = this.data                                      \
    };                                                                            \
    return super;                                                                     \
}

#define SUPER_INTERFACE_CAST_IMPL_(class, Super, Class) Super class ## _as_ ## Super(class this) { \
    Super super = {                                                               \
        .vtable = &this.vtable->super,                                 \
        .data = (Class ## _data*)this.data                                      \
    };                                                                            \
    return super;                                                                     \
}



#define PRIMITIVE_SUPER_CAST_IMPL(class, Super) Super class ## _as_ ## Super(class this) { \
    Super super = {                                                               \
        .vtable = &this.vtable->super,                                 \
        .data = NULL                                       \
    };                                                                            \
    return super;                                                                     \
}
#define PRIMITIVE_UPCAST_IMPL(class, Other) Other class ## _as_ ## Other(class this) { \
    size_t reinterpreted_data = *((size_t*)&this.data);                                                                                   \
    Other other = {                                                               \
        .vtable = (Other ## _vtable_t*)this.vtable,                                 \
        .data = (Other ## _data*)reinterpreted_data                                       \
    };                                                                            \
    return other;                                                                     \
}
#define UPCAST_IMPL(class, Other) Other class ## _as_ ## Other(class this) { \
    Other other = {                                                               \
        .vtable = (Other ## _vtable_t*)this.vtable,                                 \
        .data = (Other ## _data*)this.data                                       \
    };                                                                            \
    return other;                                                                     \
}
#define VIRTUAL_METHOD_IMPL(class, method, return_type, signature, params) \
    return_type class ## _ ## method signature {           \
        return this.vtable->method params;                                                       \
    };
#define OVERRIDE_IMPL(return_type, class, method, super, ...) return_type _ ## class ## _ ## method(super this __VA_OPT__(,) __VA_ARGS__)

#define METHOD_IMPL(class, method, return_type, signature, params) \
    VIRTUAL_METHOD_IMPL(class, method, return_type, signature, params)             \
    return_type _ ## class ## _ ## method signature

#define DOWNCAST(var, class) *((class*) (&var))

#define OBJECT_CAST_IMPL(interface, class) class interface ## _as_ ## class(interface this) { \
    size_t vtable_offset = ((Interface_vtable_t*)this.vtable)->vtable_offset;                                                                                          \
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
#define _VIRTUAL_METHOD_INVOCATION(class, method, ...) this.vtable->method(this __VA_OPT__(,) __VA_OPT__(PARAMS_INVOCATION ## _ ## class ## _ ## method))
// ok
#define VIRTUAL_METHOD_IMPLEMENTATION(return_type, class, method, ...) \
return_type CONCAT(CONCAT(class, _), method)(class this __VA_OPT__(,) __VA_ARGS__) { \
    return _VIRTUAL_METHOD_INVOCATION(class, method __VA_OPT__(,) __VA_ARGS__);                                                                       \
}

// ok
#define IMPLEMENT_SELF_VIRTUAL_METHOD(return_type, method, ...) VIRTUAL_METHOD_IMPLEMENTATION(return_type, Self, method __VA_OPT__(,) __VA_ARGS__)
// ok
#define IMPLEMENT_STATIC_METHOD(return_type, static_method, ...) return_type CONCAT(CONCAT(Self, _), static_method)(__VA_ARGS__)
// ok
#define IMPLEMENT_METHOD(return_type, class, Self, method, ...) return_type CONCAT(_, CONCAT(CONCAT(CONCAT(class, _), method), _impl))(Self this __VA_OPT__(,) __VA_ARGS__)
// ok
#define IMPLEMENT_OVERRIDE_METHOD(return_type, class, method, ...) IMPLEMENT_METHOD(return_type, Self, class, method __VA_OPT__(,) __VA_ARGS__)
// ok
#define IMPLEMENT_SELF_METHOD(return_type, method, ...) IMPLEMENT_METHOD(return_type, Self, Self, method __VA_OPT__(,) __VA_ARGS__)

#define IMPLEMENT_OPERATOR_NEW() Self CONCAT(Self, _operator_new)() { \
    CONCAT(Self, _data)* data = (CONCAT(Self, _data)*)Object_allocate_memory(sizeof(CONCAT(Self, _data))); \
    if (data == NULL) {                                          \
        return DOWNCAST(Object_null, Self);                                      \
    }                                                             \
    Self this = {                                                \
        .vtable = CONCAT(Self, _vtable)(),                       \
        .data = data\
    };                                                           \
    return this; \
}

#define IMPLEMENT_ABSTRACT_CONSTRUCTOR(name, ...) void CONCAT(CONCAT(Self, $), name) (Self this __VA_OPT__(,) __VA_ARGS__)

#define IMPLEMENT_CONSTRUCTOR(name, ...) \
        Self CONCAT(CONCAT(CONCAT(Self, $), make_), name)(__VA_ARGS__) { \
            Self this = CONCAT(Self, _operator_new)();                         \
            if (Object_isNull(CONCAT(Self, _as_Object)(this))) {               \
                return this;\
            }                            \
            CONCAT(CONCAT(Self, $), name)(this __VA_OPT__(,) __VA_ARGS__);\
            return this;\
        } \
        IMPLEMENT_ABSTRACT_CONSTRUCTOR(name __VA_OPT__(,) __VA_ARGS__)


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
