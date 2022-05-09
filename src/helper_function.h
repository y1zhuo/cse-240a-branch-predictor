#ifndef HELPER_FUNCTION_H
#define HELPER_FUNCTION_H
#include <stdbool.h>
#include <stdlib.h>
#include "ptable.h"
// -------helper function---------------
int bitCnt(int bits);

void initialBHT(PTable * table);

bool getResultFromBHT(int val);

int updateBHT(int prev, bool result);

int updatePHT(int prev, int bits, uint8_t outcome);
#endif