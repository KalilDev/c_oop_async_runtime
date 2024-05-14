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

Integer Main(List arguments, THROWS) {
    autoclean(File) file = File$make_new(StringRef$wrap("masaadsa.c").asString);
    RandomAccessFile f = File_openSync(file, FileMode$read,EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(DOWNCAST(null, Integer))
    }
    RandomAccessFile_writeByte(f, 1, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(DOWNCAST(null, Integer))
    }
    return DOWNCAST(null, Integer);
}

int main(int argc, char **argv) {
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

    Integer res = Main(arguments, &EXCEPTION);
    if (!Object_isNull(EXCEPTION.asObject)) {
        goto threw_with_list;
    }

    Object_delete(arguments.asObject);
    if (Object_isNull(res.asObject)) {
        return 0;
    }

    return (int)res.unwrap;

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
