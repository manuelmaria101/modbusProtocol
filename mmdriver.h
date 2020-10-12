#include <stdio.h>

/*returns number of registers written
startAddr: first address to be written
nReg: number of registers to be written
data: values for registers*/
int holdingRegisters_W(int startAddr, int nReg, uint8_t *data);

/*returns number of registers read
startAddr: first address to be read
nReg: number of registers to be read
data: value of registers read*/
int holdingRegisters_R(int startAddr, int nReg, uint8_t *data);


