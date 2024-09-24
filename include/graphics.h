#include <stdbool.h>
#ifndef GRAPHICS_H
#define GRAPHICS_H
#define SIZE_M 4
//  Estrutura para armazenar as coordenadas de canto superior esquerdo de cada bloco
typedef struct
{
    int top_left_point_x; // Coordenada x do canto superior esquerdo do bloco
    int top_left_point_y; // Coordenada y do canto superior esquerdo do bloco
    int filled;           // Se o espaço de bloco está preenchido é 1, se não é 0
    unsigned color;       // Cor do bloco armazenado no espaço, se houver

} Block_space;

// Estrutura para encapsular a matriz 4x4 que representa uma peça
typedef struct
{
    Block_space blocks_coordinates[4][4];
} Tetris_Piece;

// Protótipos das funções do jogo Tetris
//void fill_matrix(Block_space matrix[24][10]);                                                           // Preenche a matriz com blocos
//void print_matrix(Block_space matrix[24][10]);                                                          // Imprime e desenha a matriz
//void gen_line(int *, int *, int *, int *, unsigned *);                                                  // Gera uma linha aleatória na tela
void gen_barrel(Block_space matrix[24][10]);                                                            // Gera o barril central
void gen_block(int, int, int, int, unsigned);                                                           // Gera um bloco na tela
//Tetris_Piece gen_piece(Block_space matrix[24][10]);                                                     // Gera uma peça na tela
Tetris_Piece print_piece(int piece_set[4][4], Block_space matrix[24][10]);                              // Imprime uma peça aleatória de cor aleatória em um local aleatório do início da tela
Tetris_Piece push_piece_down(Tetris_Piece piece_To_Be_Pushed, Block_space matrix[24][10]);              // Empurra peça de tetris para baixo
Tetris_Piece push_piece_sides(bool right, Tetris_Piece piece_To_Be_Pushed, Block_space matrix[24][10]); // Empurra peça de tetris para a direita
//int clear_full_lines(Block_space matrix[24][10]);
// Tetris_Piece push_piece_left(Tetris_Piece piece_To_Be_Pushed, Block_space matrix[24][10]);  // Empurra peça de tetris para a esquerda

// Desenhar pontuação
void draw_letter_P(int start_x, int start_y, int block_size, short color);
void draw_letter_O(int start_x, int start_y, int block_size, short color);
void draw_letter_I(int start_x, int start_y, int block_size, short color);
void draw_letter_N(int start_x, int start_y, int block_size, short color);
void draw_letter_T(int start_x, int start_y, int block_size, short color);
void draw_letter_S(int start_x, int start_y, int block_size, short color);
void draw_colon(int start_x, int start_y, int block_size, short color);
void draw_digit_0(int start_x, int start_y, int block_size, short color);
void draw_digit_1(int start_x, int start_y, int block_size, short color);
void draw_digit_2(int start_x, int start_y, int block_size, short color);
void draw_digit_3(int start_x, int start_y, int block_size, short color);
void draw_digit_4(int start_x, int start_y, int block_size, short color);
void draw_digit_5(int start_x, int start_y, int block_size, short color);
void draw_digit_6(int start_x, int start_y, int block_size, short color);
void draw_digit_7(int start_x, int start_y, int block_size, short color);
void draw_digit_8(int start_x, int start_y, int block_size, short color);
void draw_digit_9(int start_x, int start_y, int block_size, short color);
void draw_digit(int digit, int start_x, int start_y, int block_size, short color);
void draw_letter_P_italic(int start_x, int start_y, int block_size, short color);
void draw_letter_A_italic(int start_x, int start_y, int block_size, short color);
void draw_letter_U_italic(int start_x, int start_y, int block_size, short color);
void draw_letter_S_italic(int start_x, int start_y, int block_size, short color);
void draw_letter_E_italic(int start_x, int start_y, int block_size, short color);
void draw_word_PAUSE_();
void draw_letter_G(int start_x, int start_y, int block_size, short color);
void draw_letter_A(int start_x, int start_y, int block_size, short color);
void draw_letter_M(int start_x, int start_y, int block_size, short color);
void draw_letter_E(int start_x, int start_y, int block_size, short color);
void draw_letter_V(int start_x, int start_y, int block_size, short color);
void draw_letter_R(int start_x, int start_y, int block_size, short color);
void draw_letter_exclamation(int start_x, int start_y, int block_size, short color);
void draw_word_GAME_OVER();
//bool check_game_over(Block_space matrix[24][10]);

int int_pow(int base, int exponent);
void draw_points_word_with_number(int points);
void render_game(Block_space matrix[24][10], int score);

#endif


