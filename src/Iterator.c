#include "Iterator.h"
#include "oop.h"
#include <assert.h>

#define Self Iterator

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_ITERATOR_PARENTS)

ENUMERATE_ITERATOR_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

#undef Self