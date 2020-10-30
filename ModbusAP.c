#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "ModbusAP.h"
#include "ModbusTCP.h"

/*-----------------------------------------------Client---------------------------------------------*/

int ReadHoldingRegisters(char *servAddr, uint16_t port, uint16_t startAddr, uint16_t nReg, uint16_t *data)
{
    printf("\nClient Read Multiple Registers\n");
    printf("\t\tst_ad = %d\tnr = %d\t", startAddr, nReg);
    if(startAddr < 1) return -1;
    if(startAddr + nReg > 65537) return -2;
    if(nReg > 125) return -3; // -> tabela 36.2 Modbus Chapter
    startAddr--;

    uint8_t APDU[PDU_SIZE], APDU_r[PDU_SIZE];
    APDU[0] = READ_HOLDING_REG;
    APDU[1] = MSB(startAddr);
    APDU[2] = LSB(startAddr);
    APDU[3] = MSB(nReg);
    APDU[4] = LSB(nReg);
    int len = sendModbusRequest("127.0.0.1", port, APDU, 5, APDU_r);
    if(len != (2*nReg + 2)) return -1;
    if(APDU_r[0] != READ_HOLDING_REG) return -2;
    if(APDU_r[1] != nReg*2) return -3;
    // memcpy(data, APDU_r + 2, len-2);
   for(int i = 0; i < nReg; i++)
        data[i] = 256*APDU_r[2 + 2*i] + APDU_r[2 + 2*i + 1];
    return (len - 2)/2;
}

int WriteMultipleRegisters(char *servAddr, uint16_t port, uint16_t startAddr, uint16_t nReg, uint16_t *data){
    printf("\nClient Write Multiple Registers\n");
    printf("\t\tst_ad = %d\tnr = %d\t", startAddr, nReg);
    if(startAddr < 1) return -1;
    if(startAddr + nReg > 65537) return -2;
    if(nReg > 123) return -3; // -> tabela 36.2 Modbus Chapter
    startAddr--;

    uint8_t APDU[PDU_SIZE], APDU_r[PDU_SIZE];
    APDU[0] = WRITE_MULTIPLE_REG;
    APDU[1] = MSB(startAddr);
    APDU[2] = LSB(startAddr);
    APDU[3] = MSB(nReg);
    APDU[4] = LSB(nReg);
    uint8_t len = nReg*2;
    APDU[5] = len;

    int APDU_len = len + 6;
    for(int i = 0; i < nReg; i++){
        APDU[6 + 2*i] = MSB(data[i]);
        APDU[6 + 2*i +1] = LSB(data[i]);
    }
    len = sendModbusRequest(servAddr, port, APDU, APDU_len, APDU_r);
    if(len != 5) return -1;
    if(APDU_r[0] !=  WRITE_MULTIPLE_REG) return -2;
    if(APDU_r[1] != MSB(startAddr) || APDU_r[2] != LSB(startAddr)) return -3;
    if(APDU_r[3] != MSB(nReg) || APDU_r[4] != LSB(nReg)) return -4;
    return (256*APDU_r[len-2] + APDU_r[len-1]);
}

/*-----------------------------------------------------------Server----------------------------------------------------*/


int getModbusRequest(int serverSocket, uint8_t *operation, uint16_t *startAddr, uint16_t *nReg, uint16_t *data){
    int APDUlen;
    uint8_t APDU[PDU_SIZE];
    int8_t transactID = receiveModbusRequest(serverSocket, APDU, APDUlen);
    *operation = APDU[0];
    switch(*operation){
        case WRITE_MULTIPLE_REG: {
            *startAddr = 256*APDU[1] + APDU[2];
            *nReg = 256*APDU[3] + APDU[4];
            uint8_t len = APDU[5];
            for(int i = 0; i < *nReg; i++)
                data[i] = 256*APDU[6 + 2*i] + APDU[6 + 2*i + 1];
        }break;
        case READ_HOLDING_REG:{
            *startAddr = 256*APDU[1] + APDU[2]; 
            *nReg = 256*APDU[3] + APDU[4]; 
        }break;
        default: break;
    }
    return transactID;
}

int sendAPResponse(int trasactID, uint8_t operation, uint16_t startAddr, uint16_t nReg, uint16_t *data){
    int APDU_rlen;
    uint8_t APDU_r[PDU_SIZE];
    switch(operation){
        case READ_HOLDING_REG: {
            APDU_r[0] = READ_HOLDING_REG;
            APDU_r[1] = nReg*2;
            for(int i = 0; i < nReg; i++){
                APDU_r[2 + 2*i] = MSB(data[i]);
                APDU_r[2 + 2*i +1] = LSB(data[i]);
            }
            APDU_rlen = nReg*2 + 2;
        }break;
        case WRITE_MULTIPLE_REG:{
            APDU_r[0] = WRITE_MULTIPLE_REG;
            APDU_r[1] = MSB(startAddr);
            APDU_r[2] = LSB(startAddr);
            APDU_r[3] = MSB(nReg);
            APDU_r[4] = LSB(nReg);
            APDU_rlen = 5;
        }break;
        default: break;
    }
    APDU_rlen = sendModbusResponse(trasactID, APDU_r, APDU_rlen);
    return APDU_rlen;
}