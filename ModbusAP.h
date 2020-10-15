#include <stdio.h>

#define NUMBEROFREG 9
#define READ_HOLDING_REG 0x3
#define WRITE_MULTIPLE_REG 0x10
#define END_COMMUNICATION 0xFF
#define PDU_SIZE 2048
#define UNIT_ID 1
#define MSB(xx)  ((xx)/256)
#define LSB(xx)  ((xx) % 256)
/*-------------------------------------------------------Client--------------------------------------------------------*/
/*returns number of registers read
servAddr: server address
port: port
startAddr: first address to be read
nReg: number of registers to be read
data: value of registers*/
int ReadHoldingRegisters(char *servAddr, uint16_t port, uint16_t startAddr, uint16_t nReg, uint8_t *data);

/*returns number of registers written
servAddr: server address
port: port
startAddr: first address to be read
nReg: number of registers to be read
data: value of registers to be written*/
int WriteMultipleRegisters(char *servAddr, uint16_t port, uint16_t startAddr, uint16_t nReg, uint8_t *data);

/*-------------------------------------------------------Server--------------------------------------------------------*/

/*returns transaction ID
serverSocket: serverSocket
operation: returns operation requested by client
startAddr: first address of operation
nReg: number of registers to be operated
data: only if client whats to write; it has the values for registers*/
int getModbusRequest(int serverSocket, uint8_t *operation, uint16_t *startAddr, uint16_t *nReg, uint8_t *data);

/*returns lenght of APDU_responde
transactID: transaction identifier
operation: operation to be performed
startAddr: first address of operation
nReg: number of registers to be operated
data: only if client whats to read; it has the values of holding registers*/
int sendAPResponse(int trasactID, uint8_t operation, uint16_t startAddr, uint16_t nReg, uint8_t *data);

int concatenate(int x, int y);
