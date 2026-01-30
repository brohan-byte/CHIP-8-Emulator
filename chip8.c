include <stdio.h>
#include <stdint.h>
#define START_ADDRESS 0x200
#define FONTSET_SIZE 80
#define FONTSET_START 0x50
#include <time.h>
#include <string.h>
#include "platform.h"
#define VIDEO_WIDTH  64
#define VIDEO_HEIGHT 32

static const uint8_t fontset[FONTSET_SIZE] = {
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

typedef struct {
	  uint16_t pc;
      uint8_t registers[16];
      uint8_t memory[4096];
      uint8_t delayTimer;
	  uint8_t soundTimer;
	  uint16_t stack[16];
      uint8_t sp;
      uint16_t index;
      uint8_t keypad[16];
      uint32_t video[64 * 32];
      uint16_t opcode;
	  unsigned int rng_state;
}chip8;



void chip8_init(chip8 *chip)
{
	memset(chip, 0, sizeof(*chip));
    chip->pc = START_ADDRESS;
	for(int i = 0;i<FONTSET_SIZE;i++){
		chip->memory[FONTSET_START+i] = fontset[i];
		
	}
	chip->rng_state = (unsigned int)time(NULL);

}

uint8_t chip8_random_byte(chip8 *chip){
	(void)chip;
    return (uint8_t)(rand() & 0xFF);
}




void op_00E0(chip8 *chip){
	memset(chip->video, 0, sizeof(chip->video));
}

void op_00EE(chip8 *chip){
	chip->sp--;
	chip->pc = chip->stack[chip->sp];
}

void op_1nnn(chip8 *chip){
	uint16_t address = chip->opcode & 0x0FFFu;
	chip->pc = address;
}

void op_2nnn(chip8 *chip){
	uint16_t address = chip->opcode & 0x0FFFu;
	chip->stack[chip->sp] = chip->pc;
	chip->sp++;
	chip->pc = address;
}

void op_3xkk(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

	if (chip->registers[Vx] == byte)
	{
		chip->pc += 2;
	}
}


void op_4xkk(chip8 *chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

	if (chip->registers[Vx] != byte)
	{
		chip->pc += 2;
	}

}

void op_5xy0(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u)>>8u;
	uint8_t Vy = (chip->opcode & 0x00F0u)>>4u;
	if(chip->registers[Vx] == chip->registers[Vy]){
		chip->pc+=2;
	}

}

void op_6xkk(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

	chip->registers[Vx] = byte;


}

void op_7xkk(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

	chip->registers[Vx] += byte;

	
}

void op_8xy0(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u)>>4u;
	chip->registers[Vx] = chip->registers[Vy];

}


void op_8xy1(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u)>>4u;
	chip->registers[Vx] |= chip->registers[Vy];
	
}

void op_8xy2(chip8 *chip){
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >>4u;
	chip->registers[Vx] &= chip->registers[Vy];
}

void op_8xy3(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	chip->registers[Vx] ^= chip->registers[Vy];
}

void op_8xy5(chip8 *chip){

	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	if (chip->registers[Vx] > chip->registers[Vy])
	{
		chip->registers[0xF] = 1;
	}
	else
	{
		chip->registers[0xF] = 0;
	}

	chip->registers[Vx] -= chip->registers[Vy];
}

void op_8xy4(chip8 *chip){

	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	uint16_t sum = chip->registers[Vx] +chip->registers[Vy];

	if (sum > 255U)
	{
		chip->registers[0xF] = 1;
	}
	else
	{
		chip->registers[0xF] = 0;
	}

	chip->registers[Vx] = sum & 0xFFu;

}



void op_8xy6(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

	chip->registers[0xF] = (chip->registers[Vx] & 0x1u);

	chip->registers[Vx] >>= 1;


}

void op_8xy7(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	if (chip->registers[Vy] > chip->registers[Vx])
	{
		chip->registers[0xF] = 1;
	}
	else
	{
		chip->registers[0xF] = 0;
	}

	chip->registers[Vx] = chip->registers[Vy] - chip->registers[Vx];

}



void op_8xyE(chip8 *chip){

	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	chip->registers[0xF] = (chip->registers[Vx] & 0x80u) >> 7u;

	chip->registers[Vx] <<= 1;

}


void op_9xy0(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

	if (chip->registers[Vx] != chip->registers[Vy])
	{
		chip->pc += 2;
	}


}

void op_Annn(chip8 *chip){

	uint16_t address = chip->opcode & 0x0FFFu;

	chip->index = address;

}

void op_Bnnn(chip8 *chip){
	uint16_t address = chip->opcode & 0x0FFFu;
	chip->pc = chip->registers[0] + address;

}

void op_Cxkk(chip8 *chip){

	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t byte = chip->opcode & 0x00FFu;

	chip->registers[Vx] = chip8_random_byte(chip) & byte;


}



