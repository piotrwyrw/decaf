#include "arch.h"

void archEntry_fromFile(arch_entry *target, char *name, FILE *f) {
    target->name_le = strlen(name);

    target->name = malloc(strlen(name) + 1);
    strcpy(target->name, name);

    fseek(f, 0, SEEK_END);
    target->data_le = ftell(f);
    rewind(f);

    target->data = malloc(target->data_le);
    fread(target->data, sizeof(char), target->data_le, f);
}

void archEntry_freeBuffers(arch_entry *entry) {
    if (entry->name)
        free(entry->name);

    if (entry->data)
        free(entry->data);
}

_Bool archEntry_isNull(arch_entry *entry) {
    return entry->data == NULL || entry->name == NULL;
}

arch_file *archFile_create(FILE *f) {
    arch_file *af = malloc(sizeof(arch_file));
    af->file = f;
    af->entry_ct = 0;
    af->table = NULL;
    return af;
}

arch_file *archFile_createManual(FILE *f, uint64_t entry_ct, arch_entry *entries) {
    arch_file *af = malloc(sizeof(arch_file));
    af->file = f;
    af->entry_ct = entry_ct;

    af->table = malloc(sizeof(arch_entry) * entry_ct);

    for (uint64_t i = 0; i < entry_ct; i ++)
        af->table[i] = entries[i];

    return af;
}

void archFile_dispose(arch_file *af) {
    if (af->table) {
        for (uint64_t i = 0; i < af->entry_ct; i ++)
            archEntry_freeBuffers(&(af->table[i]));

        free(af->table);
    }

    free(af);
}

void archFile_write(arch_file *af) {
    uint32_t magic = DECAF_MAGIC;

    fwrite(&magic, sizeof(uint32_t), 1, af->file);
    fwrite(&(af->entry_ct), sizeof(uint64_t), 1, af->file);

    magic = 0xC0FFEE;

    // Write the entries
    for (uint64_t i = 0; i < af->entry_ct; i ++) {
        arch_entry entry = af->table[i];

        fwrite(&magic, sizeof(uint32_t), 1, af->file);
        fwrite(&(entry.name_le), sizeof(uint64_t), 1, af->file);
        fwrite(entry.name, sizeof(char), entry.name_le, af->file);
        fwrite(&(entry.data_le), sizeof(uint64_t), 1, af->file);
        fwrite(entry.data, sizeof(char), entry.data_le, af->file);
    }

    fflush(af->file);

}

arch_entry archEntry_parseOne(arch_file *af) {
    // For error checking
    size_t read;

    // Magic number
    uint32_t magic;
    fread(&magic, sizeof(uint32_t), 1, af->file);

    if (magic != DECAF_ENTRY)
        return NULL_ENTRY;

    // Length of the file name
    uint64_t name_length;
    read = fread(&name_length, sizeof(uint64_t), 1, af->file);

    if (read != 1)
        return NULL_ENTRY;

    // File name
    char *name = calloc(name_length + 1, sizeof(char));
    read = fread(name, sizeof(char), name_length, af->file);

    if (read != name_length) {
        free(name);
        return NULL_ENTRY;
    }

    // Size of the raw data
    uint64_t data_size;
    read = fread(&data_size, sizeof(uint64_t), 1, af->file);

    if (read != 1)
        return NULL_ENTRY;

    // Raw data
    char *data = calloc(data_size, sizeof(char));
    read = fread(data, sizeof(char), data_size, af->file);

    if (read != data_size) {
        free(data);
        free(name);
        return NULL_ENTRY;
    }

    return (arch_entry) {.name_le = name_length, .name = name, .data_le = data_size, .data = data};
}

_Bool archFile_parse(arch_file *af) {

    // Amount of elements read, for error checking
    size_t read;

    // First, ensure that the magic number is in place
    uint32_t magic;
    fread(&magic, sizeof(uint32_t), 1, af->file);

    if (magic != DECAF_MAGIC)
        return 0;

    // Get the entries count
    uint64_t entries;
    read = fread(&entries, sizeof(uint64_t), 1, af->file);

    if (read != 1)
        return 0;

    // Allocate the entry table
    arch_entry *table = calloc(sizeof(arch_entry), entries);

    // Parse the entries

    int ix = 0;

    while (1) {
        if (feof(af->file) || ix >= entries)
            break;

        arch_entry entry = archEntry_parseOne(af);

        if (archEntry_isNull(&entry))
            goto exit_error;

        table[ix ++] = entry;
    }

    // Commit everything to the target struct
    af->table = table;
    af->entry_ct = entries;

    return 1;

    exit_error:
    free(table);
    return 0;
}

void archFile_list(arch_file *af) {
    printf("The archive contains %ld:w: entries:\n", af->entry_ct);

    if (!af->table) {
        printf("Failed to list the contents: The allocation table is null.\n");
        return;
    }

    for (uint64_t i = 0; i < af->entry_ct; i ++)
        printf("\t[%ld bytes] %s\n", af->table[i].data_le, af->table[i].name);
}