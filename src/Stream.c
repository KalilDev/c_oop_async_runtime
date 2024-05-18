#include "Stream.h"
#include "StreamSubscription.h"
#include "Function.h"
#include "primitive/Bool.h"
#include "oop.h"
#include <assert.h>


#define Self Stream

SUPER_INTERFACE_CAST_IMPL(Stream, Interface)

OBJECT_CAST_IMPL(Stream, Object)

StreamSubscription CONCAT(CONCAT(Stream, _), listen)(Stream this, Function onData, Function onError, Function onDone,
                                                    Bool cancelOnError) {
    assert(this.vtable != NULL);
    assert(!any_isNull(UPCAST(this, any)));
    assert(this.vtable->listen != NULL);
    return _VIRTUAL_METHOD_INVOCATION(Stream, listen, Function onData, Function onError, Function onDone,
                                      Bool cancelOnError);
}

IMPLEMENT_SELF_INTERFACE_VTABLE() {
    initInterfaceVtable(
            (Interface_vtable_t *) vtable,
            Interface_vtable(),
            sizeof(*Interface_vtable())
    );
}

#undef Self