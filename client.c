#include <stdio.h>
#include "ModbusAP.h"

int main(){
    uint8_t data[PDU_SIZE];
    int nReg = 3;
    uint8_t writeReg[nReg*2];
    writeReg[0] = MSB(1);
    writeReg[1] = LSB(1);
    writeReg[2] = MSB(2);
    writeReg[3] = LSB(2);
    writeReg[4] = MSB(3);
    writeReg[5] = LSB(3);
    int nRegWrite = WriteMultipleRegisters("127.0.0.1", 502, 0, nReg, writeReg);
    printf("\t\tnReg Written: %d\n", nRegWrite);

    nReg = ReadHoldingRegisters("127.0.0.1", 502, 0, nReg, data);
    printf("data: ");
    for(int i = 0; i < (nReg*2); i++){
        printf("[%d]", data[i]);
    }
    printf("\n");
}

