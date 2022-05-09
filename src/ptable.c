#include "ptable.h"
PTable createPTable(int cnt, int size){
    PTable table;
    table.data = (uint32_t*)calloc(cnt, sizeof(uint32_t));
    table.bitsSize = size;
    table.height = cnt;
    return table;
}

void deleteTable(PTable *table){
    free(table->data);
}

uint32_t getEntry(PTable *table, int index){
    return table->data[index];
}

void setEntry(PTable *table, int index, uint32_t entry){
    table->data[index] = entry;
}