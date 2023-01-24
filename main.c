#include <stdio.h>
#include <string.h>

#include "arch.h"

int main() {

    FILE *target = fopen("output.decaf", "rw");

    FILE *source1 = fopen("1.txt", "r");
    FILE *source2 = fopen("2.txt", "r");

    if (!source1 || !source2) {
        printf("Files not opened successfully.\n");

        if (target)
            fclose(target);

        return 0;
    }

    arch_entry entry1;
    archEntry_fromFile(&entry1, "1.txt", source1);

    arch_entry entry2;
    archEntry_fromFile(&entry2, "2.txt", source2);

    arch_entry entries[] = {entry1, entry2};

    arch_file *file = archFile_createManual(target, 2, entries);

    archFile_write(file);

    fclose(source1);
    fclose(source2);

    archFile_dispose(file);

    // Read ....

    arch_file *read = archFile_create(target);

    _Bool err = archFile_parse(read);

    if (!err) {
        printf("An error occurred during parsing.\n");
        return 0;
    }

    archFile_list(read);

    fclose(target);

    return 0;
}
