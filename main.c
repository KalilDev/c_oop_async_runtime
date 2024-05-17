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
#include "Thread.h"
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


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
    CAPTURE(ServerSocket, server)

IMPLEMENT_LAMBDA(JoinThreadAsync, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer, ServerSocket server) {
    ServerSocket server = DOWNCAST(this, Lambda_JoinThreadAsync).data->server;
    THROWS = va_arg(args, Throwable*);
    if (Object_isNull(server.data->subs.asObject)) {
        Completer completer = DOWNCAST(this, Lambda_JoinThreadAsync).data->completer;
        return Completer_future(completer).asObject;
    }
    thrd_sleep(&(struct timespec){.tv_nsec=16*1000000}, NULL); // sleep 16 msec
    thrd_yield();
    return Future_computation(this).asObject;
}
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
    ServerSocket server = ServerSocket_bindSync(
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
            ServerSocket_as_Stream(server),
            Lambda_OnData$make_new(completer).asFunction,
            Lambda_OnError$make_new(completer).asFunction,
            Lambda_OnDone$make_new(completer).asFunction,
            Bool$box(true)
    ).asObject;
    printf("Listening on 127.0.0.1:3000...\n");
    return Future_computation(Lambda_JoinThreadAsync$make_new(completer, server).asFunction).asObject;
}

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
    Object_delete(arguments.asObject);
    return res;
}

int main(int argc, char **argv) {
    Thread mainThread = Thread$make_main(Lambda_InvokeMainWithArguments$make_new(argc, argv).asFunction);
    int res = Thread_runInCurrentThread(mainThread);
    Object_delete(mainThread.asObject);
    return res;
}
