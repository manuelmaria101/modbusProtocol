#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "ModbusAP.h"
#include "mmdriver.h"


int serverConnect(char *serverAddr, int port)
{
    int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1) return -1;
    struct sockaddr_in local, remote;
    socklen_t addlen = sizeof(local);
    char buf[PDU_SIZE];
    local.sin_family = PF_INET;
    local.sin_port = htons(port);
    inet_aton(serverAddr, &local.sin_addr);
    bind(serverSocket, (struct sockaddr *) &local, addlen); //gives local socket an address
    listen(serverSocket, 10);
    return serverSocket;
}

int serverDisconnect(int serverSocket){
    return close(serverSocket);
}

int main(){
    int sock = serverConnect("127.0.0.1", 502);
    uint8_t operation, data[PDU_SIZE];
    int startAddr, nReg, check;
    while(1){
        int transactId = getModbusRequest(sock, &operation, &startAddr, &nReg, data);
        printf("\nTransaction ID: %d\n\n", transactId);
        switch(operation){
            case WRITE_MULTIPLE_REG: {
                printf("\treceived data WRITE\n");
                printf("\t\tst_ad = %d\tnr = %d\t", startAddr, nReg);
                printf("data: [%d%d][%d%d][%d%d]\n", data[0],data[1],data[2],data[3],data[4],data[5]);
                nReg = holdingRegisters_W(startAddr, nReg, data);
                check = sendAPResponse(transactId, operation, startAddr, nReg, data);
            }break;
            case READ_HOLDING_REG:{
                printf("\treceived data READ\n");
                printf("\t\tst_ad = %d\tnr = %d\n", startAddr, nReg);
                nReg = holdingRegisters_R(startAddr, nReg, data);
                check = sendAPResponse(transactId, operation, startAddr, nReg, data);
            }break;
            default: break;

            if(check == 0) 
                printf("\nresponse sent sucessfully\n");
            else 
                printf("\nerror\n");
        }
    }
    sock = serverDisconnect(sock);
}

