#ifndef GRAPHICS_FPGA_H
#define GRAPHICS_FPGA_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* Barramento de dados do processador gráfico */
#define DATAA 0x80
#define DATAB 0x70
#define WRFULL 0xb0
#define WRREG 0xc0
#define SCREEN 0xa0
#define RESETPULSECOUNTER 0x90

/* Opcodes das instruções */
#define WBR 0  // 0000
#define WSM 1  // 0001
#define WBM 2  // 0010
#define DP  3  // 0011

/* Declarações das funções */
int open_men_vgafpga();
int close_men_vgafpga(int32_t fd);
int mapping_fpga();
void unmapping_fpga();
void draw_square_8x8(int position, int color);
void draw_square_16x16(int position, int color);
void erase_screen();
int is_button_pressed(void);

/* Variáveis globais */
extern size_t ALT_LWFPGASLVS_SPAN;
extern off_t ALT_LWFPGASLVS_OFST;
extern void *ALT_LWFPGASLVS_BASE_VIRTUAL;

extern volatile unsigned int *DATA_A;
extern volatile unsigned int *DATA_B;
extern volatile unsigned int *WR_FULL;
extern volatile unsigned int *WR_REG;
extern volatile unsigned int *SCREEN_;
extern volatile unsigned int *RESET_PULSECOUNTER;

#endif
