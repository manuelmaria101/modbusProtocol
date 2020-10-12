#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "ModbusTCP.h"
#include "ModbusAP.h"
#include "mmdriver.h"

/*-----------------------------------------------Client---------------------------------------------*/

int ReadHoldingRegisters(char *servAddr, uint16_t port, uint16_t startAddr, uint16_t nReg, uint8_t *data)
{
    printf("\nClient Read Multiple Registers\n");
    printf("\t\tst_ad = %d\tnr = %d\t", startAddr, nReg);
    if(startAddr > NUMBEROFREG) return -1;
    uint8_t APDU[PDU_SIZE], APDU_r[PDU_SIZE];
    APDU[0] = READ_HOLDING_REG;
    APDU[1] = MSB(startAddr);
    APDU[2] = LSB(startAddr);
    APDU[3] = MSB(nReg);
    APDU[4] = LSB(nReg);
    int len = sendModbusRequest("127.0.0.1", port, APDU, 5, APDU_r);
    memcpy(data, APDU_r + 2, len-2);
    if(len < 0) return -1;
    return (len - 2)/2;
}

int WriteMultipleRegisters(char *servAddr, uint16_t port, uint16_t startAddr, uint16_t nReg, uint8_t *data){
    printf("\nClient Write Multiple Registers\n");
    printf("\t\tst_ad = %d\tnr = %d\t", startAddr, nReg);
    printf("data: [%d%d][%d%d][%d%d]\n", data[0],data[1],data[2],data[3],data[4],data[5]);
    if(startAddr > NUMBEROFREG) return -1;
    if(startAddr + nReg - 1 > NUMBEROFREG) return -2;
    if(nReg > NUMBEROFREG) return -3;

    uint8_t APDU[PDU_SIZE], APDU_r[PDU_SIZE];
    APDU[0] = WRITE_MULTIPLE_REG;
    APDU[1] = MSB(startAddr);
    APDU[2] = LSB(startAddr);
    APDU[3] = MSB(nReg);
    APDU[4] = LSB(nReg);
    uint8_t len = nReg*2;
    APDU[5] = len;

    int APDU_len = len + 6;
    memcpy(APDU + 6, data, len);

    len = sendModbusRequest(servAddr, port, APDU, APDU_len, APDU_r);
    if(len < 0) return -4;
    return concatenate(APDU_r[len-2], APDU_r[len-1]);
}

/*-----------------------------------------------------------Server----------------------------------------------------*/


int getModbusRequest(int serverSocket, uint8_t *operation, int *startAddr, int *nReg, uint8_t *data){
    int APDUlen;
    uint8_t APDU[PDU_SIZE];
    int8_t transactID = receiveModbusRequest(serverSocket, APDU, APDUlen);
    *operation = APDU[0];
    switch(*operation){
        case WRITE_MULTIPLE_REG: {
            *startAddr = concatenate(MSB(APDU[1]), LSB(APDU[2]));
            *nReg = concatenate(MSB(APDU[3]), LSB(APDU[4]));
            uint8_t len = APDU[5];
            memcpy(data, APDU + 6, len);
        }break;
        case READ_HOLDING_REG:{
            *startAddr = concatenate(MSB(APDU[1]), LSB(APDU[2]));
            *nReg = concatenate(MSB(APDU[3]), LSB(APDU[4]));
        }break;
        default: break;
    }
    return transactID;
}

int sendAPResponse(int trasactID, uint8_t operation, int startAddr, int nReg, uint8_t *data){
    int APDU_rlen;
    uint8_t APDU_r[PDU_SIZE];
    switch(operation){
        case READ_HOLDING_REG: {
            APDU_r[0] = READ_HOLDING_REG;
            APDU_r[1] = nReg*2;
            memcpy(APDU_r + 2, data, nReg*2);
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


/*----------------------------------------------------------------Auxiliar---------------------------------------*/

int concatenate(int x, int y){
    int pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;
}