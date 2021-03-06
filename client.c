#include <stdio.h>
#include "ModbusAP.h"

int main(){
    int nReg = 9;
    uint16_t data[PDU_SIZE], writeReg[nReg*2];
    uint16_t operation, startAdd;
    while(1){
        printf("Function code: (3) ou (10) ou (FF)\n");
        scanf("%x", &operation);
        switch (operation)
        {
            case READ_HOLDING_REG: {
                printf("start address: ");
                scanf("%d", &startAdd);
                printf("number of registers: ");
                scanf("%d", &nReg);
                nReg = ReadHoldingRegisters("127.0.0.1", 502, startAdd, nReg, data);
                printf("registers read: %d", nReg);
                printf("data: ");
                for(int i = 0; i < (nReg); i++){
                    printf("[%d]", data[i]);
                }
                printf("\n");
            } break;
            case WRITE_MULTIPLE_REG:{
                printf("start address: ");
                scanf("%d", &startAdd);
                printf("number of registers: ");
                scanf("%d", &nReg);
                int j = 0;
                int aux;
                for(int i = startAdd; i < nReg + startAdd; i++){
                    printf("value for register %d: ", i);
                    scanf("%d", &aux);
                    writeReg[j] = aux;
                    j++;
                }
                int nRegWrite = WriteMultipleRegisters("127.0.0.1", 502, startAdd, nReg, writeReg);
                printf("\t\tnReg Written: %d\n", nRegWrite);
            }break;
            case END_COMMUNICATION: {
                return 0;
            }break;
            default:
                break;
            }
    }
}

