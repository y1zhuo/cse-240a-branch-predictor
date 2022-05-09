#include "tournament.h"
// -------helper function---------------
int bitCnt(int bits){
    return 1<<bits;
}


void initialBHT(PTable * table){
    for(int i = 0; i<table->height; i++){
        setEntry(table, i, 1);
    }
}

bool getResultFromBHT(int val){
    int valve = 1<<(BHTBIT-1);
    return (val>=valve) ? true : false;
}

int updateBHT(int prev, bool result){
    if(result){
        prev++;
    }
    else{
        prev--;
    }
    if(prev < 0){
        return 0;
    }
    else if(prev == (1<<BHTBIT)){
        return 3;
    }
    else{
        return prev;
    }
}

int updatePHT(int prev, int bits, bool outcome){
  int mask = (1<<(bits)) - 1;
  return ((prev<<1) + (outcome ? 1 : 0) ) & mask;
}

//---------------------------------

TNM_Predictor* initTournament(int localHistBit, int globalHistBit, int pcIndexBit){
    TNM_Predictor *myPredictor = (TNM_Predictor*)malloc(sizeof(TNM_Predictor));
    myPredictor->globalPH = 0;
    myPredictor->localPHbits = localHistBit;
    myPredictor->globalPHbits = globalHistBit;
    myPredictor->pcIdxBits = pcIndexBit;
    myPredictor->localPHT = createPTable(bitCnt(pcIndexBit), localHistBit);
    myPredictor->localBHT = createPTable(bitCnt(localHistBit), 2);
    initialBHT(& myPredictor->localBHT);
    return myPredictor;
}

void deleteTNMPredictor(TNM_Predictor *predictor){
    deleteTable(&predictor->localPHT);
    deleteTable(&predictor->localBHT);
}

bool localPredictor(TNM_Predictor *predictor, uint32_t pc){
    int mask = (1<<(predictor->pcIdxBits))-1;
    int pattern = getEntry(&predictor->localPHT, pc & mask);
    int val = getEntry(&predictor->localBHT,pattern);
    return getResultFromBHT(val);
}


uint8_t TNMpredict(TNM_Predictor *predictor, uint32_t pc){
    bool localRes = localPredictor(predictor, pc);

    return localRes;
}

void TNMtrain(TNM_Predictor *predictor, uint32_t pc, uint8_t outcome){
    int mask = (1<<(predictor->pcIdxBits))-1;
    int pattern = getEntry(&predictor->localPHT, pc & mask);
    int prev = getEntry(&predictor->localBHT,pattern);
    bool localRes = getResultFromBHT(prev);
    setEntry(&predictor->localBHT, pattern, updateBHT(prev, localRes));
    pattern = updatePHT(pattern, predictor->localPHbits, outcome);
    setEntry(&predictor->localPHT, pc & mask, pattern);

}