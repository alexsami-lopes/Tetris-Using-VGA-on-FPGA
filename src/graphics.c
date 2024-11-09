#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../include/graphics.h"
#include "../include/graphics_fpga.h"
#include "../include/address_map_arm.h"
//#include <intelfpgaup/video.h>

// Variáveis globais usadas para controlar o estado atual da peça
extern int current_piece_pos_i; // Posição da peça atual no eixo Y (vertical)
extern int current_piece_pos_j; // Posição da peça atual no eixo X (horizontal)
extern int screen_x, screen_y, block_side, total_score, colision;
extern int char_x, char_y;     // Coordenadas dos caracteres no display de texto

// Definição de cores em 16 bits RGB
#define video_WHITE   0xFFFF
#define video_YELLOW  0xFFE0
#define video_RED     0xF800
#define video_GREEN   0x07E0
#define video_BLUE    0x041F
#define video_CYAN    0x07FF
#define video_MAGENTA 0xF81F
#define video_GREY    0xC618
#define video_PINK    0xFC18
#define video_ORANGE  0xFC00
#define video_BLACK   0x0000

// Função para gerar o barril na tela
void gen_barrel(Block_space matrix[24][10])
{
    unsigned int barrel_color[] = {video_WHITE}; // Define a cor do barril como branco
    unsigned color = barrel_color[0];
    int border = (block_side * 0.125); // Espessura da borda
    char msg_buffer[80];

    // Parte inferior do barril
    int x1 = matrix[23][0].top_left_point_x - block_side;
    int y1 = matrix[23][0].top_left_point_y + block_side + border;
    int x2 = matrix[23][9].top_left_point_x + (block_side * 2);
    int y2 = matrix[23][9].top_left_point_y + (block_side * 2);
    //video_box(x1, y1, x2, y2, color); // Desenha a parte inferior do barril
    x1 = x1;
    while(x1 < x2)
    {
        video_to_fpga(x1, y1, color);
        x1 += 16;
    }
    

    // Parte esquerda do barril
    x1 = matrix[4][0].top_left_point_x - block_side;
    y1 = matrix[4][0].top_left_point_y;
    //x2 = matrix[23][0].top_left_point_x - border;
    //y2 = matrix[23][0].top_left_point_y + block_side + border;
    //video_box(x1, y1, x2, y2, color); // Desenha a parte esquerda do barril
    y1 = y1;
    while(y1 < y2)
    {
        video_to_fpga(x1, y1, color);
        y1 += 16;
    }
    //video_to_fpga(x1, y1, color);

    // Parte direita do barril
    x1 = matrix[4][9].top_left_point_x + block_side + border;
    y1 = matrix[4][9].top_left_point_y;
    //x2 = matrix[23][9].top_left_point_x + (block_side * 2);
    //y2 = matrix[23][9].top_left_point_y + block_side + border;
    //video_box(x1, y1, x2, y2, color); // Desenha a parte direita do barril
    //video_to_fpga(x1, y1, color);
    y1 = y1;
    while(y1 < y2)
    {
        video_to_fpga(x1, y1, color);
        y1 += 16;
    }
    //draw_points_word_with_number(total_score);
    sprintf(msg_buffer, "BUTTONS: PAUSE | RESET | EXIT");
    //video_text ((char_x / 2) - (strlen(msg_buffer) / 2), char_y - 1, msg_buffer);
}

// Função para gerar um bloco na tela
void gen_block(int x1, int y1, int x2, int y2, unsigned color)
{
    int x22 = x2 + 1;
    int y22 = y2 + 1;

    x22++;
    y22++;

    //unsigned int border_color = video_GREY;                   // Cor da borda do bloco (cinza)
    //int border_thickness = block_side - (block_side * 0.125); // Espessura da borda

    //video_box(x1, y1, x2, y2, border_color);
    video_to_fpga(x1, y1, color);

    // Ajusta as coordenadas do bloco interno (subtrai a espessura da borda)
    // x1 += border_thickness;
    // y1 += border_thickness;
    // x2 -= border_thickness;
    // y2 -= border_thickness;
    // //video_box(x1, y1, x2, y2, color);
    // video_to_fpga(x1, y1, color);
}


