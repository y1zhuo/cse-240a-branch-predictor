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
    int valve = 1<<(PCBIT-1);
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
    else if(prev == (1<<PCBIT)){
        return 3;
    }
    else{
        return prev;
    }
}

int updatePHT(int prev, int bits, uint8_t outcome){
  int mask = (1<<(bits)) - 1;
  return ((prev<<1) + (outcome ? 1 : 0) ) & mask;
}

//---------------------------------

TNM_Predictor* initTournament(int localHistBit, int globalHistBit, int pcIndexBit){
    TNM_Predictor *tnmPredictor = (TNM_Predictor*)malloc(sizeof(TNM_Predictor));
    tnmPredictor->globalPH = 0;
    tnmPredictor->localPHbits = localHistBit;
    tnmPredictor->globalPHbits = globalHistBit;
    tnmPredictor->pcIdxBits = pcIndexBit;

    // initial local predictor
    tnmPredictor->localPHT = createPTable(bitCnt(pcIndexBit), localHistBit);
    tnmPredictor->localBHT = createPTable(bitCnt(localHistBit), PCBIT);
    initialBHT(&tnmPredictor->localBHT);

    // initial global predictor
    tnmPredictor->globalBHT = createPTable(bitCnt(globalHistBit), PCBIT);
    initialBHT(&tnmPredictor->globalBHT);

    // initial choice predicor
    tnmPredictor->choiceBHT = createPTable(bitCnt(globalHistBit), PCBIT);
    initialBHT(&tnmPredictor->choiceBHT);
    return tnmPredictor;
}

void deleteTNMPredictor(TNM_Predictor *predictor){
    deleteTable(&predictor->localPHT);
    deleteTable(&predictor->localBHT);
    deleteTable(&predictor->globalBHT);
    deleteTable(&predictor->choiceBHT);
    free(predictor);
}

bool localPredictor(TNM_Predictor *predictor, uint32_t pc){
    int mask = bitCnt(predictor->pcIdxBits)-1;
    localPattern = getEntry(&predictor->localPHT, pc & mask);
    localVal = getEntry(&predictor->localBHT,localPattern);
    return getResultFromBHT(localVal);
}

bool globalPredictor(TNM_Predictor *predictor){
    globalVal = getEntry(&predictor->globalBHT, predictor->globalPH);
    return getResultFromBHT(globalVal);
}


uint8_t TNMpredict(TNM_Predictor *predictor, uint32_t pc){
    // make prediction from local predictor
    localRes = localPredictor(predictor, pc);  
    // make prediction from glocal predictor
    glocalRes = globalPredictor(predictor);
    choiceVal = getEntry(&predictor->choiceBHT, predictor->globalPH);
    if(getResultFromBHT(choiceVal)){
        choiceRes = localRes;
    }
    else{
        choiceRes = glocalRes;
    }
    return choiceRes;
}

void TNMtrain(TNM_Predictor *predictor, uint32_t pc, uint8_t outcome){
    // Train local predictor
    setEntry(&predictor->localBHT, localPattern, updateBHT(localVal, outcome));
    localPattern = updatePHT(localPattern, predictor->localPHbits, outcome);
    setEntry(&predictor->localPHT, pc & bitCnt(predictor->pcIdxBits)-1, localPattern);

    // Train global predictor
    setEntry(&predictor->globalBHT, predictor->globalPH, updateBHT(globalVal, outcome));

    // Train choice predictor
    if(localRes == outcome && glocalRes != outcome){
        setEntry(&predictor->choiceBHT, predictor->globalPH, updateBHT(choiceVal, true));
    }
    else if(localRes != outcome && glocalRes == outcome){
        setEntry(&predictor->choiceBHT, predictor->globalPH, updateBHT(choiceVal, false));
    }

    // Update global history
    predictor->globalPH = updatePHT(predictor->globalPH, predictor->localPHbits, outcome);
}