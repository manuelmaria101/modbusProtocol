#include <stdio.h>

/*returns number of registers written
startAddr: first address to be written
nReg: number of registers to be written
data: values for registers*/
int holdingRegisters_W(uint16_t startAddr, uint16_t nReg, uint16_t *data);

/*returns number of registers read
startAddr: first address to be read
nReg: number of registers to be read
data: value of registers read*/
int holdingRegisters_R(uint16_t startAddr, uint16_t nReg, uint16_t *data);


