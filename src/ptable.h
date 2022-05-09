#ifndef PTABLE_H
#define PTABLE_H


#include <stdbool.h>
#include <stdlib.h>


#define PCBIT 2

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