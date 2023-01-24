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
#define NULL_ENTRY (arch_entry) {.data_le = 0, .data = NULL, .name_le = 0, .name = NULL}

#define ERR_NO_ENTRY_MAGIC 0

#define ERR_FIELD_FILE_NAME_LENGTH (-1)
#define ERR_FIELD_FILE_NAME (-2)
#define ERR_FIELD_RAW_DATA_SIZE (-3)
#define ERR_FIELD_RAW_DATA (-4)

#define ERR_FIELD_ENTRIES (-5)
#define ERR_ENTRY_NULL (-6)

#define ERR_FILE_TOO_SMALL (-7)
#define ERR_READ_FAILED (-8)

typedef struct {
    uint64_t name_le;
    char *name;
    uint64_t data_le;
    void *data;
} arch_entry;

typedef struct {
    FILE *file;
    uint64_t entry_ct;
    arch_entry *table;
} arch_file;

static int last_error = 1;

void archEntry_fromFile(arch_entry *target, char *name, FILE *f);

void archEntry_freeBuffers(arch_entry *entry);

_Bool archEntry_isNull(arch_entry *entry);

arch_file *archFile_create(FILE *f);

arch_file *archFile_createManual(FILE *f, uint64_t entry_ct, arch_entry *entries);

void archFile_dispose(arch_file *af);

void archFile_write(arch_file *af);

int archFile_parse(arch_file *af);

void archFile_list(arch_file *af);

void archFile_expand(arch_file *af);

const char *last_error_string();

#endif //DCAF_ARCH_H