Tetris_Piece gen_piece(Block_space matrix[24][10])
{

    Tetris_Piece generated_Piece;

    // Define as matrizes para cada peça de Tetris e suas rotações
    int I[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {1, 1, 1, 1}},
        {// Rotação 90°
         {1, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 0, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {1, 1, 1, 1}},
        {// Rotação 270°
         {1, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 0, 0, 0}}};

    int J[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 1, 0}},
        {// Rotação 90°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 1, 1, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 1, 0, 0},
         {1, 1, 1, 0}},
        {// Rotação 270°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 1, 0},
         {1, 1, 1, 0}}};

    int L[4][4][4] = {
        {// Rotação 0°
         {1, 1, 1, 0},
         {1, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 90°
         {0, 0, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 1, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 1, 1, 1},
         {0, 0, 0, 1}},
        {// Rotação 270°
         {0, 0, 0, 0},
         {1, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0}}};

    int O[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {1, 1, 0, 0},
         {1, 1, 0, 0}},
        {// Rotação 90°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {1, 1, 0, 0},
         {1, 1, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {1, 1, 0, 0},
         {1, 1, 0, 0}},
        {// Rotação 270°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {1, 1, 0, 0},
         {1, 1, 0, 0}}};

    int S[4][4][4] = {
        {
            // Rotação 0°
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {0, 1, 1, 0},
            {1, 1, 0, 0},
        },
        {// Rotação 90°
         {0, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 1, 0, 0},
         {0, 1, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 1, 1, 0},
         {1, 1, 0, 0}},
        {// Rotação 270°
         {0, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 1, 0, 0},
         {0, 1, 0, 0}}};

    int T[4][4][4] = {
        {
            // Rotação 0°
            {0, 0, 0, 0},
            {0, 0, 0, 0},
            {1, 1, 1, 0},
            {0, 1, 0, 0},
        },
        {// Rotação 90°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 1, 0, 0},
         {1, 1, 1, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 1, 0, 0},
         {1, 0, 0, 0}},
        {// Rotação 270°
         {0, 0, 0, 0},
         {0, 1, 0, 0},
         {1, 1, 0, 0},
         {0, 1, 0, 0}}};

    int Z[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {1, 1, 0, 0},
         {0, 1, 1, 0}},
        {// Rotação 90°
         {0, 0, 0, 0},
         {0, 0, 0, 0},
         {0, 1, 1, 0},
         {1, 1, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 1, 0, 0},
         {0, 1, 0, 0}},
        {// Rotação 270°
         {0, 0, 0, 0},
         {0, 1, 0, 0},
         {1, 1, 0, 0},
         {1, 0, 0, 0}}};

    int ijlostz = rand() % 7;
    int rotation;

    switch (ijlostz)
    {
    case 1:
        rotation = rand() % 4;
        generated_Piece = print_piece(I[rotation], matrix);
        break;
    case 2:
        rotation = rand() % 4;
        generated_Piece = print_piece(J[rotation], matrix);
        break;
    case 3:
        rotation = rand() % 4;
        generated_Piece = print_piece(L[rotation], matrix);
        break;
    case 4:
        rotation = rand() % 4;
        generated_Piece = print_piece(O[rotation], matrix);
        break;
    case 5:
        rotation = rand() % 4;
        generated_Piece = print_piece(S[rotation], matrix);
        break;
    case 6:
        rotation = rand() % 4;
        generated_Piece = print_piece(T[rotation], matrix);
        break;
    case 7:
        rotation = rand() % 4;
        generated_Piece = print_piece(Z[rotation], matrix);
        break;
    default:
        rotation = rand() % 4;
        generated_Piece = print_piece(I[rotation], matrix);
        break;
    }

    return generated_Piece;
}
/* Função para imprimir uma peça aleatória de cor aleatória em um local aleatório do início da tela
   Essa função só deve ser chamada dentro da função gen_piece */
