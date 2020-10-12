#include <stdio.h>

/*------------------------------------------------------------Client------------------------------------------------------------*/
/*returns lenght of APDU response 
servAddr: server address
port: port
APDU: apllication data unit
APDUlen: length of APDU
APDU_r: APDU response*/
int sendModbusRequest(char *servAddr, uint16_t port, uint8_t *APDU, int APDUlen, uint8_t *APDU_r);

/*------------------------------------------------------------Server------------------------------------------------------------*/
/*return TI(transaction ID)
serverSocket: server socket
APDU: returns received APDU
APDUlen: length of APDU */
int receiveModbusRequest(int serverSocket, uint8_t *APDU, int APDUlen);

/*returns 0 if ok
transactID: transaction ID
APDU_r: APDU_response to be sent
APDU_rlen: APDU_responde lenght*/
int sendModbusResponse(int trasactID, uint8_t *APDU_r, int APDU_rlen);