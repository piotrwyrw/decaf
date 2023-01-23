#include <stdio.h>
#include <string.h>

#include "arch.h"

int main() {

    FILE *target = fopen("output.decaf", "w");

    FILE *source = fopen("example.txt", "r");

    arch_entry entry;
    archEntry_fromFile(&entry, "example.txt", source);

    arch_entry entries[] = {entry};

    arch_file *file = archFile_createManual(target, 1, entries);

    archFile_write(file);

    fclose(target);
    fclose(source);

    archFile_dispose(file);

    return 0;
}