Tetris_Piece print_piece(int piece_set[4][4], Block_space matrix[24][10])
{
    unsigned int video_color[] = {video_WHITE, video_YELLOW, video_RED,
                                  video_GREEN, video_BLUE, video_CYAN, video_MAGENTA,
                                  video_PINK, video_ORANGE};
    int i = 0;
    int j = 0;
    // int m = rand() % 6;
    int m = 0;
    unsigned color = video_color[(rand() % 9)]; // Cor aleatória
    Tetris_Piece generated_Piece;
    for (i = 0; i < SIZE_M; i++)
    {
        for (j = 0; j < SIZE_M; j++)
        {
            if ((j + m) < 10)
            {

                generated_Piece.blocks_coordinates[i][j].top_left_point_x = matrix[i][j + m].top_left_point_x;
                generated_Piece.blocks_coordinates[i][j].top_left_point_y = matrix[i][j + m].top_left_point_y;
                generated_Piece.blocks_coordinates[i][j].filled = piece_set[i][j];
                generated_Piece.blocks_coordinates[i][j].color = color;
                if (piece_set[i][j])
                {
                    int x1 = generated_Piece.blocks_coordinates[i][j].top_left_point_x;
                    int y1 = generated_Piece.blocks_coordinates[i][j].top_left_point_y;
                    int x2 = generated_Piece.blocks_coordinates[i][j].top_left_point_x + block_side;
                    int y2 = generated_Piece.blocks_coordinates[i][j].top_left_point_y + block_side;
                    matrix[i][j + m].color = color;
                    // matrix[i][j + m].filled = 1;
                    gen_block(x1, y1, x2, y2, color); // Desenha o bloco
                }
            }
        }
    }
    // printf("Matriz[%d][%d] - Coordenada: (%d, %d)\n", i, j, top_left_point_x, top_left_point_y);
    //video_show(); // Mostra os gráficos gerados
    return generated_Piece;
}

Tetris_Piece push_piece_down(Tetris_Piece piece_To_Be_Pushed, Block_space matrix[24][10])
{
    Tetris_Piece pushed_Piece;
    unsigned color;
    int i, j;

    //video_erase(); // Limpa o conteúdo atual da tela
    //video_clear(); // Limpa o buffer da tela (Back buffer)

    //gen_barrel(matrix); // Gera o barril na matriz
    //video_to_fpga(x1, y1, color);
    erase_piece(piece_To_Be_Pushed, matrix);

    // Verifica se a peça pode se mover para baixo
    bool can_move_down = true;
    for (i = 0; i < SIZE_M; i++)
    {
        for (j = 0; j < SIZE_M; j++)
        {
            if (piece_To_Be_Pushed.blocks_coordinates[i][j].filled == 1)
            {
                int i_final = current_piece_pos_i + i + 1;
                int j_final = current_piece_pos_j + j;

                // Verifica se atingiu o fundo ou se a próxima célula está ocupada
                if (i_final >= 24 || matrix[i_final][j_final].filled == 1)
                {
                    can_move_down = false;
                    break;
                }
            }
        }
        if (!can_move_down)
            break;
    }

    // Se a peça pode se mover, move-a para baixo
    if (can_move_down)
    {
        current_piece_pos_i++; // Move a peça para baixo
    }
    else
    {
        // Se não pode mais descer, grava a peça na matriz
        colision = 1;
        for (i = 0; i < SIZE_M; i++)
        {
            for (j = 0; j < SIZE_M; j++)
            {
                if (piece_To_Be_Pushed.blocks_coordinates[i][j].filled == 1)
                {
                    int i_final = current_piece_pos_i + i;
                    int j_final = current_piece_pos_j + j;
                    matrix[i_final][j_final] = piece_To_Be_Pushed.blocks_coordinates[i][j]; // Grava o bloco na matriz
                }
            }
        }
        return piece_To_Be_Pushed; // Se a peça foi gravada na matriz, retorna
    }

    // Atualiza a posição dos blocos da peça e desenha na tela
    for (i = 0; i < SIZE_M; i++)
    {
        for (j = 0; j < SIZE_M; j++)
        {
            if (piece_To_Be_Pushed.blocks_coordinates[i][j].filled == 1)
            {
                int j_final = current_piece_pos_j + j;
                int i_final = current_piece_pos_i + i;

                // Atualiza as coordenadas da peça
                piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_x = matrix[i_final][j_final].top_left_point_x;
                piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_y = matrix[i_final][j_final].top_left_point_y;

                // Desenha o bloco
                int x1 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_x;
                int y1 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_y;
                int x2 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_x + block_side;
                int y2 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_y + block_side;
                color = piece_To_Be_Pushed.blocks_coordinates[i][j].color;

                gen_block(x1, y1, x2, y2, color); // Desenha o bloco
            }
        }
    }

    // Desenhar os blocos existentes na matriz
    for (i = 0; i < 24; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (matrix[i][j].filled == 1)
            {
                int x1 = matrix[i][j].top_left_point_x;
                int y1 = matrix[i][j].top_left_point_y;
                int x2 = matrix[i][j].top_left_point_x + block_side;
                int y2 = matrix[i][j].top_left_point_y + block_side;
                color = matrix[i][j].color;
                gen_block(x1, y1, x2, y2, color); // Desenha o bloco
            }
        }
    }
    // sprintf (msg_buffer, "Points: %03d", total_score);
    // //video_text (char_x - strlen(msg_buffer), 25, msg_buffer);
    // draw_points_word_with_number(total_score);
    //video_show(); // Mostra os gráficos gerados
    pushed_Piece = piece_To_Be_Pushed;

    return pushed_Piece;
}

