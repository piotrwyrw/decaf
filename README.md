# DeCAF

A very simple archive file format.

## File format

| Description             | Size / Data type | Structure variable |
|-------------------------|------------------|--------------------|
| **0xDECAF**             | uint32_t         | _(not applicable)_ |
| Amount of entries       | uint64_t         | entry_ct           |
| Entry table (see below) | _(complex)_      | table              |

## File entry

| Description                               | Size / Data type | Structure variable |
|-------------------------------------------|------------------|--------------------|
| **0xC0FFEE**                              | uint32_t         | _(not applicable)_ |
| Length of the file name                   | uint64_t         | name_le            |
| File name with the length specified above | char [name_le]   | name               |
| Size of the raw data                      | uint64_t         | data_le            |
| Raw file data                             | void *           | data               |