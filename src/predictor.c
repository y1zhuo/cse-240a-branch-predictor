//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//

#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Yue Zhuo";
const char *studentID   = "A16110292";
const char *email       = "y1zhuo@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

//define number of bits required for indexing the BHT here. 
int ghistoryBits = 14; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;


//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
//gshare
uint8_t *bht_gshare;
uint8_t ghistory;

// Tournament
TNM_Predictor *tnm;

// Custom
Custom_Predictor *cPredictor;

int bitCnt(int bits){
    return 1<<bits;
}


void initialBHT(PTable * table){
    for(int i = 0; i<table->height; i++){
        setEntry(table, i, WN);
    }
}

bool getResultFromBHT(int val){
    int valve = 1<<(PCBIT-1);
    return (val>=valve) ? TAKEN : NOTTAKEN;
}

uint8_t updateBHT(int prev, bool result){
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
        return (1<<PCBIT)- 1;
    }
    else{
        return prev;
    }
}


int updatePH(int prev, int bits, uint8_t outcome){
  int mask = (1<<(bits)) - 1;
  return ((prev<<1) + (outcome ? 1 : 0) ) & mask;
}

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

//-----------------------------------------------------

// Initialize the predictor
//

//gshare functions
void init_gshare() {
  int bht_entries = 1 << ghistoryBits;
  bht_gshare = (uint8_t*)malloc(bht_entries * sizeof(uint8_t));
  int i = 0;
  for(i = 0; i< bht_entries; i++){
    bht_gshare[i] = WN;
  }
  ghistory = 0;
}


uint8_t 
gshare_predict(uint32_t pc) {
  //get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries-1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries -1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;

switch(bht_gshare[index]){
    case SN:
      return NOTTAKEN;
    case WN:
      return NOTTAKEN;
    case WT:
      return TAKEN;
    case ST:
      return TAKEN;
    default:
      printf("Warning: Undefined state of entry in GSHARE BHT!\n");
      return NOTTAKEN;
  }
}

void
train_gshare(uint32_t pc, uint8_t outcome) {
  //get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries-1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries -1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;

  //Update state of entry in bht based on outcome
  switch(bht_gshare[index]){
    case SN:
      bht_gshare[index] = (outcome==TAKEN)?WN:SN;
      break;
    case WN:
     bht_gshare[index] = (outcome==TAKEN)?WT:SN;
      break;
    case WT:
      bht_gshare[index] = (outcome==TAKEN)?ST:WN;
      break;
    case ST:
      bht_gshare[index] = (outcome==TAKEN)?ST:WT;
      break;
    default:
    printf("Warning: Undefined state of entry in GSHARE BHT!\n");
      break;
  }
  ghistory = ((ghistory << 1) | outcome); 
}

void
cleanup_gshare() {
  free(bht_gshare);
}

void cleanup_TNM(){
  deleteTNMPredictor(tnm);
}

void cleanup_custom(){
  free(cPredictor->choicePHT);
  free(cPredictor->takenPHT);
  free(cPredictor->notTakenPHT);
  free(cPredictor);
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
    localPattern = updatePH(localPattern, predictor->localPHbits, outcome);
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
    predictor->globalPH = updatePH(predictor->globalPH, predictor->localPHbits, outcome);
}

// ----------------Custom Predictor--------------------
Custom_Predictor* initCustom(){
  Custom_Predictor *custom = (Custom_Predictor*)malloc(sizeof(Custom_Predictor));
  ghistoryBits = 12;
  custom->pht_size = 1<<ghistoryBits;
  custom->mask = custom->pht_size-1;
  custom->global_history = 0;
  custom->choicePHT = (uint8_t*)malloc(sizeof(uint8_t) * custom->pht_size);
  custom->takenPHT = (uint8_t*)malloc(sizeof(uint8_t) * custom->pht_size);
  custom->notTakenPHT = (uint8_t*)malloc(sizeof(uint8_t) * custom->pht_size);

  // initial PHT
  for(int i = 0; i < custom->pht_size; i++){
    custom->choicePHT[i] = WN;
    custom->notTakenPHT[i] = WN;
    custom->takenPHT[i] = WT;
  }

  return custom;
}



bool customPredict(Custom_Predictor *predictor, uint32_t pc){
    predictor->index = (pc ^ predictor->global_history) & predictor->mask;
    pc = pc & predictor->mask;
    predictor->choice = getResultFromBHT(predictor->choicePHT[pc]);
    if(predictor->choice){
      predictor->customRes = getResultFromBHT(predictor->takenPHT[predictor->index]);
    }
    else{
      predictor->customRes = getResultFromBHT(predictor->notTakenPHT[predictor->index]);
    }
    return predictor->customRes;
}

void customTrain(Custom_Predictor *predictor, uint32_t pc, uint8_t outcome){
    uint8_t customRes = customPredict(predictor, pc);
    pc = pc & predictor->mask;
    
    if(!(predictor->choice != outcome && customRes == outcome)){
      predictor->choicePHT[pc] = updateBHT(predictor->choicePHT[pc], outcome);
    }
    if(predictor->choice){
      predictor->takenPHT[predictor->index] = updateBHT(predictor->takenPHT[predictor->index], outcome);
    }
    else{
      predictor->notTakenPHT[predictor->index] = updateBHT(predictor->notTakenPHT[predictor->index], outcome);
    }

    predictor->global_history = updatePH(predictor->global_history, ghistoryBits, outcome);
}



//------------------------------------//
//        Predictor Functions         //
//------------------------------------//


void
init_predictor()
{
  switch (bpType) {
    case STATIC:
    case GSHARE:
      init_gshare();
      break;
    case TOURNAMENT:
      // tnm = initTournament(lhistoryBits, ghistoryBits, pcIndexBits);
      tnm = initTournament(10, 12, 10);
      break;
    case CUSTOM:
      cPredictor = initCustom();
    default:
      break;
  }
  
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return gshare_predict(pc);
    case TOURNAMENT:
      return TNMpredict(tnm, pc);
    case CUSTOM:
      return customPredict(cPredictor,pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void
train_predictor(uint32_t pc, uint8_t outcome)
{

  switch (bpType) {
    case STATIC:
    case GSHARE:
      train_gshare(pc, outcome);
      break;
    case TOURNAMENT:
      TNMtrain(tnm, pc, outcome);
      break;
    case CUSTOM:
      customTrain(cPredictor,pc,outcome);
    default:
      break;
  }
  

}
