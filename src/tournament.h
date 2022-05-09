#ifndef TOURNAMENT_H
#define TOURNAMENT_H
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "ptable.h"
#define PCBIT 2


typedef struct _Tournament_Predictor
{
    int globalPH, globalPHbits, pcIdxBits, localPHbits;
    PTable localPHT, localBHT, globalBHT, choiceBHT;

} TNM_Predictor;

TNM_Predictor* initTournament(int localHistBit, int globalHistBit, int pcIndexBit);

void deleteTNMPredictor(TNM_Predictor *predictor);

bool localPredictor(TNM_Predictor *predictor, uint32_t pc);

bool globalPredictor(TNM_Predictor *predictor);

uint8_t TNMpredict(TNM_Predictor *predictor, uint32_t pc);

void TNMtrain(TNM_Predictor *predictor, uint32_t pc, uint8_t outcome);

// Global Variables
int localPattern, localVal, globalVal, choiceVal; // local predictor
bool localRes, glocalRes, choiceRes;    // prediction outcome


#endif