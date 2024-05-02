#include "Iterator.h"
#include "oop.h"

SUPER_CAST_IMPL(Iterator, Object)

VIRTUAL_METHOD_IMPL(Iterator, moveNext, bool, (Iterator this), (this))
VIRTUAL_METHOD_IMPL(Iterator, current, Object, (Iterator this), (this))