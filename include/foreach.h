#include "Iterator.h"
#include "Iterable.h"

#define foreach(type, var, iterable, block) \
    {                                \
        Iterator iterator = Iterable_iterator(iterable);                                 \
    while (Iterator_moveNext(iterator)) { \
        Object __ ## var = Iterator_current(iterator);                                   \
         type var = DOWNCAST(__ ## var, type);                                \
            {                               \
                block;                                \
            }\
        }                                   \
    Object_delete(Iterator_as_Object(iterator));                                        \
    }
