#include "Object.h"
#include "Socket.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "string.h"
#include "StringBuffer.h"
#include <strings.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "String.h"
#include "primitive/StringRef.h"
#include "foreach.h"
#include "GrowableList.h"
#include "Throwable.h"
#include "Future.h"
#include "IOException.h"
#include "Function.h"
#include "autoclean.h"
#include "primitive/Bool.h"
#include "StreamSubscription.h"
#include "UInt8List.h"

#define Super() Object_vtable()
#define Self Socket
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_SOCKET_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    List self = DOWNCAST(this, List);
    if (List_length(self) == 0) {
        Super()->delete(this);
        return;
    }
    // If there are objects, delete them
    foreach (Object, e, List_as_Iterable(self), {
        Object_delete(e);
    })

    // Delete the array
    Super()->delete(this);
}

#define CHUNK 2048
IMPLEMENT_SELF_METHOD(void, sendChunkSync, THROWS) {
    ByteBuffer queued = this.data->queued;
    size_t queuedCount = ByteBuffer_length(queued);
    unsigned char chunk[CHUNK];
    size_t toBeSentCount = ByteBuffer_consumeToBuffer(queued, chunk, CHUNK);

    ssize_t res = write(this.data->sockfd, chunk, toBeSentCount);
    if (res < 0) {
        THROW(IOException$make_fromErrno())
    }
}
IMPLEMENT_SELF_METHOD(void, sendOverqueuedSync, THROWS) {
    ByteBuffer queued = this.data->queued;
    size_t queuedCount = ByteBuffer_length(queued);
    if (queuedCount < CHUNK) {
        return;
    }
    unsigned char chunk[CHUNK];
    do {
        size_t toBeSentCount = ByteBuffer_consumeToBuffer(queued, chunk, CHUNK);

        ssize_t res = write(this.data->sockfd, chunk, toBeSentCount);
        if (res < 0) {
            THROW(IOException$make_fromErrno())
        }
    } while (queuedCount >= CHUNK);
}

IMPLEMENT_OVERRIDE_METHOD(Future, IOSink, flush) {
    Socket self = IOSink_as_Socket(this);
    _Socket_sendOverqueuedSync_impl(self, CRASH_ON_EXCEPTION);
    _Socket_sendChunkSync_impl(self, CRASH_ON_EXCEPTION);
    return Future$make_value(null);
}

IMPLEMENT_OVERRIDE_METHOD(void, Sink, close) {
    Socket self = IOSink_as_Socket(DOWNCAST(this, IOSink));
    _Socket_flush_impl(DOWNCAST(this, IOSink));
    THROWS = CRASH_ON_EXCEPTION;
    int res = close(self.data->sockfd);
    if (res < 0) {
        THROW(IOException$make_fromErrno())
    }
    self.data->sockfd = -1;
}

#define CAPTURE_MYSELF(CAPTURE) \
    CAPTURE(Socket, myself)

IMPLEMENT_LAMBDA(OnCancel, CAPTURE_MYSELF, NO_OWNED_CAPTURES, Socket myself) {
    Lambda_OnCancel self = DOWNCAST(this, Lambda_OnCancel);
    Socket myself = self.data->myself;
    Sink_close(Socket_as_IOSink(myself).asSink);
    return null;
}

#define ENUMERATE_LOOPER_CAPTURES(CAPTURE) \
    CAPTURE(Socket, myself)                \
    CAPTURE(StreamController, controller)


IMPLEMENT_LAMBDA(Looper, ENUMERATE_LOOPER_CAPTURES, NO_OWNED_CAPTURES, Socket myself, StreamController controller) {
    Lambda_Looper looper = DOWNCAST(this, Lambda_Looper);
    Socket myself = looper.data->myself;
    StreamController controller = looper.data->controller;
        Throwable error = DOWNCAST(null, Throwable);
    UInt8List data = DOWNCAST(null, UInt8List);
    THROWS = &error;
    if (myself.data->sockfd < 0 ) {
        goto done;
    }
    {
        ByteBuffer buffer = ByteBuffer$make_new();
        ByteBuffer_ensure(buffer, CHUNK, EXCEPTION);
        if (HAS_EXCEPTION) {
            APPEND_STACK(error);
            goto thrown;
        }
        unsigned char chunk[CHUNK];
        ssize_t bytes = read(myself.data->sockfd, chunk, CHUNK);
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
    return Future_computation(this).asObject;
    done:

    StreamController_close(controller);
    return null;
}

IMPLEMENT_LAMBDA(OnListen, CAPTURE_MYSELF, NO_OWNED_CAPTURES, Socket myself) {
    Lambda_OnCancel self = DOWNCAST(this, Lambda_OnCancel);
    Socket myself = self.data->myself;
    int sockfd = myself.data->sockfd;
    THROWS = CRASH_ON_EXCEPTION;
    StreamController controller = va_arg(args, StreamController);
    StreamSubscription subs = va_arg(args, StreamSubscription);

    bool blocking = false;
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        THROW(IOException$make_fromErrno(), null)
    }
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    if (fcntl(sockfd, F_SETFL, flags) != 0) {
        THROW(IOException$make_fromErrno(), null)
    }

    Function looper = Lambda_Looper$make_new(myself, controller).asFunction;
    Function_call(looper, 0);
    return null;
}

