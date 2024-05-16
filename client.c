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
#include "ServerSocket.h"
#include "Completer.h"
#include "Socket.h"
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define ENUMERATE_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)

IMPLEMENT_LAMBDA(OnData, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    Object data = va_arg(args, Object);
    THROWS = va_arg(args, Throwable*);
    autoclean(String) str = Object_toString(data);
    printf("data arrived: %s\n", String_cStringView(str));
    return null;
}
IMPLEMENT_LAMBDA(OnError, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    Completer completer = DOWNCAST(this, Lambda_OnError).data->completer;
    Throwable error = va_arg(args, Throwable);
    THROWS = va_arg(args, Throwable*);
    fprintf(stderr, "error arrived\n");
    Completer_completeException(completer, error);
    return null;
}
IMPLEMENT_LAMBDA(OnDone, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    Completer completer = DOWNCAST(this, Lambda_OnDone).data->completer;
    THROWS = va_arg(args, Throwable*);
    printf("done arrived\n");
    Completer_complete(completer, Integer$box(0).asObject);
    return null;
}


#define ENUMERATE_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer) \
    CAPTURE(Socket, socket)

IMPLEMENT_STATIC_FUNCTION(MainWithRandomAccessFile) {
    RandomAccessFile raf = va_arg(args, RandomAccessFile);
    THROWS = va_arg(args, Throwable*);
    printf("\nOokay, im gonna execute the async stuff now that the file is open\n");
    autoclean(String) str = RandomAccessFile_readString(raf, 100, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    printf("\nsome stuff i read '%s'\n", String_cStringView(str));
    return Integer$box(10).asObject;
}

Object Main(List arguments, THROWS) {
//    File file = File$make_new(StringRef$wrap("CMakeCache.txt").asString);
//    Future f = File_open(file, FileMode$read);
//    printf("\nScheduled to open an file asynchronously\n");
//    Future res =  Future_then(f, StaticFunction_MainWithRandomAccessFile$make_new().asFunction);
//    printf("\nEven tho there will be more async operations, i am not gonna execute them right now, thus, i can perform more stuff now!\n");
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 3001;
    addr.sin_family = AF_INET;
    Socket socket = Socket_connectSync(
    AF_INET,
    (const struct sockaddr*)&addr,
    sizeof(addr),
    EXCEPTION
    );
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    Completer completer = Completer$make_new();
    Stream_listen(
            Socket_as_Stream(socket),
            Lambda_OnData$make_new(completer).asFunction,
            Lambda_OnError$make_new(completer).asFunction,
            Lambda_OnDone$make_new(completer).asFunction,
            Bool$box(true)
    ).asObject;
    printf("Connected to 127.0.0.1:3000...\n");
    return Completer_future(completer).asObject;
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
