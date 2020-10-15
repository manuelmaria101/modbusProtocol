#include <stdio.h>
#include "mmdriver.h"

uint16_t holdReg[10];

int holdingRegisters_R(uint16_t startAddr, uint16_t nReg, uint16_t *data){
    int i = 0;
    printf("\tReading Multiple Registers\n");
    printf("\t\tstartAddr: %d\tnReg: %d\n\t", startAddr, nReg);
    for(int j = startAddr; j < nReg + startAddr; j++){
        data[i] = holdReg[j];
        printf("\treadHoldReg[%d] = %d", j, holdReg[j]);
        i++;
    }
    printf("\n");
    return i;
}


int holdingRegisters_W(uint16_t startAddr, uint16_t nReg, uint16_t *data){
    int i = 0;
    printf("\tWriting Multiple Registers\n");
    printf("\t\tstartAddr: %d\tnReg: %d\n\t", startAddr, nReg);
    for(int j = startAddr; j < nReg + startAddr; j++){
        holdReg[j] = data[i];
        printf("\tholdREG[%d] = %d", j, holdReg[j]);
        i++;
    }
    printf("\n");
    return i;
}

