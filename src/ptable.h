#ifndef PTABLE_H
#define PTABLE_H

#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

typedef struct _PTable{
    uint32_t *data;
    int bitsSize;
    int height;
} PTable;

PTable createPTable(int cnt, int size);

void deleteTable(PTable *table);

uint32_t getEntry(PTable *table, int index);

void setEntry(PTable *table, int index, uint32_t entry);

#endif