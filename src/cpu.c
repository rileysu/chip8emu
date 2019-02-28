#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "arrlist.h"
#include "cpu.h"

const uint8_t charsprites[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

CPU initCPU(){
	CPU cpu = malloc(sizeof(struct CPU_t));
	//Reset so cpu starts from scratch
	resetCPU(cpu);
	return cpu;
}

void freeCPU(CPU cpu){
	free(cpu);
}

void resetCPU(CPU cpu){
	//Blank memory and registers
	memset(cpu->screen, 0, sizeof(cpu->screen));
	memset(cpu->ram, 0, sizeof(cpu->ram));
	for (int i = 0; i < sizeof(charsprites); i++)
	       cpu->ram[i] = charsprites[i];	
	memset(cpu->stack, 0, sizeof(cpu->stack));
	memset(cpu->v, 0, sizeof(cpu->v));
	cpu->i = 0;
	cpu->dt = 0;
	cpu->st = 0;
	//Init pc to start of program as per spec
	cpu->pc = 0x200;
	cpu->sp = 0;
	//Set cpu ticks to current ticks
	cpu->ticks = SDL_GetTicks();
	//Set screen to update on first cycle
	cpu->uscreen = 1;	
}

void loadCPU(ArrList l, CPU cpu){
	uint8_t *rp = &(cpu->ram[0x200]);
	forArrList(i,l){
		*(rp++) = *(uint8_t*)i;
	}
}

void stepCPU(CPU cpu){
	uint16_t inst = cpu->ram[cpu->pc] << 8 | cpu->ram[cpu->pc + 1];
	uint8_t vx;
	uint8_t vy;
	uint8_t n;

	switch (inst & 0xf000){
		case 0x0000:
			switch (inst & 0x00ff){
				case 0x00e0: //0x00E0 Clears the display
					for (int y = 0; y < SCREEN_HEIGHT; y++){
						for (int x = 0; x < SCREEN_WIDTH; x++){
							cpu->screen[x][y] = 0;
						}
					}
					cpu->pc += 2;	
					break;

				case 0x00ee: //0x00EE Return from a subroutine
					cpu->pc = cpu->stack[--(cpu->sp)] + 2;
					break;

				default:
					fprintf(stderr, "Unknown instruction: %04x\n", inst);
					break;
			}

			break;

		case 0x1000: //0x1nnn Jump to location nnn
			cpu->pc = inst & 0x0fff;
			break;

		case 0x2000: //0x2nnn Call subroutine at nnn
			cpu->stack[cpu->sp++] = cpu->pc;
			cpu->pc = inst & 0x0fff;
			break;

		case 0x3000: //0x3xkk Skip next instruction if Vx = kk 
			if (cpu->v[(inst & 0x0f00) >> 8] == (inst & 0x00ff)){
				cpu->pc += 4;
			} else {
				cpu->pc += 2;
			}
			break;

		case 0x4000: //0x4xkk Skip next instruction if Vx != kk	
			if (cpu->v[(inst & 0x0f00) >> 8] != (inst & 0x00ff)){
				cpu->pc += 4;
			} else {
				cpu->pc += 2;
			}
			break;

		case 0x5000: //0x5xy0 Skip next instruction if Vx = Vy	
			if (cpu->v[(inst & 0x0f00) >> 8] == cpu->v[(inst & 0x00f0) >> 4]){
				cpu->pc += 4;
			} else {
				cpu->pc += 2;
			}
			break;

		case 0x6000: //0x6xkk Set Vx = kk
			cpu->v[(inst & 0x0f00) >> 8] = (inst & 0x00ff);
			cpu->pc += 2;
			break;

		case 0x7000: //0x7xkk Set Vx = Vx + kk	
			cpu->v[(inst & 0x0f00) >> 8] += (inst & 0x00ff);
			cpu->pc += 2;
			break;

		case 0x8000:
			
			switch (inst & 0x000f){
				case 0x0000: //0x8xy0 Set Vx = Vy
					cpu->v[(inst & 0x0f00) >> 8] = cpu->v[(inst & 0x00f0) >> 4];
					cpu->pc += 2;
					break;
				
				case 0x0001: //0x8xy1 Set Vx = Vx | Vy
					cpu->v[(inst & 0x0f00) >> 8] |= cpu->v[(inst & 0x00f0) >> 4];
					cpu->pc += 2;
					break;

				case 0x0002: //0x8xy2 Set Vx = Vx & Vy
					cpu->v[(inst & 0x0f00) >> 8] &= cpu->v[(inst & 0x00f0) >> 4];
					cpu->pc += 2;
					break;

				case 0x0003: //0x8xy3 Set Vx = Vx ^ Vy
					cpu->v[(inst & 0x0f00) >> 8] ^= cpu->v[(inst & 0x00f0) >> 4];
					cpu->pc += 2;
					break;

				case 0x0004: //0x8xy4 Set Vx = Vx + Vy, set Vf carry
					cpu->v[15] = ((int)cpu->v[(inst & 0x0f00) >> 8] + cpu->v[(inst & 0x00f0) >> 4] > 256);
					cpu->v[(inst & 0x0f00) >> 8] += cpu->v[(inst & 0x00f0) >> 4];
					cpu->pc += 2;
					break;

				case 0x0005: //0x8xy5 Set Vx = Vx - Vy, set Vf = NOT borrow
					cpu->v[15] = ((int)cpu->v[(inst & 0x0f00) >> 8] - cpu->v[(inst & 0x00f0) >> 4] < 0);
					cpu->v[(inst & 0x0f00) >> 8] -= cpu->v[(inst & 0x00f0) >> 4];
					cpu->pc += 2;
					break;

				case 0x0006: //0x8xy6 Set Vx = Vy >> 1 Set Vf = underflow
					cpu->v[15] = cpu->v[(inst & 0x0f00) >> 8] & 0x01;
					cpu->v[(inst & 0x0f00) >> 8] = cpu->v[(inst & 0x00f0) >> 4] >> 1;
					cpu->pc += 2;
					break;

				case 0x0007: //0x8xy7 Set Vx = Vy - Vx Set Vf = NOT borrow
					cpu->v[15] = ((int)cpu->v[(inst & 0x00f0) >> 4] - cpu->v[(inst & 0x0f00) >> 8] < 0);
					cpu->v[(inst & 0x0f00) >> 8] = cpu->v[(inst & 0x00f0) >> 4] - cpu->v[(inst & 0x0f00) >> 8];
					cpu->pc += 2;
					break;

				case 0x000e: //0x8xye Set Vx = Vy = Vy << 1 Set Vf = overflow
					cpu->v[15] = cpu->v[(inst & 0x0f00) >> 8] >> 0x07;
					cpu->v[(inst & 0x0f00) >> 8] = cpu->v[(inst & 0x00f0) >> 4] << 1;
					cpu->pc += 2;
					break;

				default:
					fprintf(stderr, "Unknown instruction %04x\n", inst);
					break;
			}

			break;

		case 0x9000: //0x9xy0 Skip next instruction if Vx != Vy	
			if (cpu->v[(inst & 0x0f00) >> 8] != cpu->v[(inst & 0x00f0) >> 4]){
				cpu->pc += 4;
			} else {
				cpu->pc += 2;
			}
			break;

		case 0xa000: //0xannn Set I = nnn
			cpu->i = (inst & 0x0fff);
			cpu->pc += 2;
			break;

		case 0xb000: //0xbnnn Jump to location nnn + V0
			cpu->pc = (inst & 0x0fff) + cpu->v[0];
			break;

		case 0xc000: //0xcxkk Set Vx = random byte & kk
			cpu->v[(inst & 0x0f00) >> 8] = rand() & (inst & 0x00ff);
			cpu->pc += 2;
			break;

		case 0xd000: //0xdxyn Display n-byte sprite starting at memory location I at (Vx, Vy), set VF  = collision
			vx = cpu->v[(inst & 0x0f00) >> 8];
			vy = cpu->v[(inst & 0x00f0) >> 4];
			n = inst & 0x000f;
			cpu->v[15] = 0;

			for (int y = 0; y < n; y++){
				uint8_t pstring = cpu->ram[cpu->i + y];
				for (int x = 0; x < sizeof(uint8_t) * 8; x++){
					if (pstring & (0x80 >> x)){
						if (cpu->screen[x+vx][y+vy])
							cpu->v[15] = 1;
						cpu->screen[x+vx][y+vy] ^= 1;
					}
				}
			}

			//Screen should update
			cpu->uscreen = 1;
			cpu->pc += 2;
			break;

		case 0xe000:

			switch (inst & 0x000f){
				
				case 0x000e: //0xex9e Skip next instruction if key with the value of Vx is pressed
					//TODO
					cpu->pc += 2;
					break;

				case 0x0001: //0xexa1 Skip next instruction if key with the value of Vx is not pressed
					//TODO
					cpu->pc += 4;
					break;
			}

			break;

		case 0xf000:

			switch (inst & 0x00ff){
				
				case 0x0007: //0xfx07 Set Vx = delay timer value
						cpu->v[(inst & 0x0f00) >> 8] = cpu->dt;
						cpu->pc += 2;
					break;

				case 0x000a: //0xfx0a Wait for a key press, store the value of the key in Vx
						//TODO
					break;

				case 0x0015: //0xfx15 Set delay timer = Vx
						cpu->dt = cpu->v[(inst & 0x0f00) >> 8];
						cpu->pc += 2;
					break;

				case 0x0018: //0xfx18 Set sound timer = Vx	
						cpu->st = cpu->v[(inst & 0x0f00) >> 8];
						cpu->pc += 2;
					break;

				case 0x001e: //0xfx1e Set I = I + Vx
						if ((cpu->i + cpu->v[(inst & 0x0f00) >> 8]) > 0xfff)
							cpu->v[15] = 1;
						else
							cpu->v[15] = 0;
						cpu->i += cpu->v[(inst & 0x0f00) >> 8];
						cpu->pc += 2;
					break;

				case 0x0029: //0xfx29 Set I = location of sprite for digit Vx
						cpu->i = cpu->v[(inst & 0x0f00) >> 8] * 5;
						cpu->pc += 2;
					break;
				
				case 0x0033: //0xfx33 Store BCD representation of Vx in memory locations I, I+1, I+2
						cpu->ram[cpu->i] = cpu->v[(inst & 0x0f00) >> 8] / 100;
						cpu->ram[cpu->i+1] = (cpu->v[(inst & 0x0f00) >> 8] / 10) % 10;
						cpu->ram[cpu->i+2] = (cpu->v[(inst & 0x0f00) >> 8] % 100) % 10;
						cpu->pc += 2;
					break;
				
				case 0x0055: //0xfx55 Store registers V0 through Vx in memory starting at location I
						for (int i = 0; i <= ((inst & 0x0f00) >> 8); i++)
							cpu->ram[cpu->i + i] = cpu->v[i];
						cpu->i += ((inst & 0x0f00) >> 8) + 1;
						cpu->pc += 2;
					break;

				case 0x0065: //0xfx65 Read registers V0 through Vx from memory starting at location I
						for (int i = 0; i <= ((inst & 0x0f00) >> 8); i++)
							cpu->v[i] = cpu->ram[cpu->i + i];
						cpu->i += ((inst & 0x0f00) >> 8) + 1;
						cpu->pc += 2;
					break;

				default:
					fprintf(stderr, "Unknown instruction %04x\n", inst);
					break;
			}

			break;

		default:
			fprintf(stderr, "Unknown instruction: %04x\n", inst);
			break;
	}

	int currTicks = SDL_GetTicks();
	int diff = (currTicks - cpu->ticks) / 17;

	if (cpu->dt > 0)
		if (cpu->dt > diff)
			cpu->dt -= diff;
		else 
			cpu->dt = 0;
	
	if (cpu->st > 0)
		if (cpu->st > diff)
			cpu->st -= diff;
		else 
			cpu->st = 0;

	if (diff >= 1)	
		cpu->ticks = SDL_GetTicks();
}

void printDiagsCPU(CPU cpu){
	for (int i = 0; i < 30; i++)
		printf("-");
	printf("\n");

	printf("stack:");
	for (int i = 0; i < 16; i++)
		printf(" %04x", cpu->stack[i]);
	printf("\n");
	
	printf("registers:");
	for (int i = 0; i < 16; i++)
		printf(" %02x", cpu->v[i]);
	printf("\n");

	printf("I: %04x\n", cpu->i);

	printf("dt: %02x\n", cpu->dt);

	printf("st: %02x\n", cpu->st);
	
	printf("pc: %04x\n", cpu->pc);
	
	printf("sp: %04x\n", cpu->sp);
	
	for (int i = 0; i < 30; i++)
		printf("-");
	printf("\n");
}

void printDiagsScreenCPU(CPU cpu){
	for (int y = 0; y < SCREEN_HEIGHT; y++){
		for (int x = 0; x < SCREEN_WIDTH; x++){
			if (cpu->screen[x][y])
				printf("#");
			else
				printf(".");
		}
		printf("\n");
	}
}

void printDiagsMemoryCPU(CPU cpu){
	for (int i = 0; i < 30; i++)
		printf("-");
	printf("\n");
	
	for (int i = 0; i < 4096; i++){
		printf("%04x: %02x\n", i, cpu->ram[i]);
	}
	
	for (int i = 0; i < 30; i++)
		printf("-");
	printf("\n");
}
