#include "Object.h"
#include "Map.h"
#include "HashMap.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include <assert.h>
#include "String.h"
#include "Throwable.h"
#include "primitive/StringRef.h"
#include "HashMapBucket.h"
#include "foreach.h"
#define Super() List_vtable()
#define Self HashMap
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_HASH_MAP_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Map, set, Object key, Object value, THROWS) {
    HashMap self = DOWNCAST(this, HashMap);
    // whether or not we should grow
    if (self.data->length + 1 >= List_length(self.data->buckets) / 2) {
        size_t old_bucket_count = List_length(self.data->buckets);
        List oldBuckets = self.data->buckets;
        size_t new_bucket_count = old_bucket_count == 0 ? 8 : old_bucket_count << 1;
        List newBuckets = List_new();
        for (size_t i = 0; i < new_bucket_count; i++) {
            HashMapBucket bucket = HashMapBucket$make_empty();
            List_add(newBuckets, bucket.asObject, EXCEPTION);
            if (HAS_EXCEPTION) {
                RETHROW();
            }
        }
        self.data->length = 0;
        self.data->buckets = newBuckets;
        foreach (HashMapBucket, oldBucket, List_as_Iterable(oldBuckets), {
            while (!Object_isNull(oldBucket.asObject) && oldBucket.data->filled) {
                Map_set(this, oldBucket.data->key, oldBucket.data->value, EXCEPTION);
                if (HAS_EXCEPTION) {
                    RETHROW();
                }
                HashMapBucket_release(oldBucket);
            }
        })
        Object_delete(oldBuckets.asObject);
    }
    // okay, now we did the dirty work of rehashing
    List buckets = self.data->buckets;
    size_t bucket_count = List_length(buckets);
    long keyHash = Object_getHashCode(key);
    size_t i = keyHash % bucket_count;
    HashMapBucket bucket = HashMapBucket$$fromObject(List_at(buckets, i));
    bool inserted = HashMapBucket_set(bucket, keyHash, key, value, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW();
    }
    if (inserted) {
        self.data->length++;
    }
}
IMPLEMENT_OVERRIDE_METHOD(Object, Map, get, Object key)   {
    HashMap self = DOWNCAST(this, HashMap);
    List buckets = self.data->buckets;
    size_t bucket_count = List_length(buckets);
    long keyHash = Object_getHashCode(key);
    size_t i = keyHash % bucket_count;
    HashMapBucket bucket = HashMapBucket$$fromObject(List_at(buckets, i));
    return HashMapBucket_get(bucket, keyHash, key);
}
IMPLEMENT_OVERRIDE_METHOD(bool, Map, hasKey, Object key) {
    HashMap self = DOWNCAST(this, HashMap);
    List buckets = self.data->buckets;
    size_t bucket_count = List_length(buckets);
    long keyHash = Object_getHashCode(key);
    size_t i = keyHash % bucket_count;
    HashMapBucket bucket = HashMapBucket$$fromObject(List_at(buckets, i));
    return HashMapBucket_hasKey(bucket, keyHash, key);
}
IMPLEMENT_OVERRIDE_METHOD(Object, Map, remove, Object key, THROWS) {
    HashMap self = DOWNCAST(this, HashMap);
    List buckets = self.data->buckets;
    size_t bucket_count = List_length(buckets);
    long keyHash = Object_getHashCode(key);
    size_t i = keyHash % bucket_count;
    HashMapBucket bucket = HashMapBucket$$fromObject(List_at(buckets, i));
    bool removed = false;
    Object result = HashMapBucket_remove(bucket, keyHash, key, &removed);
    if (removed) {
        self.data->length--;
    }
    return result;
}

IMPLEMENT_OVERRIDE_METHOD(size_t, Map, length) {
    HashMap self = DOWNCAST(this, HashMap);
    return self.data->length;
}

IMPLEMENT_OVERRIDE_METHOD(void, Map, clear, THROWS) {
    HashMap self = DOWNCAST(this, HashMap);
    foreach(HashMapBucket, bucket, List_as_Iterable(self.data->buckets), {
        HashMapBucket_clear(bucket);
    })
    self.data->length = 0;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    HashMap self = DOWNCAST(this, HashMap);
    Object_delete(self.data->buckets.asObject);
    Super()->super.delete(this);
}


IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0);
    // HashMap
    // Map
    Map_vtable_t *map_vtable = (Map_vtable_t *)vtable;
    map_vtable->set = _HashMap_set_impl;
    map_vtable->get = _HashMap_get_impl;
    map_vtable->hasKey = _HashMap_hasKey_impl;
    map_vtable->remove = _HashMap_remove_impl;
    map_vtable->length = _HashMap_length_impl;
    map_vtable->clear = _HashMap_clear_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    object_vtable->delete = _HashMap_delete_impl;
}


IMPLEMENT_CONSTRUCTOR(new) {
    Map$new(this.asMap);
}

#undef Super
#undef Self