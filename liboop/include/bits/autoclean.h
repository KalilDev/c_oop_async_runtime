#define autoclean(type) __attribute__((cleanup(Object_delete_any_ptr))) type