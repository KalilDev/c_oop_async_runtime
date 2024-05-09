#include "Object.h"
#include "List.h"
#include "primitive/Integer.h"
#include <stdio.h>
#include "foreach.h"
#include "downcast.h"
#include "autoclean.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "primitive/Double.h"
#include "File.h"
#include "primitive/Bool.h"
#include "Directory.h"

int main() {
    autoclean(List) list = List_new();
    if (Object_isNull(List_as_Object(list))) {
        fprintf(stderr, "Could not allocate the list\n");
        return -1;
    }

    List_add(list, Integer_as_Object(Integer$box(1)));
    List_add(list, Integer_as_Object(Integer$box(2)));
    List_add(list, Integer_as_Object(Integer$box(3)));
    List_add(list, Integer_as_Object(Integer$box(8)));


    printf("[");
    foreach(Integer, current, List_as_Iterable(list), {
        printf("%i,", Integer_unbox_i(current));
    })
    printf("]\n");

    autoclean(String) s = String_format_c("Object 1: {} 2: {}\n", null, Double$box(2.0));
    printf("%s\n", String_cStringView(s));

    autoclean(File) f = File$make_new(StringRef_as_String(StringRef$wrap("../main.c")));

    autoclean(Directory) cwd = Directory_current();
    FileSystemEntity cwdAbs_e = FileSystemEntity_absolute(Directory_as_FileSystemEntity(cwd));
    autoclean(Directory) cwdAbs = DOWNCAST(cwdAbs_e, Directory);
    autoclean(Directory) project = Directory_parent(cwd);
    autoclean(List) projectFiles = Directory_list(project);
    autoclean(String) s1 = String_format_c("Cwd: {}, CwdAbs: {}, Project: {}, main: {}, p: {}\n", cwd, cwdAbs, project, f, projectFiles);
    printf("%s\n", String_cStringView(s1));
    autoclean(String) mainString = File_readStringSync(f);
    printf("%s\n", String_cStringView(mainString));

}
