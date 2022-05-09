#include "helper_function.h"

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