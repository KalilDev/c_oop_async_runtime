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
#include <libgen.h>

#define WITH_OOP_MAIN
#include "main.h"
#include "Socket.h"
#include "HttpRequest.h"
#include "HttpServer.h"
#include "UInt8List.h"
#include "StringBuffer.h"
#include "File.h"
#include "Directory.h"


#define ENUMERATE_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)

#define Ln(line) StringBuffer_writeLn(_builder, StringRef$wrap(line).asObject);
#define Cstr(string) StringBuffer_write(_builder, StringRef$wrap(string).asObject);
#define Obj(string) StringBuffer_write(_builder, string.asObject);
#define BUILD_RESPONSE autoclean(StringBuffer) _builder = StringBuffer$make_new();
#define END_RESPONSE \
    String content = StringBuffer_releaseToString(_builder); \
    Integer contentLength = Integer$box_l((long)String_length(content));

Bool notFound(HttpRequest request, HttpResponse response) {
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

    return True;
}
Bool getStyle(HttpRequest request, HttpResponse response) {
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

    return True;
}

Bool getIndex(HttpRequest request, HttpResponse response) {
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

    return True;
}

#define CAPTURE_REQUEST_AND_RESPONSE(CAPTURE) \
    CAPTURE(HttpRequest, request)             \
    CAPTURE(HttpResponse, response)

IMPLEMENT_LAMBDA(OnFinishSendingSuccess, CAPTURE_REQUEST_AND_RESPONSE, NO_OWNED_CAPTURES, HttpRequest request, HttpResponse response) {
    Lambda_OnFinishSendingSuccess self = DOWNCAST(this, Lambda_OnFinishSendingSuccess);
    HttpRequest request = self.data->request;
    HttpResponse response = self.data->response;

    fprintf(stdout, "Finished sending\n");
    // TODO: onComplete
    /*Future onComplete = */Sink_close(HttpResponse_as_Sink(response));

    Object_delete(response.asObject);
    Object_delete(request.asObject);
}

IMPLEMENT_LAMBDA(OnFinishSendingError, CAPTURE_REQUEST_AND_RESPONSE, NO_OWNED_CAPTURES, HttpRequest request, HttpResponse response) {
    Lambda_OnFinishSendingSuccess self = DOWNCAST(this, Lambda_OnFinishSendingSuccess);
    HttpRequest request = self.data->request;
    HttpResponse response = self.data->response;
    Throwable error = va_arg(args, Throwable);
    autoclean(String) message = Object_toString(error.asObject);

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

    // TODO: onComplete
    /*Future onComplete = */Sink_close(HttpResponse_as_Sink(response));

    Object_delete(response.asObject);
    Object_delete(request.asObject);
}

Bool _directoryIndexRoute(HttpRequest request, HttpResponse response, Directory directory) {
    String uri = HttpRequest_uri(request);
    List children = Directory_list(directory, CRASH_ON_EXCEPTION);

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
    Cstr("    <h1>Directory \"");
    Obj(uri);
    Ln("\"</h1>");
    Ln("    <ul>");
    foreach (FileSystemEntity, fse, List_as_Iterable(children), {
        autoclean (String) name = FileSystemEntity_name(fse);
        Ln("    <li>");
        Cstr("      <a href=\"");
        Obj(name)
        Cstr("\">");
        Obj(name)
        Ln("</a>")
        Ln("    </li>");
    })
    Ln("    </ul>");
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

    return True;
}
Bool currentDirectoryIndexRoute(HttpRequest request, HttpResponse response) {
    Directory directory = Directory_current();
    return _directoryIndexRoute(request, response, directory);
}
Bool directoryIndexRoute(HttpRequest request, HttpResponse response) {
    String uri = HttpRequest_uri(request);
    String path = StringRef$wrap(String_cStringView(uri) + 1).asString;
    if (!Directory_isDirectorySync(path)) {
        return False;
    }
    Directory directory = Directory$make_new(path);
    if (!Directory_existsSync(directory)) {
        return False;
    }
    return _directoryIndexRoute(request, response, directory);
}

Bool fileGetRoute(HttpRequest request, HttpResponse response) {
    String uri = HttpRequest_uri(request);
    String path = StringRef$wrap(String_cStringView(uri) + 1).asString;
    if (!File_isFileSync(path)) {
        return False;
    }
    File file = File$make_new(path);
    if (!File_existsSync(file, CRASH_ON_EXCEPTION)) {
        return False;
    }
    size_t length = File_lengthSync(file, CRASH_ON_EXCEPTION);
    Integer contentLength = Integer$box_l((long)length);

    HttpHeaders_add(HttpResponse_headers(response), StringRef$wrap("Content-length").asString,  Object_toString(contentLength.asObject));
    HttpHeaders_add(HttpResponse_headers(response), StringRef$wrap("Content-type").asString, StringRef$wrap("text/plain").asString);
    HttpResponse_send(response);

    Future onFinishSendingFile = IOSink_addStream(
        HttpResponse_as_IOSink(response),
        File_openRead(file)
    );
    onFinishSendingFile = Future_then(
        onFinishSendingFile,
        Lambda_OnFinishSendingSuccess$make_new(request, response).asFunction
    );
    onFinishSendingFile = Future_catch(
        onFinishSendingFile,
        Lambda_OnFinishSendingError$make_new(request, response).asFunction
    );
    return True;
}

#define MATCHES_METHOD(method_) Object_equals(method.asObject, StringRef$wrap(method_).asObject)
#define MATCHES_URI(uri_) Object_equals(uri.asObject, StringRef$wrap(uri_).asObject)
#define MATCHES_ROUTE(method_, uri_) (MATCHES_METHOD(method_) && MATCHES_URI(uri_))
Object route(HttpRequest request, HttpResponse response) {
    String uri = HttpRequest_uri(request);
    String method = HttpRequest_method(request);
//    if (MATCHES_ROUTE("GET", "/index.html") || MATCHES_ROUTE("GET", "/")) {
//        return getIndex(request, response).asObject;
//    }
//    if (MATCHES_ROUTE("GET", "/style.css")) {
//        return getStyle(request, response).asObject;
//    }
    if (MATCHES_ROUTE("GET", "/")) {
        return currentDirectoryIndexRoute(request, response).asObject;
    }
    if (MATCHES_METHOD("GET") && String_length(uri) > 1) {
        bool handled = fileGetRoute(request, response).unwrap;
        if (handled) {
            return True.asObject;
        }
        handled = directoryIndexRoute(request, response).unwrap;
        if (handled) {
            return True.asObject;
        }
    }
    return notFound(request, response).asObject;
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
    fflush(stderr);
    Completer_completeException(completer, error);
    return null;
}
IMPLEMENT_LAMBDA(OnDone, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    Completer completer = DOWNCAST(this, Lambda_OnDone).data->completer;
    THROWS = va_arg(args, Throwable*);
    printf("done arrived\n");
    fflush(stdout);
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