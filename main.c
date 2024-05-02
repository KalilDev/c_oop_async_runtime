#include "Object.h"
#include "List.h"
#include "primitive/Integer.h"
#include <stdio.h>
#include "foreach.h"
#include "downcast.h"
#include "autoclean.h"

int main() {
    autoclean(List) list = List_new();
    if (Object_isNull(List_as_Object(list))) {
        fprintf(stderr, "Could not allocate the list\n");
        return -1;
    }
    List_add(list, Integer_as_Object(Integer_box(1)));
    List_add(list, Integer_as_Object(Integer_box(2)));
    List_add(list, Integer_as_Object(Integer_box(3)));
    List_add(list, Integer_as_Object(Integer_box(8)));
    printf("[");
    foreach(Integer, current, List_as_Iterable(list), {
            printf("%i,", Integer_unbox_i(current));
    })
    printf("]\n");
}
