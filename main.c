#include "Object.h"
#include "primitive/Integer.h"
#include <stdio.h>
#include "foreach.h"
#include "downcast.h"
#include "autoclean.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "primitive/Double.h"
#include "primitive/Bool.h"
#include "src/oop.h"
#include "EventLoop.h"
#include "Future.h"
#include "ServerSocket.h"
#include "Completer.h"
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define WITH_OOP_MAIN
#include "main.h"
#include "Socket.h"
#include "HttpRequest.h"
#include "HttpServer.h"
#include "UInt8List.h"
#include "StringBuffer.h"


#define ENUMERATE_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)

#define Ln(line) StringBuffer_writeLn(_builder, StringRef$wrap(line).asObject);
#define BUILD_RESPONSE autoclean(StringBuffer) _builder = StringBuffer$make_new();
#define END_RESPONSE \
    String content = StringBuffer_releaseToString(_builder); \
    Integer contentLength = Integer$box_l((long)String_length(content));

Object notFound(HttpRequest request, HttpResponse response) {
    HttpResponse_setStatusCode(response, Integer$box_l(404));
    HttpResponse_setStatusReason(response, StringRef$wrap("Not found").asString);
    BUILD_RESPONSE
    Ln("Not found");
    END_RESPONSE
    HttpHeaders_add(HttpResponse_headers(response), StringRef$wrap("Content-length").asString,  Object_toString(contentLength.asObject));
    HttpHeaders_add(HttpResponse_headers(response), StringRef$wrap("Content-type").asString, StringRef$wrap("text/plain").asString);
    HttpResponse_send(response);


    Sink_add(HttpResponse_as_Sink(response), content.asObject);
    // TODO: onComplete
    /*Future onComplete = */Sink_close(HttpResponse_as_Sink(response));


    Object_delete(response.asObject);
    Object_delete(request.asObject);

    return null;
}
Object getStyle(HttpRequest request, HttpResponse response) {
    BUILD_RESPONSE
    Ln("* {");
    Ln("}");
    END_RESPONSE
    HttpHeaders_add(HttpResponse_headers(response), StringRef$wrap("Content-length").asString,  Object_toString(contentLength.asObject));
    HttpHeaders_add(HttpResponse_headers(response), StringRef$wrap("Content-type").asString, StringRef$wrap("text/css").asString);
    HttpResponse_send(response);


    Sink_add(HttpResponse_as_Sink(response), content.asObject);
    // TODO: onComplete
    /*Future onComplete = */Sink_close(HttpResponse_as_Sink(response));


    Object_delete(response.asObject);
    Object_delete(request.asObject);

    return null;
}

Object getIndex(HttpRequest request, HttpResponse response) {
    BUILD_RESPONSE
    Ln("<!DOCTYPE html>");
    Ln("<html lang=\"en\">");
    Ln("  <head>");
    Ln("    <meta charset=\"ASCII\">");
    Ln("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    Ln("    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">")
    Ln("    <title>Hello</title>");
    Ln("    <link rel=\"stylesheet\" href=\"style.css\">");
    Ln("  </head>");
    Ln("  <body>");
    Ln("    <h1>Hello</h1>");
    Ln("  </body>");
    Ln("</html>");
    END_RESPONSE
    HttpHeaders_add(HttpResponse_headers(response), StringRef$wrap("Content-length").asString,  Object_toString(contentLength.asObject));
    HttpHeaders_add(HttpResponse_headers(response), StringRef$wrap("Content-type").asString, StringRef$wrap("text/html").asString);
    HttpResponse_send(response);


    Sink_add(HttpResponse_as_Sink(response), content.asObject);
    // TODO: onComplete
    /*Future onComplete = */Sink_close(HttpResponse_as_Sink(response));


    Object_delete(response.asObject);
    Object_delete(request.asObject);

    return null;
}
#define MATCHES_ROUTE(method_, uri_) (Object_equals(uri.asObject, StringRef$wrap(uri_).asObject) && Object_equals(method.asObject, StringRef$wrap(method_).asObject))
Object route(HttpRequest request, HttpResponse response) {
    String uri = HttpRequest_uri(request);
    String method = HttpRequest_method(request);
    if (MATCHES_ROUTE("GET", "/index.html") || MATCHES_ROUTE("GET", "/")) {
        return getIndex(request, response);
    }
    if (MATCHES_ROUTE("GET", "/style.css")) {
        return getStyle(request, response);
    }
    return notFound(request, response);
}


IMPLEMENT_LAMBDA(OnData, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    HttpRequest request = va_arg(args, HttpRequest);
    THROWS = va_arg(args, Throwable*);
    fprintf(stdout, "Request!\n");
    fprintf(stdout, "Method: %s\n", String_cStringView(HttpRequest_method(request)));
    fprintf(stdout, "Uri: %s\n", String_cStringView(HttpRequest_uri(request)));
    fprintf(stdout, "Http: %s\n", String_cStringView(HttpRequest_http(request)));



    HttpResponse response = HttpRequest_response(request);
    Sink requestSink = HttpResponse_as_Sink(response);

    HttpResponse_setHttp(response, StringRef$wrap("HTTP/2.0").asString);
    HttpResponse_setStatusCode(response, Integer$box_l(200));
    HttpResponse_setStatusReason(response, StringRef$wrap("Okay!!!").asString);

    HttpHeaders responseHeaders = HttpResponse_headers(response);
    HttpHeaders_add(responseHeaders, StringRef$wrap("Date").asString, StringRef$wrap("Sun, 01 Oct 2000 23:25:17 GMT").asString);
    HttpHeaders_add(responseHeaders, StringRef$wrap("Server").asString, StringRef$wrap("Poor man's dart HttpServer").asString);
    HttpHeaders_add(responseHeaders, StringRef$wrap("Last-modified").asString, StringRef$wrap("Tue, 04 Jul 2000 09:46:21 GMT").asString);


    route(request, response);

    return null;
}
IMPLEMENT_LAMBDA(OnError, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    Completer completer = DOWNCAST(this, Lambda_OnError).data->completer;
    Throwable error = va_arg(args, Throwable);
    THROWS = va_arg(args, Throwable*);
    autoclean(String) errorStr = Object_toString(error.asObject);
    fprintf(stderr, "error arrived %s\n", String_cStringView(errorStr));
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
    HttpServer server = HttpServer_bindSync(
            StringRef$wrap("0.0.0.0").asString,
            3000,
            EXCEPTION
    );
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    Completer completer = Completer$make_new();
    Stream_listen(
            HttpServer_as_Stream(server),
            Lambda_OnData$make_new(completer).asFunction,
            Lambda_OnError$make_new(completer).asFunction,
            Lambda_OnDone$make_new(completer).asFunction,
            Bool$box(true)
    ).asObject;
    printf("Listening on 127.0.0.1:3000...\n");
    return null;
}