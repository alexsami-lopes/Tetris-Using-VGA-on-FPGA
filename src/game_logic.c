#include <stdbool.h>                    // Para usar o tipo bool
#include <stdlib.h>                     // Para funções de alocação de memória e controle de processos
#include <intelfpgaup/video.h>          // Para funções de vídeo no DE1-SoC
#include "../include/input.h"           // Funções de entrada (acelerômetro e botões)
#include "../include/graphics.h"        // Funções gráficas
#include "../include/game_logic.h"      // Cabeçalho da lógica do jogo
#include "../include/address_map_arm.h" // Mapeamento de endereços de memória ARM

/**
 * @brief Preenche a matriz do jogo Tetris com espaços de blocos vazios e suas coordenadas
 * 
 * @param matrix Block_space[24][10] - Matriz de 24 linhas por 10 colunas, representando os blocos do jogo
 */
void fill_matrix(Block_space matrix[24][10])
{
    // Calcula as dimensões da matriz com base no tamanho da tela e do bloco
    int matrix_width = (10 * block_side);   // Largura da matriz, 10 blocos
    int matrix_height = screen_y * 0.8;     // Altura da matriz, 80% da altura da tela
    int center_x = screen_x / 2;            // Coordenada X do centro da tela
    int base_y = screen_y * 0.1;            // Base da matriz, 10% da altura da tela

    // Define as coordenadas iniciais para o primeiro bloco
    int initial_matrix_point_x = center_x - matrix_width / 2;       // Ponto inicial X da matriz (centralizado)
    int initial_matrix_point_y = screen_y - matrix_height - base_y; // Ponto inicial Y da matriz
    int top_left_point_x = initial_matrix_point_x;                  // X do canto superior esquerdo do primeiro bloco
    int top_left_point_y = initial_matrix_point_y;                  // Y do canto superior esquerdo do primeiro bloco

    // Preenche a matriz com coordenadas para cada bloco
    int i = 0;
    int j = 0;
    for (i = 0; i < 24; i++)     // 24 linhas na matriz
    {
        for (j = 0; j < 10; j++) // 10 colunas na matriz
        {
            matrix[i][j].top_left_point_x = top_left_point_x; // Coordenada X do bloco
            matrix[i][j].top_left_point_y = top_left_point_y; // Coordenada Y do bloco
            matrix[i][j].filled = 0;                          // Inicialmente, o bloco não está preenchido
            matrix[i][j].color = video_WHITE;                 // A cor inicial é branca
            top_left_point_x += block_side;                   // Move para o próximo bloco na horizontal
        }
        top_left_point_x = initial_matrix_point_x;            // Reseta o X para a próxima linha
        top_left_point_y += block_side;                       // Move para a próxima linha
    }

    gen_barrel(matrix); // Gera o barril (contorno do campo de jogo) na matriz
}

/**
 * @brief Verifica e remove as linhas completas na matriz, movendo as linhas superiores para baixo
 * 
 * @param matrix Block_space[24][10] - Matriz de 24 linhas por 10 colunas representando os blocos do jogo
 * @return int - Retorna a pontuação obtida com base no número de linhas removidas
 */
int clear_full_lines(Block_space matrix[24][10])
{
    int i, j, k;
    int score_increment = 10;             // Pontuação base por linha completa
    int new_score = 0;                    // Pontuação obtida nesta rodada

    // Verifica cada linha da matriz para ver se está completa
    for (i = 23; i >= 0; i--)             // Começa da última linha (23) e vai até a primeira (0)
    {
        bool line_full = true;            // Assume que a linha está cheia

        // Verifica se a linha está completa
        for (j = 0; j < 10; j++)          // Verifica todos os blocos da linha
        {
            if (matrix[i][j].filled == 0) // Se encontrar um bloco vazio
            {
                line_full = false;        // A linha não está completa
                break;
            }
        }

        // Se a linha estiver completa, remove-a e desloca as linhas acima
        if (line_full)
        {
            // Move todas as linhas acima da linha removida para baixo
            for (k = i; k > 0; k--)                                // Desce todas as linhas acima de "i"
            {
                for (j = 0; j < 10; j++)                           // Move cada bloco da linha
                {
                    matrix[k][j].filled = matrix[k - 1][j].filled; // Copia o estado do bloco de cima
                    matrix[k][j].color = matrix[k - 1][j].color;   // Copia a cor do bloco de cima
                }
            }

            // Conta a linha como removida
            lines_cleared++;
            i++; // Reavalia a linha atual após o deslocamento
        }
    }

    // Calcula a pontuação com base no número de linhas removidas
    if (lines_cleared > 0)
    {
        int multiplier = 1; // Multiplicador de pontuação
        if (lines_cleared == 2)
            multiplier = 2;
        else if (lines_cleared == 3)
            multiplier = 3;
        else if (lines_cleared >= 4)
            multiplier = 4;

        // A pontuação aumenta conforme o número de linhas removidas simultaneamente
        new_score = score_increment * lines_cleared * multiplier;
    }

    return new_score; // Retorna a nova pontuação
}

/**
 * @brief Verifica se o jogo terminou, ou seja, se as peças atingiram o topo do barril
 * 
 * @param matrix Block_space[24][10] - Matriz de 24 linhas por 10 colunas representando os blocos do jogo
 * @return bool - Retorna true se o jogo terminou (peças atingiram o topo), false caso contrário
 */
bool check_game_over(Block_space matrix[24][10])
{
    for (int j = 0; j < 10; j++)      // Verifica todas as colunas na linha 4
    {
        if (matrix[4][j].filled == 1) // Se algum bloco na linha 4 estiver preenchido
        {
            return true;              // Retorna true, indicando "Game Over"
        }
    }
    return false; // Retorna false, indicando que o jogo ainda pode continuar
}