IMPLEMENT_OVERRIDE_METHOD(StreamSubscription, Stream, listen, Function onData, Function onError, Function onDone, Bool cancelOnError) {
    Self self = Stream_as_Socket(this);
    return Stream_listen(StreamController_as_Stream(self.data->streamController), onData, onError, onDone, cancelOnError);
}
IMPLEMENT_OVERRIDE_METHOD(void, Sink, add, Object value) {
    Socket self = IOSink_as_Socket(DOWNCAST(this, IOSink));
    if (IS_OBJECT_ASSIGNABLE(value, String)) {
        ByteBuffer_writeString(self.data->queued, DOWNCAST(value, String), CRASH_ON_EXCEPTION);
    } else if (IS_OBJECT_ASSIGNABLE(value, List)) {
        ByteBuffer_writeAll(self.data->queued, DOWNCAST(value, List), CRASH_ON_EXCEPTION);
    } else {
        CRASH();
    }
    _Socket_sendOverqueuedSync_impl(self, CRASH_ON_EXCEPTION);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
3,
        "Sink", offsetof(Socket_vtable_t , IOSink_vtable),
        "IOSink", offsetof(Socket_vtable_t , IOSink_vtable),
        "Stream", offsetof(Socket_vtable_t , Stream_vtable)
        );
    // Socket
    // Stream
    Stream_vtable_t *stream_vtable = &vtable->Stream_vtable;
    initImplementedInterfaceVtable(
            (Interface_vtable_t*)stream_vtable,
            (Interface_vtable_t*)Stream_vtable(),
            sizeof(*Stream_vtable()),
            offsetof(struct Socket_vtable_t, Stream_vtable)
    );
    stream_vtable->listen = _Socket_listen_impl;
    // IOSink
    IOSink_vtable_t *io_sink_vtable = &vtable->IOSink_vtable;
    initImplementedInterfaceVtable(
        (Interface_vtable_t*)io_sink_vtable,
        (Interface_vtable_t*)IOSink_vtable(),
        sizeof(*IOSink_vtable()),
        offsetof(struct Socket_vtable_t, IOSink_vtable)
    );
    io_sink_vtable->flush = _Socket_flush_impl;
    // Sink
    Sink_vtable_t *sink_vtable = (Sink_vtable_t*)io_sink_vtable;
    sink_vtable->add = _Socket_add_impl;
    sink_vtable->close = _Socket_close_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    //object_vtable->delete = _List_delete_impl;
}

OBJECT_CAST_IMPL(Stream, Socket)
INTERFACE_CAST_IMPL(Socket, Stream, Object)


OBJECT_CAST_IMPL(IOSink, Socket)
INTERFACE_CAST_IMPL(Socket, IOSink, Object)

IMPLEMENT_CONSTRUCTOR(new, int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    this.data->sockfd = sockfd;
    this.data->addr = malloc(addrlen);
    memcpy((struct sockaddr*)this.data->addr, addr, addrlen);
    this.data->addrlen = addrlen;
    this.data->queued = ByteBuffer$make_new();
    this.data->streamController = StreamController$make_new(
        Lambda_OnListen$make_new(this).asFunction,
        Lambda_OnCancel$make_new(this).asFunction
    );
}

IMPLEMENT_STATIC_METHOD(Socket, connectSync, int domain, const struct sockaddr* addr, socklen_t addrlen, THROWS) {
    int sockfd = socket(domain, SOCK_STREAM, 0);
    if (sockfd < 0) {
        THROW(IOException$make_fromErrno(), DOWNCAST(null, Socket))
    }
    int success = connect(sockfd, addr, addrlen);
    if (success < 0) {
        int e = errno;
        success = close(sockfd);
        if (success < 0) {
            // fuuuck
            e = errno;
        }
        THROW(IOException$make_new(e), DOWNCAST(null, Socket))
    }
    return Socket$make_new(sockfd, addr, addrlen);
}

#undef Super
#undef Self