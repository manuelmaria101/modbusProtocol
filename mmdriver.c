#include <stdio.h>
#include "mmdriver.h"
#include "ModbusAP.h"

int holdReg[10];

int holdingRegisters_R(int startAddr, int nReg, uint8_t *data){
    int i = 0;
    printf("\tReading Multiple Registers\n");
    printf("\t\tstartAddr: %d\tnReg: %d\n\t", startAddr, nReg);
    for(int j = startAddr; j < nReg; j++){
        data[i] = MSB((uint16_t)holdReg[j]);
        data[i+1] = LSB((uint16_t)holdReg[j]);
        i+=2;
        printf("\treadHoldReg[%d] = %d", j, holdReg[j]);
    }
    printf("\n");
    return i/2;
}


int holdingRegisters_W(int startAddr, int nReg, uint8_t *data){
    int i = 0;
    printf("\tWriting Multiple Registers\n");
    printf("\t\tstartAddr: %d\tnReg: %d\n\t", startAddr, nReg);
    for(int j = startAddr; j < nReg; j++){
        holdReg[j] = concatenate(data[i], data[i+1]);
        printf("\tholdREG[%d] = %d", j, holdReg[j]);
        i+=2;
    }
    printf("\n");
    return i/2;
}