void op_Dxyn(chip8 *chip){


	uint8_t Vx = (chip->opcode & 0x0F00u)>>8u;
	uint8_t Vy = (chip->opcode & 0x00F0u)>>4u;
	uint8_t height = (chip->opcode) & 0x000Fu;
	uint8_t xPos = chip->registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = chip->registers[Vy] % VIDEO_HEIGHT;
	chip->registers[0xF] = 0;
	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = chip->memory[chip->index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &chip->video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			if (spritePixel)
			{
				if (*screenPixel == 0xFFFFFFFF)
				{
					chip->registers[0xF] = 1;
				}

				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}

}


void op_Ex9E(chip8 *chip){

	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

	uint8_t key = chip->registers[Vx];

	if (chip->keypad[key])
	{
		chip->pc += 2;
	}


}



void op_ExA1(chip8 *chip){

	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

	uint8_t key = chip->registers[Vx];

	if (!chip->keypad[key])
	{
		chip->pc += 2;
	}


}

void op_Fx07(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

	chip->registers[Vx] = chip->delayTimer;

}

void op_Fx0A(chip8 *chip)
{
    uint8_t Vx = (uint8_t)((chip->opcode & 0x0F00u) >> 8u);

    if (chip->keypad[0]) {
        chip->registers[Vx] = 0;
    } else if (chip->keypad[1]) {
        chip->registers[Vx] = 1;
    } else if (chip->keypad[2]) {
        chip->registers[Vx] = 2;
    } else if (chip->keypad[3]) {
        chip->registers[Vx] = 3;
    } else if (chip->keypad[4]) {
        chip->registers[Vx] = 4;
    } else if (chip->keypad[5]) {
        chip->registers[Vx] = 5;
    } else if (chip->keypad[6]) {
        chip->registers[Vx] = 6;
    } else if (chip->keypad[7]) {
        chip->registers[Vx] = 7;
    } else if (chip->keypad[8]) {
        chip->registers[Vx] = 8;
    } else if (chip->keypad[9]) {
        chip->registers[Vx] = 9;
    } else if (chip->keypad[10]) {
        chip->registers[Vx] = 10;
    } else if (chip->keypad[11]) {
        chip->registers[Vx] = 11;
    } else if (chip->keypad[12]) {
        chip->registers[Vx] = 12;
    } else if (chip->keypad[13]) {
        chip->registers[Vx] = 13;
    } else if (chip->keypad[14]) {
        chip->registers[Vx] = 14;
    } else if (chip->keypad[15]) {
        chip->registers[Vx] = 15;
    } else {
        // No key pressed: repeat this instruction next cycle
        chip->pc -= 2;
    }
}

void op_Fx15(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

	chip->delayTimer = chip->registers[Vx];


}

void op_Fx18(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	chip->soundTimer = chip->registers[Vx];

}

void op_Fx1E(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

	chip->index += chip->registers[Vx];

}

void op_Fx29(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t digit = chip->registers[Vx];

	chip->index = FONTSET_START + (5 * digit);

}


void op_Fx33(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	uint8_t value = chip->registers[Vx];

	chip->memory[chip->index + 2] = value % 10;
	value /= 10;

	chip->memory[chip->index + 1] = value % 10;
	value /= 10;

	chip->memory[chip->index] = value % 10;

	

}

void op_Fx55(chip8 *chip){
	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; ++i)
	{
		chip->memory[chip->index + i] = chip->registers[i];
	}

}

void op_Fx65(chip8 *chip){

	uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		chip->registers[i] = chip->memory[chip->index + i];
	}

}
void op_NULL(chip8 *chip)
{
    (void)chip;  // unused
}
void chip8_execute(chip8 *chip)
{
    switch (chip->opcode & 0xF000u)
    {
    case 0x0000:
        switch (chip->opcode & 0x00FFu)
        {
        case 0x00E0: op_00E0(chip); break;
        case 0x00EE: op_00EE(chip); break;
        default:     op_NULL(chip); break;
        }
        break;

    case 0x1000: op_1nnn(chip); break;
    case 0x2000: op_2nnn(chip); break;
    case 0x3000: op_3xkk(chip); break;
    case 0x4000: op_4xkk(chip); break;
    case 0x5000: op_5xy0(chip); break;
    case 0x6000: op_6xkk(chip); break;
    case 0x7000: op_7xkk(chip); break;

    case 0x8000:
        switch (chip->opcode & 0x000Fu)
        {
        case 0x0: op_8xy0(chip); break;
        case 0x1: op_8xy1(chip); break;
        case 0x2: op_8xy2(chip); break;
        case 0x3: op_8xy3(chip); break;
        case 0x4: op_8xy4(chip); break;
        case 0x5: op_8xy5(chip); break;
        case 0x6: op_8xy6(chip); break;
        case 0x7: op_8xy7(chip); break;
        case 0xE: op_8xyE(chip); break;
        default:  op_NULL(chip); break;
        }
        break;

    case 0x9000: op_9xy0(chip); break;
    case 0xA000: op_Annn(chip); break;
    case 0xB000: op_Bnnn(chip); break;
    case 0xC000: op_Cxkk(chip); break;
    case 0xD000: op_Dxyn(chip); break;

    case 0xE000:
        switch (chip->opcode & 0x00FFu)
        {
        case 0x9E: op_Ex9E(chip); break;
        case 0xA1: op_ExA1(chip); break;
        default:   op_NULL(chip); break;
        }
        break;

    case 0xF000:
        switch (chip->opcode & 0x00FFu)
        {
        case 0x07: op_Fx07(chip); break;
        case 0x0A: op_Fx0A(chip); break;
        case 0x15: op_Fx15(chip); break;
        case 0x18: op_Fx18(chip); break;
        case 0x1E: op_Fx1E(chip); break;
        case 0x29: op_Fx29(chip); break;
        case 0x33: op_Fx33(chip); break;
        case 0x55: op_Fx55(chip); break;
        case 0x65: op_Fx65(chip); break;
        default:   op_NULL(chip); break;
        }
        break;

    default:
        op_NULL(chip);
        break;
    }
}