Tetris_Piece push_piece_sides(bool right, Tetris_Piece piece_To_Be_Pushed, Block_space matrix[24][10])
{
    Tetris_Piece pushed_Piece;
    unsigned color;
    int i = 0;
    int j = 0;

    //video_erase(); // Limpa o conteúdo atual da tela
    //video_clear(); // Limpa o buffer da tela (Back buffer)

    //gen_barrel(matrix); // Gera o barril na matriz
    erase_piece(piece_To_Be_Pushed, matrix);

    // Verifica se pode mover a peça para a direita
    if (right)
    {
        bool can_move_right = true;
        for (i = 0; i < SIZE_M; i++)
        {
            for (j = SIZE_M - 1; j >= 0; j--)
            {
                if (piece_To_Be_Pushed.blocks_coordinates[i][j].filled == 1)
                {
                    int j_final = current_piece_pos_j + j + 1;
                    if (j_final >= 10 || matrix[current_piece_pos_i + i][j_final].filled == 1)
                    {
                        can_move_right = false;
                        break;
                    }
                }
            }
            if (!can_move_right)
                break;
        }

        if (can_move_right)
        {
            current_piece_pos_j++;
        }
    }
    // Verifica se pode mover a peça para a esquerda
    else
    {
        bool can_move_left = true;
        for (i = 0; i < SIZE_M; i++)
        {
            for (j = 0; j < SIZE_M; j++)
            {
                if (piece_To_Be_Pushed.blocks_coordinates[i][j].filled == 1)
                {
                    int j_final = current_piece_pos_j + j - 1;
                    if (j_final < 0 || matrix[current_piece_pos_i + i][j_final].filled == 1)
                    {
                        can_move_left = false;
                        break;
                    }
                }
            }
            if (!can_move_left)
                break;
        }

        if (can_move_left)
        {
            current_piece_pos_j--;
        }
    }

    // Atualiza a posição dos blocos da peça e desenha na tela
    for (i = 0; i < SIZE_M; i++)
    {
        for (j = 0; j < SIZE_M; j++)
        {
            int j_final = current_piece_pos_j + j;
            int i_final = current_piece_pos_i + i;

            // Evitar ultrapassar o limite direito
            if (j_final >= 10)
            {
                j_final = 9;
            }

            piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_x = matrix[i_final][j_final].top_left_point_x;

            if (piece_To_Be_Pushed.blocks_coordinates[i][j].filled == 1)
            {
                int x1 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_x;
                int y1 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_y;
                int x2 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_x + block_side;
                int y2 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_y + block_side;
                color = piece_To_Be_Pushed.blocks_coordinates[i][j].color;

                gen_block(x1, y1, x2, y2, color); // Desenha o bloco
            }
        }
    }

    // Desenhar os blocos existentes na matriz
    for (i = 0; i < 24; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (matrix[i][j].filled == 1)
            {
                int x1 = matrix[i][j].top_left_point_x;
                int y1 = matrix[i][j].top_left_point_y;
                int x2 = matrix[i][j].top_left_point_x + block_side;
                int y2 = matrix[i][j].top_left_point_y + block_side;
                color = matrix[i][j].color;
                gen_block(x1, y1, x2, y2, color); // Desenha o bloco
            }
        }
    }
    // sprintf (msg_buffer, "Points: %03d", total_score);
    // //video_text (char_x - strlen(msg_buffer), 25, msg_buffer);
    // draw_points_word_with_number(total_score);
    //video_show(); // Mostra os gráficos gerados
    pushed_Piece = piece_To_Be_Pushed;

    return pushed_Piece;
}


