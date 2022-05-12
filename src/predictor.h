//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
//
// Student Information
//
extern const char *studentName;
extern const char *studentID;
extern const char *email;

//------------------------------------//
//      Global Predictor Defines      //
//------------------------------------//
#define NOTTAKEN  0
#define TAKEN     1

// The Different Predictor Types
#define STATIC      0
#define GSHARE      1
#define TOURNAMENT  2
#define CUSTOM      3
extern const char *bpName[];

// Definitions for 2-bit counters
#define SN  0			// predict NT, strong not taken
#define WN  1			// predict NT, weak not taken
#define WT  2			// predict T, weak taken
#define ST  3			// predict T, strong taken
#define PCBIT 2

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//
extern int ghistoryBits; // Number of bits used for Global History
extern int lhistoryBits; // Number of bits used for Local History
extern int pcIndexBits;  // Number of bits used for PC index
extern int bpType;       // Branch Prediction Type
extern int verbose;

//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void init_predictor();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome);

void cleanup_gshare();

void cleanup_TNM();

// -------Predictor Table Class---------------

typedef struct _PTable{
    uint32_t *data;
    int bitsSize;
    int height;
} PTable;

PTable createPTable(int cnt, int size);

void deleteTable(PTable *table);

uint32_t getEntry(PTable *table, int index);

void setEntry(PTable *table, int index, uint32_t entry);

// ------Helper Function----------------------------

int bitCnt(int bits);

void initialBHT(PTable * table);

bool getResultFromBHT(int val);

uint8_t updateBHT(int prev, bool result);

int updatePHT(int prev, int bits, uint8_t outcome);

// ---------------Tournament_Predictor---------------------------

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

// ------------Custom_Predictor--------------------------
typedef struct _Custom_Predictor
{
    uint8_t *choicePHT, *takenPHT, *notTakenPHT;
    uint32_t global_history, mask, index;
    uint8_t choice, customRes;
    int pht_size;
} Custom_Predictor;

void cleanup_custom();

Custom_Predictor* initCustom();

bool customPredict(Custom_Predictor *predictor, uint32_t pc);

void customTrain(Custom_Predictor *predictor, uint32_t pc, uint8_t outcome);

#endif
