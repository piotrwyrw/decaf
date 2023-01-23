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

decaf_error archFile_parse(arch_file *af) {
    // Ensure that this is indeed a DeCAF file by comparing the magic number
    uint32_t magic;
    uint64_t count;

    if (fread(&magic, sizeof(uint32_t), 1, af->file) != sizeof(uint32_t))
        return DECAF_WRONG_MAGIC_SIZE;

    if (magic != DECAF_MAGIC)
        return DECAF_WRONG_MAGIC;

    // Then, get the entry count (control value)

    if (fread(&count, sizeof(uint64_t), 1, af->file) != sizeof(uint64_t))
        return DECAF_WRONG_ENTRY_COUNT_SIZE;

    af->entry_ct = count;
    af->table = malloc(sizeof(arch_entry) * count);

    /**
     * Parse the entries
     */

    int i = 0;
    int c = 1;

    while (1) {

        if (feof(af->file))
            break;

        // Entry magic number
        if (fread(&magic, sizeof(uint32_t), 1, af->file) != sizeof(uint32_t))
            return DECAF_WRONG_ENTRY_MAGIC_SIZE;

        if (magic != DECAF_ENTRY)
            return DECAF_WRONG_ENTRY_MAGIC;

        i ++;
        c ++;

        if (c > count)
            return DECAF_TOO_MANY_ENTRIES;

        uint64_t name_le;
        char *name;
        uint64_t data_le;
        void *data;

        // Name length
        if (fread(&name_le, sizeof(uint64_t), 1, af->file) != sizeof(uint64_t))
            return DECAF_WRONG_ENTRY_NAME_SIZE;

        name = calloc(name_le + 1, sizeof(char));

        // File name
        if (fread(&name, sizeof(char), name_le, af->file) != sizeof(char) * name_le)
            return DECAF_WRONG_ENTRY_FILE_NAME_LENGTH;

        // Data size
        if (fread(&data_le, sizeof(uint64_t), 1, af->file) != sizeof(uint64_t))
            return DECAF_WRONG_ENTRY_DATA_LENGTH_SIZE;

        data = calloc(data_le, sizeof(char));

        // Raw data
        if (fread(&data, data_le, 1, af->file) != data_le)
            return DECAF_WRONG_ENTRY_DATA_LENGTH;

        af->table[i].name_le = name_le;
        af->table[i].name = name;
        af->table[i].data_le = data_le;
        af->table[i].data = data;
    }

    if (c < count)
        return DECAF_TOO_FEW_ENTRIES;

    return DECAF_OK;
}