#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H


// Variáveis globais usadas para controlar o estado atual da peça
extern int current_piece_pos_i; // Posição da peça atual no eixo Y (vertical)
extern int current_piece_pos_j; // Posição da peça atual no eixo X (horizontal)
extern int screen_x, screen_y, block_side, char_x, char_y, total_score, lines_cleared, colision;
extern volatile sig_atomic_t game_over;
extern Tetris_Piece generated_Piece; // Peça que está caindo

// Struct for Tetris pieces
//typedef struct {
//    Block_space blocks_coordinates[4][4];
//} Tetris_Piece;

void fill_matrix(Block_space matrix[24][10]);
Tetris_Piece gen_piece(Block_space matrix[24][10]);
int clear_full_lines(Block_space matrix[24][10]);
bool check_game_over(Block_space matrix[24][10]);
void update_game_logic(Block_space matrix[24][10]);


#endif