void chip_cycle(chip8 *chip){

	chip->opcode = (chip->memory[chip->pc] << 8u) | chip->memory[chip->pc + 1];
	chip->pc+=2;
	chip8_execute(chip);
	if(chip->delayTimer>0){
		chip->delayTimer--;
	}

	if(chip->soundTimer>0){
		chip->soundTimer--;
	}


}









void chip8_load_rom(chip8 *chip, const char* filename){
 	FILE *fptr = fopen(filename, "rb");
    if (!fptr) {
        perror("fopen");
        return;
    }

    if (fseek(fptr, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(fptr);
        return;
    }

    long sz_long = ftell(fptr);
    if (sz_long < 0) {
        perror("ftell");
        fclose(fptr);
        return;
    }

    size_t size = (size_t)sz_long;
	if (START_ADDRESS + size > sizeof(chip->memory)) {
    	fprintf(stderr, "ROM too large: %zu bytes\n", size);
    	fclose(fptr);
    	return;
	}

	if (fseek(fptr, 0, SEEK_SET) != 0) {
        perror("fseek");
        fclose(fptr);
        return;
    }

    uint8_t *buffer = (uint8_t *)malloc(size);
	if (!buffer) {
        perror("malloc");
        fclose(fptr);
        return;
    }

    size_t nread = fread(buffer, 1, size, fptr);
	if (nread != size) {
    	fprintf(stderr, "ROM read failed (read %zu of %zu bytes)\n", nread, size);
        free(buffer);
		return;
    }

    fclose(fptr);


    for (size_t i = 0; i < size; i++) {
        chip->memory[START_ADDRESS + i] = buffer[i];
    }

    free(buffer);
    return;

}





int main(int argc, char** argv){

    if(argc!=4){
        fprintf(stderr,
                "Usage: %s <Scale> <Delay> <ROM>\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    int videoScale = atoi(argv[1]);
    uint32_t cycleDelay = (uint32_t)atoi(argv[2]);
    const char* filename = argv[3];
    chip8 chip = {0};
    chip8_init(&chip);
    chip8_load_rom(&chip, filename);
	printf("ROM @0x200: %02X %02X %02X %02X\n",
       chip.memory[0x200], chip.memory[0x201],
       chip.memory[0x202], chip.memory[0x203]);

	printf("ROM file: %s\n", filename);
	Platform p;
    if (!platform_init(&p,
                   "CHIP-8 Emulator",
                   VIDEO_WIDTH * videoScale,
                   VIDEO_HEIGHT * videoScale,
                   VIDEO_WIDTH,
                   VIDEO_HEIGHT)) {
    fprintf(stderr, "Platform initialization failed\n");
    return EXIT_FAILURE;
    }

    int videoPitch = sizeof(chip.video[0])*VIDEO_WIDTH;
    

	for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++) {
        chip.video[i] = 0xFFFFFFFFu;
    }
    platform_update(&p, chip.video, videoPitch);
    SDL_Delay(1000);

    /* clear back to black so you can see real drawing afterward */
    for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; i++) {
        chip.video[i] = 0x00000000u;
    }
    platform_update(&p, chip.video, videoPitch);


	uint32_t lastCycleTime = SDL_GetTicks();
    bool quit = false;

    while(!quit){
        quit = platform_process_input(&p, chip.keypad);
        uint32_t currentTime = SDL_GetTicks();
        uint32_t dt = currentTime - lastCycleTime;
        if(dt>cycleDelay){
            lastCycleTime = currentTime;
			chip_cycle(&chip);
			static int step = 0;
			if ((step++ % 50) == 0) {
    			printf("PC=0x%03X OPC=0x%04X I=0x%03X\n", chip.pc, chip.opcode, chip.index);
			}
            platform_update(&p, chip.video, videoPitch);
        }

    }
	platform_destroy(&p);
    return 0;

}



