
#include <stdint.h>
#include <signal.h>
#include "../include/graphics.h"
#ifndef INPUT_H
#define INPUT_H


#define ADXL345_DEVID 0x00
#define ADXL345_BW_RATE 0x2C
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_THRESH_ACT 0x24
#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27
#define ADXL345_INT_ENABLE 0x2E
#define ADXL345_INT_SOURCE 0x30
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_ACTIVITY 0x10
#define ADXL345_DATAREADY 0x80

// Define as chaves específicas
#define KEY1 0b1000 // Primeiro botão (pausa)
#define KEY2 0b0100 // Segundo botão (reiniciar)
#define KEY3 0b0010 // Terceiro botão (sair)

extern volatile sig_atomic_t pause_;
extern volatile sig_atomic_t stop;

// Protótipos das funções do Acelerômetro
int open_physical(int);
void *map_physical(int, unsigned int, unsigned int);
void close_physical(int);
int unmap_physical(void *, unsigned int);
void mux_init();
int I2C0_Init();
void ADXL345_REG_READ(uint8_t, uint8_t *);
void ADXL345_REG_WRITE(uint8_t, uint8_t);
void ADXL345_REG_MULTI_READ(uint8_t address, uint8_t values[], uint8_t len);
int ADXL345_init(uint8_t devid);
int ADXL345_IsDataReady();
void ADXL345_XYZ_Read(int16_t *);
void ADXL345_IdRead(uint8_t *pId);
int I2C0_onoff(unsigned int onoff);

void handle_input(Tetris_Piece* current_piece, Block_space matrix[24][10]);

#endif



