#include <stdio.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <liboop.h>

#define WITH_OOP_MAIN
#include <main.h>


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

Object Main(List arguments, THROWS) {
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(3000);
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