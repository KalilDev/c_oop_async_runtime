#include "Object.h"
#include "FileReadStream.h"
#include "FileReadStream.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "string.h"
#include "StringBuffer.h"
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include "primitive/StringRef.h"
#include "foreach.h"
#include "GrowableList.h"
#include "Throwable.h"
#include "IOException.h"
#include "Function.h"
#include "primitive/Bool.h"
#include "StreamSubscription.h"
#include "UInt8List.h"
#include "ByteBuffer.h"
#include <fcntl.h>
#include <netinet/in.h>

#define Super() Object_vtable()
#define Self FileReadStream
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_FILE_READ_STREAM_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
}


#define CAPTURE_MYSELF(CAPTURE) \
    CAPTURE(FileReadStream, myself)

IMPLEMENT_LAMBDA(OnCancel, CAPTURE_MYSELF, NO_OWNED_CAPTURES, FileReadStream myself) {
    Lambda_OnCancel self = DOWNCAST(this, Lambda_OnCancel);
    StreamController controller = va_arg(args, StreamController);
    FileReadStream myself = self.data->myself;
    IOCoroutine coroutine = myself.data->coroutine;
    if (Object_isNull(coroutine.asObject)) {
        StreamController_close(controller);
        return null;
    }
    IOCoroutine_remove(coroutine);
    if (myself.data->fd < 0) {
        // we already closed the file
        return null;
    }
    close(myself.data->fd);
    myself.data->fd = -1;
    StreamController_close(controller);
    return null;
}

#define ENUMERATE_STEP_CAPTURES(CAPTURE) \
    CAPTURE(FileReadStream, myself)                \
    CAPTURE(StreamController, controller)

#define CHUNK 512

IMPLEMENT_LAMBDA(Step, ENUMERATE_STEP_CAPTURES, NO_OWNED_CAPTURES, FileReadStream myself, StreamController controller) {
    Lambda_Step step = DOWNCAST(this, Lambda_Step);
    FileReadStream myself = step.data->myself;
    StreamController controller = step.data->controller;
    Throwable error = DOWNCAST(null, Throwable);
    UInt8List data = DOWNCAST(null, UInt8List);
    THROWS = &error;
    {
        ByteBuffer buffer = ByteBuffer$make_new();
        ByteBuffer_ensure(buffer, CHUNK, EXCEPTION);
        if (HAS_EXCEPTION) {
            APPEND_STACK(error);
            goto thrown;
        }
        unsigned char chunk[CHUNK];
        ssize_t bytes = read(myself.data->fd, chunk, CHUNK);
        if (bytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // We are not blocking, so just loop again
                Object_delete(buffer.asObject);
                goto loop;
            }
            *EXCEPTION = IOException$make_fromErrno().asThrowable;
            APPEND_STACK(error);
            Object_delete(buffer.asObject);
            goto thrown;
        }
        if (bytes == 0) {
            // eof -> done
            Object_delete(buffer.asObject);
            goto done;
        }
        ByteBuffer_writeBuffer(buffer, chunk, bytes, EXCEPTION);
        if (HAS_EXCEPTION) {
            APPEND_STACK(error);
            Object_delete(buffer.asObject);
            goto thrown;
        }

        data = ByteBuffer_releaseToBytes(buffer);
        Object_delete(buffer.asObject);
    }


    data:

    StreamController_add(controller, data.asObject);

    goto loop;
    thrown:;
    Throwable _error = *EXCEPTION;
    *EXCEPTION = DOWNCAST(null, Throwable);
    StreamController_addError(controller, _error);
    loop:

    // Recurse again in the next microtask
    return True.asObject;
    done:

    // todo: exception
    close(myself.data->fd);
    myself.data->fd = -1;
    StreamController_close(controller);
    return False.asObject;
}

IMPLEMENT_LAMBDA(OnListen, CAPTURE_MYSELF, NO_OWNED_CAPTURES, FileReadStream myself) {
    Lambda_OnListen self = DOWNCAST(this, Lambda_OnListen);
    FileReadStream myself = self.data->myself;
    StreamController controller = va_arg(args, StreamController);
    StreamSubscription subs = va_arg(args, StreamSubscription);

    const char* path = String_cStringView(FileSystemEntity_path(myself.data->file.asFileSystemEntity));
    THROWS = CRASH_ON_EXCEPTION;

    int fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        THROW(IOException$make_fromErrno(), null)
    }
    myself.data->fd = fd;
    Function stepper = Lambda_Step$make_new(myself, controller).asFunction;
    IOCoroutine coro = IOCoroutine$make_new(stepper, fd, -1);
    myself.data->coroutine = coro;
    return null;
}

IMPLEMENT_OVERRIDE_METHOD(StreamSubscription, Stream, listen, Function onData, Function onError, Function onDone, Bool cancelOnError) {
    Self self = Stream_as_FileReadStream(this);
    return Stream_listen(StreamController_as_Stream(self.data->streamController), onData, onError, onDone, cancelOnError);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
1,
        "Stream", offsetof(FileReadStream_vtable_t , Stream_vtable)
        );
    // FileReadStream
    // Stream
    Stream_vtable_t *stream_vtable = &vtable->Stream_vtable;
    initImplementedInterfaceVtable(
            (Interface_vtable_t*)stream_vtable,
            (Interface_vtable_t*)Stream_vtable(),
            sizeof(*Stream_vtable()),
            offsetof(struct FileReadStream_vtable_t, Stream_vtable)
    );
    stream_vtable->listen = _FileReadStream_listen_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    //object_vtable->delete = _List_delete_impl;
}

OBJECT_CAST_IMPL(Stream, FileReadStream)
INTERFACE_CAST_IMPL(FileReadStream, Stream, Object)

IMPLEMENT_CONSTRUCTOR(new, File file) {
    this.data->file = file;
    this.data->fd = -1;
    this.data->coroutine = DOWNCAST(null, IOCoroutine);
    this.data->streamController = StreamController$make_new(
            Lambda_OnListen$make_new(this).asFunction,
            Lambda_OnCancel$make_new(this).asFunction
    );
}
#undef Super
#undef Self