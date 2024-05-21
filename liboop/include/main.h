#ifdef WITH_OOP_MAIN
#include "oop.h"
#include "Object.h"
#include "List.h"
#include "Throwable.h"

Object Main(List arguments, THROWS);







#ifndef _OOP_MAIN_IMPLEMENTATION
#define _OOP_MAIN_IMPLEMENTATION
#include "Thread.h"
#include "Throwable.h"
#include "oop_impl.h"
#include "primitive/StringRef.h"
#include "primitive/OutOfMemoryException.h"
#include <assert.h>

#define ENUMERATE_IMWA_CAPTURES(CAPTURE) \
    CAPTURE(int, argc)                       \
    CAPTURE(char**, argv)

IMPLEMENT_LAMBDA(InvokeMainWithArguments, ENUMERATE_IMWA_CAPTURES, NO_OWNED_CAPTURES, int argc, char**argv) {
    Lambda_InvokeMainWithArguments self = DOWNCAST(this, Lambda_InvokeMainWithArguments);
    THROWS = va_arg(args, Throwable*);
    argc = self.data->argc;
    char **argv = self.data->argv;
    List arguments = List_new();
    if (Object_isNull(arguments.asObject)) {
        THROW(OutOfMemoryException$at(LOCATION), null);
    }
    List_setLength(arguments, argc - 1 < 0 ? 0 : argc - 1, EXCEPTION);
    if (HAS_EXCEPTION) {
        Object_delete(arguments.asObject);
        RETHROW(null)
    }

    for (size_t i = 1; i < argc; i++) {
        List_setAt(arguments, i - 1, StringRef$wrap(argv[i]).asObject);
        if (HAS_EXCEPTION) {
            Object_delete(arguments.asObject);
            RETHROW(null)
        }
    }
    Object res = Main(arguments, EXCEPTION);
    if (HAS_EXCEPTION) {
        Object_delete(arguments.asObject);
        RETHROW(res)
    }
    Object_delete(arguments.asObject);
    return res;
}

#undef ENUMERATE_IMWA_CAPTURES

IMPLEMENT_STATIC_FUNCTION(PrintStack) {
    Throwable error = va_arg(args, Throwable);
    APPEND_STACK(error);
    THROWS = va_arg(args, Throwable*);

    String errorMessage = Object_toString(error.asObject);
    const char* error_message;
    if (Object_isNull(errorMessage.asObject)) {
        error_message = "Unknown error";
    } else {
        error_message = String_cStringView(errorMessage);
    }
    fprintf(stderr, "An error ocurred: %s\nStack trace:\n", error_message);
    Throwable_printStackTrace(error, stderr);
    Object_delete(errorMessage.asObject);
    Object_delete(error.asObject);
    return null;
}

int main(int argc, char **argv) {
    Thread mainThread = Thread$make_main(Lambda_InvokeMainWithArguments$make_new(argc, argv).asFunction);
    Future result = Thread_result(mainThread);
    Future catching = Future_catch(result, StaticFunction_PrintStack$make_new().asFunction);
    int res = Thread_runInCurrentThread(mainThread);
    // needed for the exceptions. instead we could write a new invoke for main but meh for now
    EventLoop_drain(EventLoop_current());
    Object_delete(catching.asObject);
    Object_delete(result.asObject);
    Object_delete(mainThread.asObject);
    return res;
}
#endif // _OOP_MAIN_IMPLEMENTATION
#endif // WITH_OOP_MAIN