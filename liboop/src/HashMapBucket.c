#include "liboop.h"
#include <assert.h>
#include <string.h>
#define Super() Object_vtable()
#define Self HashMapBucket
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_HASH_MAP_BUCKET_PARENTS)
ENUMERATE_HASH_MAP_BUCKET_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    Self self = DOWNCAST(this, Self);
    HashMapBucket_clear(self);
    Super()->delete(this);
}

IMPLEMENT_SELF_METHOD(bool, set, long keyHash, Object key, Object value, THROWS) {
    // Try to find
    HashMapBucket bucket = this;
    while (!Object_isNull(bucket.asObject)) {
        if (bucket.data->filled && bucket.data->keyHash == keyHash && Object_equals(bucket.data->key, key)) {
            bucket.data->value = value;
            Object_delete(bucket.data->key);
            bucket.data->key = key;
            return false;
        }
        bucket = HashMapBucket$$fromObject(this.data->next);
    }
    // Nope, didnt find. Insert it anywhere possible
    bucket = this;
    while (!Object_isNull(bucket.asObject)) {
        if (!bucket.data->filled) {
            bucket.data->value = value;
            bucket.data->key = key;
            bucket.data->keyHash = keyHash;
            bucket.data->filled = true;
            return true;
        }
        if (Object_isNull(this.data->next)) {
            HashMapBucket newBucket = HashMapBucket$make_empty();
            if (Object_isNull(newBucket.asObject)) {
                THROW(OutOfMemoryException_atUnknownLocation, false)
            }
            this.data->next = newBucket.asObject;
        }
        bucket = HashMapBucket$$fromObject(this.data->next);
    }
    THROW(Error$make_new(StringRef$wrap("impossible").asString), false);
}
IMPLEMENT_SELF_METHOD(Object, get, long keyHash, Object key) {
    HashMapBucket bucket = this;
    while (!Object_isNull(bucket.asObject)) {
        if (bucket.data->filled && bucket.data->keyHash == keyHash && Object_equals(bucket.data->key, key)) {
            return bucket.data->value;
        }
        bucket = HashMapBucket$$fromObject(this.data->next);
    }
    return null;
}
IMPLEMENT_SELF_METHOD(bool, hasKey, long keyHash, Object key) {
    HashMapBucket bucket = this;
    while (!Object_isNull(bucket.asObject)) {
        if (bucket.data->filled && bucket.data->keyHash == keyHash && Object_equals(bucket.data->key, key)) {
            return true;
        }
        bucket = HashMapBucket$$fromObject(this.data->next);
    }
    return false;
}
IMPLEMENT_SELF_METHOD(Object, remove, long keyHash, Object key, bool* removed) {
    HashMapBucket bucket = this;
    while (!Object_isNull(bucket.asObject)) {
        if (bucket.data->filled && bucket.data->keyHash == keyHash && Object_equals(bucket.data->key, key)) {
            Object_delete(bucket.data->key);
            Object value = bucket.data->value;
            bucket.data->value = null;
            bucket.data->filled = false;
            return value;
        }
        bucket = HashMapBucket$$fromObject(this.data->next);
    }
    return null;

}
IMPLEMENT_SELF_METHOD(void, release) {
    this.data->key = null;
    this.data->value = null;
    this.data->filled = false;
}

IMPLEMENT_SELF_METHOD(void, clear) {
    Object_delete(this.data->next);
    Object_delete(this.data->key);
    Object_delete(this.data->value);
    this.data->key = null;
    this.data->value = null;
    this.data->next = null;
    this.data->filled = false;
}


IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // HashMapBucket
    vtable->set = _HashMapBucket_set_impl;
    vtable->get = _HashMapBucket_get_impl;
    vtable->hasKey = _HashMapBucket_hasKey_impl;
    vtable->remove = _HashMapBucket_remove_impl;
    vtable->clear = _HashMapBucket_clear_impl;
    vtable->release = _HashMapBucket_release_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _HashMapBucket_delete_impl;
}

#undef Super
#undef Self