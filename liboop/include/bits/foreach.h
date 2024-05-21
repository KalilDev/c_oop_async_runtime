#include "bits/Iterator.h"
#include "bits/Iterable.h"

#define foreach(type, var, iterable, block) \
    {                                \
        Iterator iterator = Iterable_iterator(iterable);                                 \
    while (Iterator_moveNext(iterator)) { \
        Object __ ## var = Iterator_current(iterator);                                   \
         type var = CONCAT(type, $$fromObject)(__ ## var);                                \
            {                               \
                block;                                \
            }\
        }                                   \
    Object_delete(iterator.asObject);                                        \
    }
