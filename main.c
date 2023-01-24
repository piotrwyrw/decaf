#include <stdio.h>
#include <string.h>

#include "arch.h"

int package(int argc, char **argv) {
    arch_entry *table = NULL;
    arch_file *file = NULL;
    unsigned n = argc - 3;

    FILE *ftarget = fopen(argv[2], "w");

    if (!ftarget) {
        printf("[ERR] Failed to open target file for writing: \"%s\".\n", argv[2]);
        goto exit;
    }

    table = calloc(n, sizeof(arch_entry));

    unsigned ix = 0;

    for (unsigned i = 3; i < argc; i ++) {
        FILE *f = fopen(argv[i], "r");

        if (!f) {
            printf("[ERR] Failed to open file for reading: \"%s\"\n", argv[i]);
            goto exit;
        }

        archEntry_fromFile(&(table[ix ++]), argv[i], f);

        printf("[OK] Open file \"%s\".\n", argv[i]);

        fclose(f);
    }

    file = archFile_createManual(ftarget, n, table);
    archFile_write(file);
    fclose(ftarget);

    printf("[OK] Files packaged to DeCAF archive \"%s\".\n", argv[2]);

    exit:
    if (table)
        free(table);

    if (file)
        archFile_dispose(file);

    return 0;
}

int list(int argc, char **argv) {

    char *src = argv[2];

    FILE *f = fopen(src, "r");

    if (!f) {
        printf("[ERR] Failed to open file for reading: \"%s\".\n", src);
        goto exit;
    }

    arch_file *af = archFile_create(f);

    if (!archFile_parse(af)) {
        printf("[ERR] An error occurred while parsing the file \"%s\".\n", src);
        goto exit;
    }

    archFile_list(af);

    archFile_dispose(af);

    exit:
    if (f)
        fclose(f);

    return 0;
}

int unpack(int argc, char **argv) {

    char *src = argv[2];

    FILE *f = fopen(src, "r");

    if (!f) {
        printf("[ERR] Failed to open file for reading: \"%s\".\n", src);
        goto exit;
    }

    arch_file *af = archFile_create(f);

    if (!archFile_parse(af)) {
        printf("[ERR] An error occurred while parsing the file \"%s\".\n", src);
        goto exit;
    }

    archFile_expand(af);

    archFile_dispose(af);

    exit:
    if (f)
        fclose(f);

    return 0;
}

int main(int argc, char **argv) {

     if (argc <= 1) {
         printf("Usage:\n\tdecaf package [TARGET] [FILES]\n\tdecaf list [ARCH]\n\tdecaf unpack [ARCH]\n");
         return 0;
     }

     char *mode = argv[1];

     if (!strcmp(mode, "package") && argc < 4 || ((!strcmp(mode, "list") || !strcmp(mode, "unpack"))&& argc != 3)) {
         printf("[ERR] Wrong number of parameters.\n");
         return 0;
     }

     if (!strcmp(mode, "package"))
         return package(argc, argv);

     if (!strcmp(mode, "list"))
         return list(argc, argv);

    if (!strcmp(mode, "unpack"))
        return unpack(argc, argv);

     printf("[ERR] Unsupported mode: \"%s\".\n", argv[1]);

    return 0;
}
