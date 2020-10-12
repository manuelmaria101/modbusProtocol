#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "ModbusTCP.h"
#include "mmdriver.h"
#include "ModbusAP.h"



/*----------------------------------------------------------Client----------------------------------------------------*/

int TID = 0;

int sendModbusRequest(char *servAddr, uint16_t port, uint8_t *APDU, int APDUlen, uint8_t *APDU_r){
    //PDU
    TID++;
    uint8_t PDU[PDU_SIZE];
    PDU[0] = MSB(TID);
    PDU[1] = LSB(TID);

    //protocol id
    PDU[2] = 0;
    PDU[3] = 0;

    //length
    uint16_t len = APDUlen + 1;
    PDU[4] = MSB(len);
    PDU[5] = LSB(len);
    
    //unit id
    PDU[6] = UNIT_ID;

    memcpy(PDU + 7, APDU, APDUlen);

    int mySocket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv;
    socklen_t addlen = sizeof(serv);
    serv.sin_family = PF_INET;
    serv.sin_port = htons(port);
    inet_aton(servAddr, &serv.sin_addr);
    len = connect(mySocket, (struct sockaddr*) &serv, addlen);
    len = write(mySocket, PDU, APDUlen + 7);
    len = read(mySocket, PDU, PDU_SIZE);
    if(len < 7)
        return -1;
    if(PDU[0] != MSB(TID) || PDU[1] != LSB(TID)){
        printf("PDU[0] = %d\t PDU[1] = %d\n", PDU[0], PDU[1]);
        return -2;
    }
    if(PDU[2] != 0 || PDU[3] != 0) 
        return -3;
    len = len - 6; 
    if(PDU[4] != MSB(len) || PDU[5] != LSB(len)) 
        return -4;
    if(PDU[6] != UNIT_ID) 
        return -5;
    memcpy(APDU_r, PDU + 7, len - 1);
    return len - 1;
}

/*----------------------------------------------------------Server----------------------------------------------------*/

int commsSocket;

int receiveModbusRequest(int serverSocket, uint8_t *APDU, int APDUlen){
    struct sockaddr_in remote;
    int addlen = sizeof(remote);
    commsSocket = accept(serverSocket, (struct sockaddr *)&remote, &addlen);
    uint8_t PDU_MBAP[PDU_SIZE];
    int len = read(commsSocket, PDU_MBAP, 7);
    if(len != 7) return -1;
    uint8_t transactID;
    transactID = concatenate(PDU_MBAP[0], PDU_MBAP[1]);
    APDUlen = concatenate(PDU_MBAP[4], PDU_MBAP[5]);
    APDUlen -=1;
    len = read(commsSocket, APDU, APDUlen);
    if(len == -1) return -1;
    return transactID;
}

int sendModbusResponse(int trasactID, uint8_t *APDU_r, int APDU_rlen){
    uint8_t PDU[PDU_SIZE];
    //assemble MBAP
    PDU[0] = MSB(trasactID);
    PDU[1] = LSB(trasactID);
    PDU[2] = 0;
    PDU[3] = 0;
    APDU_rlen+=1;
    PDU[4] = MSB(APDU_rlen);
    PDU[5] = LSB(APDU_rlen);
    PDU[6] = UNIT_ID;
    APDU_rlen-=1;
    memcpy(PDU + 7, APDU_r, APDU_rlen);
    int len = write(commsSocket, PDU, 7 + APDU_rlen);
    if(len != 7 + APDU_rlen) return -1;
    return 0;
}