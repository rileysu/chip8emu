#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "arrlist.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

typedef struct CPU_t {
        uint8_t screen[SCREEN_WIDTH][SCREEN_WIDTH]; //Screen data
        uint8_t ram[4096]; //RAM data
        uint16_t stack[16]; //Stack data
        uint8_t v[16]; //V generic registers
        uint16_t i; //I special register
        uint8_t dt; //Delay timer register
        uint8_t st; //Sound timer register
        uint16_t pc; //Program counter
        uint16_t sp; //Stack pointer
	uint32_t ticks;
	uint8_t uscreen;
} *CPU;

CPU initCPU();
void freeCPU(CPU);
void resetCPU(CPU);

void loadCPU(ArrList, CPU);
void stepCPU(CPU);

void printDiagsCPU(CPU);
void printDiagsScreenCPU(CPU);
void printDiagsMemoryCPU(CPU);
#endif