// Função principal para desenhar a palavra "points: " com os 4 dígitos da pontuação
void draw_points_word_with_number(int points)
{
    int block_size = (block_side + 2) / 4; // Tamanho dos blocos que compõem as letras
    int start_x = screen_x * 0.64;         // Coordenada X inicial para desenhar a palavra
    int start_y = screen_y * 0.1;          // Coordenada Y inicial para desenhar a palavra
    short color = video_WHITE;             // Cor dos blocos (exemplo: branco)

    // Desenhar cada letra da palavra "points: "
    draw_letter_P(start_x, start_y, block_size, color);
    draw_letter_O(start_x + 6 * block_size, start_y, block_size, color); // Espaçamento entre as letras
    draw_letter_I(start_x + 12 * block_size, start_y, block_size, color);
    draw_letter_N(start_x + 16 * block_size, start_y, block_size, color);
    draw_letter_T(start_x + 22 * block_size, start_y, block_size, color);
    draw_letter_S(start_x + 28 * block_size, start_y, block_size, color);
    draw_colon(start_x + 34 * block_size, start_y, block_size, color); // Desenha ":"

    // Desenha os 4 dígitos da pontuação, com espaçamento adequado
    int digit_x = start_x + 40 * block_size; // Posição para os dígitos
    for (int i = 2; i >= 0; i--)
    {
        int digit = (points / (int)int_pow(10, i)) % 10;
        draw_digit(digit, digit_x, start_y, block_size, color);
        digit_x += 6 * block_size; // Espaçamento entre os dígitos
    }
}

// Funções para desenhar letras individuais como blocos
void draw_letter_P(int start_x, int start_y, int block_size, short color)
{
    // Desenha a letra P usando quadrados e retângulos
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                                   // linha vertical
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);                      // topo
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color); // parte do meio
    //video_box(start_x + 2 * block_size, start_y + block_size, start_x + 3 * block_size, start_y + 2 * block_size, color); // lateral direita
}

// Similarmente, funções para outras letras
void draw_letter_O(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                                   // esquerda
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 5 * block_size, color);              // direita
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);                      // topo
    //video_box(start_x + block_size, start_y + 4 * block_size, start_x + 3 * block_size, start_y + 5 * block_size, color); // base
}

void draw_letter_I(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x + block_size, start_y, start_x + 2 * block_size, start_y + 5 * block_size, color); // linha vertical
}

void draw_letter_N(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                      // esquerda
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 5 * block_size, color); // direita
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);         // diagonal
}

void draw_letter_T(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x + block_size, start_y, start_x + 2 * block_size, start_y + 5 * block_size, color); // linha vertical
    //video_box(start_x, start_y, start_x + 3 * block_size, start_y + block_size, color);                  // topo
}

void draw_letter_S(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + 3 * block_size, start_y + block_size, color);                                       // topo
    //video_box(start_x, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color);                  // meio
    //video_box(start_x, start_y + 4 * block_size, start_x + 3 * block_size, start_y + 5 * block_size, color);                  // base
    //video_box(start_x, start_y + block_size, start_x + block_size, start_y + 2 * block_size, color);                          // esquerda superior
    //video_box(start_x + 2 * block_size, start_y + 3 * block_size, start_x + 3 * block_size, start_y + 4 * block_size, color); // direita inferior
}

void draw_colon(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x + block_size, start_y + block_size, start_x + 2 * block_size, start_y + 2 * block_size, color);     // ponto superior
    //video_box(start_x + block_size, start_y + 3 * block_size, start_x + 2 * block_size, start_y + 4 * block_size, color); // ponto inferior
}

void draw_digit_0(int start_x, int start_y, int block_size, short color)
{
    draw_letter_O(start_x, start_y, block_size, color);
}

void draw_digit_1(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x + 2 * block_size, start_y, start_x + 3 * block_size, start_y + 5 * block_size, color); // barra vertical direita
}

void draw_digit_2(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + 3 * block_size, start_y + block_size, color);                      // topo
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 3 * block_size, color);              // lado direito superior
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color); // meio
    //video_box(start_x, start_y + 2 * block_size, start_x + block_size, start_y + 5 * block_size, color);                  // lado esquerdo inferior
    //video_box(start_x + block_size, start_y + 4 * block_size, start_x + 3 * block_size, start_y + 5 * block_size, color); // base
}

void draw_digit_3(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);                      // topo
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color); // meio
    //video_box(start_x + block_size, start_y + 4 * block_size, start_x + 3 * block_size, start_y + 5 * block_size, color); // base
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 5 * block_size, color);              // barra direita
}

void draw_digit_4(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x * block_size, start_y, start_x + 1 * block_size, start_y + block_size, color);     // topo direito
    //video_box(start_x + 2 * block_size, start_y, start_x + 3 * block_size, start_y + 5 * block_size, color); // barra vertical direita
    //video_box(start_x, start_y + 2 * block_size, start_x + 2 * block_size, start_y + 3 * block_size, color); // barra horizontal do meio
}

