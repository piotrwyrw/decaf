//
// Created by pwpio on 23/01/2023.
//

#ifndef DCAF_ARCH_H
#define DCAF_ARCH_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define DECAF_MAGIC (0xDECAF)
#define DECAF_ENTRY (0xC0FFEE)

typedef enum {
    DECAF_WRONG_MAGIC_SIZE                      = 0,
    DECAF_WRONG_MAGIC                           = 0,
    DECAF_WRONG_ENTRY_MAGIC_SIZE                = 0,
    DECAF_WRONG_ENTRY_MAGIC                     = 0,

    DECAF_WRONG_ENTRY_COUNT_SIZE                = 0,
    DECAF_WRONG_ENTRY_NAME_SIZE                 = 0,
    DECAF_WRONG_ENTRY_FILE_NAME_LENGTH          = 0,
    DECAF_WRONG_ENTRY_DATA_LENGTH_SIZE          = 0,
    DECAF_WRONG_ENTRY_DATA_LENGTH               = 0,

    DECAF_TOO_MANY_ENTRIES                      = 0,
    DECAF_TOO_FEW_ENTRIES                       = 0,

    DECAF_NOTHING                                = 1,
    DECAF_OK                                     = 1
} decaf_error;

typedef struct {
    uint64_t    name_le;
    char        *name;
    uint64_t    data_le;
    void        *data;
} arch_entry;

typedef struct {
    FILE        *file;
    uint64_t    entry_ct;
    arch_entry  *table;
} arch_file;

void archEntry_fromFile(arch_entry *target, char *name, FILE *f);
void archEntry_freeBuffers(arch_entry *entry);

arch_file *archFile_create(FILE *f);
arch_file *archFile_createManual(FILE *f, uint64_t entry_ct, arch_entry *entries);
void archFile_dispose(arch_file *af);
void archFile_write(arch_file *af);

decaf_error archFile_parse(arch_file *af);

#endif //DCAF_ARCH_H
