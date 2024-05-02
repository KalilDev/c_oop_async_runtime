#pragma once
#include "oop.h"
#include <stdlib.h>
#include "Object.h"
#include "Iterator.h"
#include "List.h"

struct ListIterator;

DECLARE_CLASS(ListIterator, Iterator, NO_METHODS, {
    List list;
    long long i;
})


const ListIterator_vtable_t* ListIterator_vtable();

DECLARE_SUPER_CAST(ListIterator, Iterator)
DECLARE_UPCAST(ListIterator, Object)
ListIterator ListIterator_new(List list);