void draw_digit_5(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);                          // topo
    //video_box(start_x, start_y, start_x + block_size, start_y + 3 * block_size, color);                                       // barra esquerda
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color);     // meio
    //video_box(start_x + block_size, start_y + 4 * block_size, start_x + 3 * block_size, start_y + 5 * block_size, color);     // base
    //video_box(start_x + 3 * block_size, start_y + 2 * block_size, start_x + 4 * block_size, start_y + 5 * block_size, color); // barra direita inferior
}

void draw_digit_6(int start_x, int start_y, int block_size, short color)
{
    draw_digit_5(start_x, start_y, block_size, color);                                                                    // Reutiliza partes do "5"
    //video_box(start_x + 3 * block_size, start_y + block_size, start_x + 4 * block_size, start_y + 3 * block_size, color); // barra direita superior
}

void draw_digit_7(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);         // topo
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 5 * block_size, color); // barra direita
}

void draw_digit_8(int start_x, int start_y, int block_size, short color)
{
    draw_letter_O(start_x, start_y, block_size, color);                                                                   // Desenha o "0"
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color); // barra horizontal do meio
}

void draw_digit_9(int start_x, int start_y, int block_size, short color)
{
    draw_digit_8(start_x, start_y, block_size, color);                                                   // Reutiliza o "8"
    //video_box(start_x, start_y + 2 * block_size, start_x + block_size, start_y + 5 * block_size, color); // remove barra esquerda inferior
}

// Função para desenhar os dígitos de 0 a 9
void draw_digit(int digit, int start_x, int start_y, int block_size, short color)
{
    switch (digit)
    {
    case 0:
        draw_digit_0(start_x, start_y, block_size, color);
        break;
    case 1:
        draw_digit_1(start_x, start_y, block_size, color);
        break;
    case 2:
        draw_digit_2(start_x, start_y, block_size, color);
        break;
    case 3:
        draw_digit_3(start_x, start_y, block_size, color);
        break;
    case 4:
        draw_digit_4(start_x, start_y, block_size, color);
        break;
    case 5:
        draw_digit_5(start_x, start_y, block_size, color);
        break;
    case 6:
        draw_digit_6(start_x, start_y, block_size, color);
        break;
    case 7:
        draw_digit_7(start_x, start_y, block_size, color);
        break;
    case 8:
        draw_digit_8(start_x, start_y, block_size, color);
        break;
    case 9:
        draw_digit_9(start_x, start_y, block_size, color);
        break;
    default:
        break;
    }
}

int int_pow(int base, int exponent)
{
    int result = 1;
    while (exponent > 0)
    {
        result *= base;
        exponent--;
    }
    return result;
}

void draw_letter_P_italic(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                                   // lado esquerdo
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);                      // topo
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color); // meio
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 3 * block_size, color);              // lado direito do topo
}

void draw_letter_A_italic(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x + block_size, start_y, start_x + 2 * block_size, start_y + block_size, color);                      // topo
    //video_box(start_x, start_y + block_size, start_x + block_size, start_y + 5 * block_size, color);                      // lado esquerdo
    //video_box(start_x + 3 * block_size, start_y + block_size, start_x + 4 * block_size, start_y + 5 * block_size, color); // lado direito
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color); // meio
}

void draw_letter_U_italic(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                                   // lado esquerdo
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 5 * block_size, color);              // lado direito
    //video_box(start_x + block_size, start_y + 4 * block_size, start_x + 3 * block_size, start_y + 5 * block_size, color); // base
}
void draw_letter_S_italic(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);                          // topo
    //video_box(start_x, start_y, start_x + block_size, start_y + 2 * block_size, color);                                       // lado esquerdo do topo
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color);     // meio
    //video_box(start_x + 3 * block_size, start_y + 3 * block_size, start_x + 4 * block_size, start_y + 5 * block_size, color); // lado direito do final
    //video_box(start_x + block_size, start_y + 4 * block_size, start_x + 3 * block_size, start_y + 5 * block_size, color);     // base
}

void draw_letter_E_italic(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                                   // lado esquerdo
    //video_box(start_x + block_size, start_y, start_x + 4 * block_size, start_y + block_size, color);                      // topo
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color); // meio
    //video_box(start_x + block_size, start_y + 4 * block_size, start_x + 4 * block_size, start_y + 5 * block_size, color); // base
}

