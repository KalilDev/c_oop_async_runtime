#include "Iterator.h"
#include "oop.h"
#include <assert.h>

#define Self Iterator
SUPER_CAST_IMPL(Iterator, Object)

ENUMERATE_ITERATOR_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

#undef Self