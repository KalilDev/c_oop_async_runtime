#include "Object.h"
#include "primitive/Integer.h"
#include "primitive/OutOfMemoryException.h"
#include <stdio.h>
#include "foreach.h"
#include "downcast.h"
#include "autoclean.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "primitive/Double.h"
#include "File.h"
#include "RandomAccessFile.h"
#include "primitive/Bool.h"
#include "Directory.h"
#include "src/oop.h"
#include "EventLoop.h"
#include "Future.h"
#include "Error.h"
#include "TypeError.h"
#include <assert.h>

IMPLEMENT_STATIC_FUNCTION(MainWithRAF) {
    RandomAccessFile raf = va_arg(args, RandomAccessFile);
    THROWS = va_arg(args, Throwable*);
    autoclean(String) str = RandomAccessFile_readString(raf, 100, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    printf("%s\n", String_cStringView(str));
    return Integer$box(10).asObject;
}

Object Main(List arguments, THROWS) {
    File file = File$make_new(StringRef$wrap("CMakeCache.txt").asString);
    Future f = File_open(file, FileMode$read);
    return Future_then(f, StaticFunction_MainWithRAF$make_new().asFunction).asObject;
}

int main(int argc, char **argv) {
    EventLoop loop = EventLoop_instance();
    Throwable EXCEPTION = DOWNCAST(null, Throwable);
    List arguments = List_new();
    if (Object_isNull(arguments.asObject)) {
        EXCEPTION = OutOfMemoryException$at(LOCATION).asThrowable;
        goto threw_without_list;
    }

    List_setLength(arguments, argc - 1 < 0 ? 0 : argc - 1, &EXCEPTION);
    if (!Object_isNull(EXCEPTION.asObject)) {
        goto threw_with_list;
    }

    for (size_t i = 1; i < argc; i++) {
        List_setAt(arguments, i - 1, StringRef$wrap(argv[i]).asObject);
        if (!Object_isNull(EXCEPTION.asObject)) {
            goto threw_with_list;
        }
    }

    Object res = Main(arguments, &EXCEPTION);
    if (!Object_isNull(EXCEPTION.asObject)) {
        goto threw_with_list;
    }
    Object_delete(arguments.asObject);

    EventLoop_drain(loop);

    if (Object_isNull(res)) {
        return 0;
    }

    if (IS_OBJECT_ASSIGNABLE(res, Future)) {
        Future futureRes = Future$$fromObject(res);
        switch (futureRes.data->state) {
            case FutureState$pending: {
                EXCEPTION = Error$make_new(StringRef$wrap("Unexpected state, this means the event loop was not drained!").asString).asThrowable;
                APPEND_STACK(EXCEPTION)
                goto threw_without_list;
            }
            case FutureState$complete: {
                res = futureRes.data->value;
                break;
            }
            case FutureState$completedWithError: {
                EXCEPTION = futureRes.data->exception;
                APPEND_STACK(EXCEPTION)
                goto threw_without_list;
            }
        }
    }

    if (!IS_OBJECT_ASSIGNABLE(res, Integer)) {
        EXCEPTION = TypeError$make_new(res, StringRef$wrap("Integer").asString).asThrowable;
        APPEND_STACK(EXCEPTION)
        goto threw_without_list;
    }

    Integer resInt = Integer$$fromObject(res);

    return (int)resInt.unwrap;

    threw_with_list:;
    Object_delete(arguments.asObject);
    threw_without_list:;
    String errorMessage = Object_toString(EXCEPTION.asObject);
    const char* error_message;
    if (Object_isNull(errorMessage.asObject)) {
        error_message = "Unknown error";
    } else {
        error_message = String_cStringView(errorMessage);
    }
    fprintf(stderr, "An error ocurred: %s\nStack trace:\n", error_message);
    Throwable_printStackTrace(EXCEPTION, stderr);
    Object_delete(errorMessage.asObject);
    return -1;
}