void draw_word_PAUSE_()
{

    int block_size = (block_side) / 4;                           // Tamanho dos blocos que compõem as letras
    int total_blocks_width = (5 * 4) + 4;                        // 5 letras, cada uma com 4 blocos de largura e 4 blocos de espaço entre letras
    int total_width_in_pixels = total_blocks_width * block_size; // Largura total da palavra em pixels

    int start_x = (screen_x / 2) - (total_width_in_pixels / 2); // Centraliza no eixo X
    int start_y = (screen_y / 2);                               // Centraliza no eixo Y
    short color = video_WHITE;                                  // Cor dos blocos (exemplo: branco)

    // Desenha cada letra com um pequeno deslocamento para criar o efeito itálico
    draw_letter_P_italic(start_x, start_y, block_size, color);
    draw_letter_A_italic(start_x + 5 * block_size, start_y + block_size, block_size, color);
    draw_letter_U_italic(start_x + 10 * block_size, start_y + 2 * block_size, block_size, color);
    draw_letter_S_italic(start_x + 15 * block_size, start_y + 3 * block_size, block_size, color);
    draw_letter_E_italic(start_x + 20 * block_size, start_y + 4 * block_size, block_size, color);
}

// Desenha a letra "G"
void draw_letter_G(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + 4 * block_size, start_y + block_size, color);                                       // topo
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                                       // lado esquerdo
    //video_box(start_x, start_y + 4 * block_size, start_x + 4 * block_size, start_y + 5 * block_size, color);                  // base
    //video_box(start_x + 3 * block_size, start_y + 3 * block_size, start_x + 4 * block_size, start_y + 5 * block_size, color); // lado direito inferior
    //video_box(start_x + 2 * block_size, start_y + 2 * block_size, start_x + 4 * block_size, start_y + 3 * block_size, color); // barra central
}

// Desenha a letra "A"
void draw_letter_A(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y + block_size, start_x + block_size, start_y + 5 * block_size, color);                      // lado esquerdo
    //video_box(start_x + 3 * block_size, start_y + block_size, start_x + 4 * block_size, start_y + 5 * block_size, color); // lado direito
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);                      // topo
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color); // barra central
}

// Desenha a letra "M"
void draw_letter_M(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                                   // lado esquerdo
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 5 * block_size, color);              // lado direito
    //video_box(start_x + block_size, start_y + block_size, start_x + 2 * block_size, start_y + 2 * block_size, color);     // diagonal esquerda
    //video_box(start_x + 2 * block_size, start_y + block_size, start_x + 3 * block_size, start_y + 2 * block_size, color); // diagonal direita
}

// Desenha a letra "E"
void draw_letter_E(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + 4 * block_size, start_y + block_size, color);                      // topo
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                      // lado esquerdo
    //video_box(start_x, start_y + 4 * block_size, start_x + 4 * block_size, start_y + 5 * block_size, color); // base
    //video_box(start_x, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color); // barra central
}

// Desenha a letra "V"
void draw_letter_V(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + block_size, start_y + 4 * block_size, color);                                   // lado esquerdo
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 4 * block_size, color);              // lado direito
    //video_box(start_x + block_size, start_y + 4 * block_size, start_x + 3 * block_size, start_y + 5 * block_size, color); // base
}

// Desenha a letra "R"
void draw_letter_R(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + block_size, start_y + 5 * block_size, color);                                       // lado esquerdo
    //video_box(start_x + 3 * block_size, start_y, start_x + 4 * block_size, start_y + 3 * block_size, color);                  // lado direito superior
    //video_box(start_x + block_size, start_y, start_x + 3 * block_size, start_y + block_size, color);                          // topo
    //video_box(start_x + block_size, start_y + 2 * block_size, start_x + 3 * block_size, start_y + 3 * block_size, color);     // barra central
    //video_box(start_x + 2 * block_size, start_y + 3 * block_size, start_x + 3 * block_size, start_y + 4 * block_size, color); // diagonal inferior direita (parte 1)
    //video_box(start_x + 3 * block_size, start_y + 4 * block_size, start_x + 4 * block_size, start_y + 6 * block_size, color); // diagonal inferior direita (parte 2)   
}

// Desenha o ponto de exclamação "!"
void draw_letter_exclamation(int start_x, int start_y, int block_size, short color)
{
    //video_box(start_x, start_y, start_x + block_size, start_y + 4 * block_size, color);                  // barra vertical
    //video_box(start_x, start_y + 5 * block_size, start_x + block_size, start_y + 6 * block_size, color); // ponto na base
}

void draw_word_GAME_OVER()
{
    int block_size = (block_side) / 4;                           // Tamanho dos blocos que compõem as letras
    int total_blocks_width = 45;                                 // Total de blocos para "GAME OVER!"
    int total_width_in_pixels = total_blocks_width * block_size; // Largura total da frase em pixels

    int start_x = (screen_x / 2) - (total_width_in_pixels / 2); // Centraliza no eixo X
    int start_y = (screen_y / 2);                               // Centraliza no eixo Y
    short color = video_RED;                                    // Cor dos blocos

    // Desenha "GAME"
    draw_letter_G(start_x, start_y, block_size, color);
    draw_letter_A(start_x + 5 * block_size, start_y, block_size, color);
    draw_letter_M(start_x + 10 * block_size, start_y, block_size, color);
    draw_letter_E(start_x + 15 * block_size, start_y, block_size, color);

    // Espaço entre as palavras
    start_x += 25 * block_size;

    // Desenha "OVER"
    draw_letter_O(start_x, start_y, block_size, color);
    draw_letter_V(start_x + 5 * block_size, start_y, block_size, color);
    draw_letter_E(start_x + 10 * block_size, start_y, block_size, color);
    draw_letter_R(start_x + 15 * block_size, start_y, block_size, color);

    // Espaço antes do ponto de exclamação
    start_x += 20 * block_size;

    // Desenha "!"
    draw_letter_exclamation(start_x, start_y, block_size, color);
}


// Função para converter uma cor RGB 16 bits em 9 bits BGR
// Função para converter uma cor de 16 bits RGB para 9 bits BGR
unsigned int convert_to_9bit_BGR(unsigned int color) {
    uint8_t red = (color >> 11) & 0x1F;     // Extrair 5 bits de vermelho
    uint8_t green = (color >> 5) & 0x3F;    // Extrair 6 bits de verde
    uint8_t blue = color & 0x1F;            // Extrair 5 bits de azul

    uint8_t b = blue >> 2;    // Reduzir para 3 bits
    uint8_t g = green >> 3;   // Reduzir para 3 bits
    uint8_t r = red >> 2;     // Reduzir para 3 bits

    return (b << 6) | (g << 3) | r;
}

void video_to_fpga(int x1, int y1, unsigned int color) {
    int block_x = x1 / 8;      // Índice do bloco na direção x
    int block_y = y1 / 8;      // Índice do bloco na direção y

    int position = block_y * 80 + block_x;  // Cálculo da posição (0 a 4799 para blocos de 8x8 em uma tela de 640x480)

    int converted_color = (int) convert_to_9bit_BGR(color);
    printf("Posição: %d\n", position); // Para depuração
    draw_square_16x16(position, converted_color);
}


Tetris_Piece erase_piece(Tetris_Piece piece_To_Be_Pushed, Block_space matrix[24][10])
{
    Tetris_Piece pushed_Piece;
    unsigned color;
    int i, j;
    // Atualiza a posição dos blocos da peça e desenha na tela
    for (i = 0; i < SIZE_M; i++)
    {
        for (j = 0; j < SIZE_M; j++)
        {
            if (piece_To_Be_Pushed.blocks_coordinates[i][j].filled == 1)
            {
                int j_final = current_piece_pos_j + j;
                int i_final = current_piece_pos_i + i;

                // Atualiza as coordenadas da peça
                piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_x = matrix[i_final][j_final].top_left_point_x;
                piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_y = matrix[i_final][j_final].top_left_point_y;

                // Desenha o bloco
                int x1 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_x;
                int y1 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_y;
                int x2 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_x + block_side;
                int y2 = piece_To_Be_Pushed.blocks_coordinates[i][j].top_left_point_y + block_side;
                color = video_BLACK;

                gen_block(x1, y1, x2, y2, color); // Desenha o bloco
            }
        }
    }

    pushed_Piece = piece_To_Be_Pushed;
    return pushed_Piece;

}

void erase_line(int line, Block_space matrix[24][10])
{
    unsigned color;
    int i;
    // Atualiza a posição dos blocos da peça e desenha na tela
    for (i = 0; i < 10; i++)
    {

        // Desenha o bloco
        int x1 = matrix[line][i].top_left_point_x;
        int y1 = matrix[line][i].top_left_point_y;
        int x2 = matrix[line][i].top_left_point_x + block_side;
        int y2 = matrix[line][i].top_left_point_y + block_side;
        color = video_BLACK;

        gen_block(x1, y1, x2, y2, color); // Desenha o bloco

        
    }


}


