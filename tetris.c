#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <intelfpgaup/KEY.h>   // Biblioteca para controle dos botões do DE1-SoC
#include <intelfpgaup/video.h> // Biblioteca para controle da saída VGA no DE1-SoC

int screen_x, screen_y; // Resolução da tela VGA
int block_side;         // Tamanho lateral de cada bloco
int char_x, char_y;     // Coordenadas dos caracteres no display de texto

// typedef enum { false, true } bool;  // Definindo um tipo booleano
//  Estrutura para armazenar as coordenadas de canto superior esquerdo de cada bloco
typedef struct
{
    int top_left_point_x; // Coordenada x do canto superior esquerdo do bloco
    int top_left_point_y; // Coordenada y do canto superior esquerdo do bloco
    int filled = 0;       // Se o espaço de bloco está preenchido é 1, se não é 0

} Block_space;

// Protótipos das funções
void fill_matrix(Block_space matrix[24][10]);                       // Preenche a matriz com blocos
void print_matrix(Block_space matrix[24][10]);                      // Imprime e desenha a matriz
void gen_line(int *, int *, int *, int *, unsigned *);              // Gera uma linha aleatória na tela
void gen_barrel(Block_space matrix[24][10]);                        // Gera o barril central
void gen_block(int, int, int, int, unsigned);                       // Gera um bloco na tela
void gen_piece(Block_space matrix[24][10]);                         // Gera uma peça na tela
void print_piece(int piece_set[4][4], Block_space matrix[24][10]);  // Imprime uma peça aleatória de cor aleatória em um local aleatório do início da tela

volatile sig_atomic_t stop; // Variável usada para sinalizar interrupção do programa

// Função de callback para capturar o sinal SIGINT (Ctrl+C) e encerrar o programa
void catchSIGINT(int signum)
{
    stop = 1; // Sinaliza que o programa deve parar
}

int main()
{
    Block_space matrix[24][10]; // Matriz 24x10 que representa os blocos na tela

    int KEY_data;       // Variável para armazenar o estado do botão do DE1-SoC
    int x1, y1, x2, y2; // Coordenadas para desenhar linhas ou blocos
    unsigned color;     // Cor para os gráficos
    time_t t;           // Variável para o tempo (usado para semente de rand)

    signal(SIGINT, catchSIGINT); // Captura SIGINT (^C) para fechar o programa de forma segura
    srand((unsigned)time(&t));   // Inicializa o gerador de números aleatórios

    // Abre os drivers de dispositivo para os botões e a saída de vídeo
    if (!KEY_open() || !video_open())
    {
        printf("Erro ao abrir drivers KEY ou video.\n");
        return -1; // Sai se não conseguir abrir os drivers
    }

    video_read(&screen_x, &screen_y, &char_x, &char_y); // Lê as dimensões da tela e do texto
    block_side = (screen_y * 0.8) / 24;                 // Calcula o tamanho lateral de cada bloco com base na tela

    video_erase();        // Limpa o conteúdo atual da tela
    video_show();         // Mostra a tela limpa
    video_clear();        // Limpa o buffer da tela (Back buffer)
    fill_matrix(matrix);  // Preenche a matriz com blocos
    print_matrix(matrix); // Desenha os blocos da matriz na tela
    gen_piece(matrix);    // Gera uma peça na tela

    video_show(); // Mostra os gráficos gerados

    // Loop principal que continua até o usuário pressionar Ctrl+C
    while (!stop)
    {
        printf("Press a pushbutton KEY (^C to exit)\n");
        KEY_read(&KEY_data); // Lê o estado do botão

        // Espera até que um botão seja pressionado ou o programa seja interrompido
        while (!KEY_data && !stop)
            KEY_read(&KEY_data);

        // Gera uma linha aleatória na tela
        gen_line(&x1, &y1, &x2, &y2, &color);
        video_show();                      // Mostra a linha na tela
        video_line(x1, y1, x2, y2, color); // Desenha a linha
        video_show();                      // Atualiza a tela novamente com a linha desenhada
    }

    // Fecha os drivers de vídeo e do botão
    video_close();
    KEY_close();

    printf("\nExiting sample program\n");

    return 0;
}

// Função para gerar o barril na tela
void gen_barrel(Block_space matrix[24][10])
{
    unsigned int barrel_color[] = {video_WHITE}; // Define a cor do barril como branco
    unsigned color = barrel_color[0];

    // Parte inferior do barril
    int x1 = matrix[23][0].top_left_point_x - block_side;
    int y1 = matrix[23][0].top_left_point_y + block_side;
    int x2 = matrix[23][9].top_left_point_x + (block_side * 2);
    int y2 = matrix[23][9].top_left_point_y + (block_side * 2);
    video_box(x1, y1, x2, y2, color); // Desenha a parte inferior do barril

    // Parte esquerda do barril
    x1 = matrix[4][0].top_left_point_x - block_side;
    y1 = matrix[4][0].top_left_point_y;
    x2 = matrix[23][0].top_left_point_x;
    y2 = matrix[23][0].top_left_point_y + block_side;
    video_box(x1, y1, x2, y2, color); // Desenha a parte esquerda do barril

    // Parte direita do barril
    x1 = matrix[4][9].top_left_point_x + block_side;
    y1 = matrix[4][9].top_left_point_y;
    x2 = matrix[23][9].top_left_point_x + (block_side * 2);
    y2 = matrix[23][9].top_left_point_y + block_side;
    video_box(x1, y1, x2, y2, color); // Desenha a parte direita do barril
}

// Função para gerar um bloco na tela
void gen_block(int x1, int y1, int x2, int y2, unsigned color)
{

    unsigned int border_color = video_GREY;                   // Cor da borda do bloco (cinza)
    int border_thickness = block_side - (block_side * 0.125); // Espessura da borda

    video_box(x1, y1, x2, y2, border_color);

    // Ajusta as coordenadas do bloco interno (subtrai a espessura da borda)
    x1 += border_thickness;
    y1 += border_thickness;
    x2 -= border_thickness;
    y2 -= border_thickness;
    video_box(x1, y1, x2, y2, color);
}

// Função para preencher a matriz com coordenadas de blocos
void fill_matrix(Block_space matrix[24][10])
{
    // Calcula as dimensões da matriz com base no tamanho da tela e do bloco
    int matrix_width = (10 * block_side);
    int matrix_height = screen_y * 0.8;
    int center_x = screen_x / 2;
    int base_y = screen_y * 0.1;

    // Define as coordenadas iniciais para o primeiro bloco
    int initial_matrix_point_x = center_x - matrix_width / 2;
    int initial_matrix_point_y = screen_y - matrix_height - base_y;
    int top_left_point_x = initial_matrix_point_x;
    int top_left_point_y = initial_matrix_point_y;

    // Preenche a matriz com coordenadas para cada bloco
    int i = 0;
    int j = 0;
    for (i = 0; i < 24; i++)
    {
        for (j = 0; j < 10; j++)
        {
            matrix[i][j].top_left_point_x = top_left_point_x;
            matrix[i][j].top_left_point_y = top_left_point_y;
            top_left_point_x += block_side;
        }
        top_left_point_x = initial_matrix_point_x;
        top_left_point_y += block_side;
    }

    gen_barrel(matrix); // Gera o barril na matriz
}

// Função para imprimir e desenhar a matriz na tela
void print_matrix(Block_space matrix[24][10])
{

    unsigned int video_color[] = {video_WHITE, video_YELLOW, video_RED,
                                  video_GREEN, video_BLUE, video_CYAN, video_MAGENTA,
                                  video_PINK, video_ORANGE};
    unsigned color = video_color[(rand() % 9)]; // Cor aleatória

    // Desenha cada bloco na tela com base nas coordenadas da matriz
    int i = 0;
    int j = 0;
    for (i = 0; i < 24; i++)
    {
        for (j = 0; j < 10; j++)
        {
            int x1 = matrix[i][j].top_left_point_x;
            int y1 = matrix[i][j].top_left_point_y;
            int x2 = matrix[i][j].top_left_point_x + block_side;
            int y2 = matrix[i][j].top_left_point_y + block_side;
            color = video_color[(rand() % 9)];
            gen_block(x1, y1, x2, y2, color); // Desenha o bloco
        }
    }
}

// Função para gerar uma linha aleatória na tela
void gen_line(int *x1, int *y1, int *x2, int *y2, unsigned *color)
{
    unsigned int line_color[] = {video_BLUE, video_GREEN, video_RED, video_YELLOW, video_MAGENTA};
    int color_idx = rand() % 5; // Escolhe uma cor aleatória para a linha

    // Gera coordenadas aleatórias para a linha
    *x1 = rand() % screen_x;
    *x2 = rand() % screen_x;
    *y1 = rand() % screen_y;
    *y2 = rand() % screen_y;
    *color = line_color[color_idx]; // Define a cor da linha
}

void gen_piece(Block_space matrix[24][10])
{

#define SIZE_M 4

    // Define as matrizes para cada peça de Tetris e suas rotações
    int I[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {1, 1, 1, 1},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 90°
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {1, 1, 1, 1},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 270°
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0}}};

    int J[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 0}},
        {// Rotação 90°
         {0, 1, 0, 0},
         {0, 1, 1, 1},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 1, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0}},
        {// Rotação 270°
         {0, 0, 1, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}}};

    int L[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {1, 1, 1, 0},
         {1, 0, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 90°
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 1, 1, 1},
         {0, 0, 0, 1},
         {0, 0, 0, 0}},
        {// Rotação 270°
         {1, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 0, 0, 0}}};

    int O[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {0, 1, 1, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        {// Rotação 90°
         {0, 0, 0, 0},
         {0, 1, 1, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 1, 1, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        {// Rotação 270°
         {0, 0, 0, 0},
         {0, 1, 1, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}}};

    int S[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {0, 1, 1, 0},
         {1, 1, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 90°
         {0, 1, 0, 0},
         {0, 1, 1, 1},
         {0, 0, 1, 0},
         {0, 0, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 1, 1, 0},
         {1, 1, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 270°
         {0, 1, 0, 0},
         {0, 1, 1, 1},
         {0, 0, 1, 0},
         {0, 0, 0, 0}}};

    int T[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {1, 1, 1, 0},
         {0, 1, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 90°
         {0, 1, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 1, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0}},
        {// Rotação 270°
         {0, 1, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}}};

    int Z[4][4][4] = {
        {// Rotação 0°
         {0, 0, 0, 0},
         {1, 1, 0, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        {// Rotação 90°
         {0, 0, 1, 1},
         {0, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 180°
         {0, 0, 0, 0},
         {0, 1, 1, 0},
         {1, 1, 0, 0},
         {0, 0, 0, 0}},
        {// Rotação 270°
         {0, 0, 1, 1},
         {0, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}}};

    int ijlostz = rand() % 7;

    switch (ijlostz)
    {
    case 1:
        int k = rand() % 4;
        print_piece(I[k], matrix);
        break;
    case 2:
        int k = rand() % 4;
        print_piece(J[k], matrix);
        break;
    case 3:
        int k = rand() % 4;
        print_piece(L[k], matrix);
        break;
    case 4:
        int k = rand() % 4;
        print_piece(O[k], matrix);
        break;
    case 5:
        int k = rand() % 4;
        print_piece(S[k], matrix);
        break;
    case 6:
        int k = rand() % 4;
        print_piece(T[k], matrix);
        break;
    case 7:
        int k = rand() % 4;
        print_piece(Z[k], matrix);
        break;
    default:
        int k = rand() % 4;
        print_piece(I[k], matrix);
        break;
    }
}
/* Função para imprimir uma peça aleatória de cor aleatória em um local aleatório do início da tela
   Essa função só deve ser chamada dentro da função gen_piece */
void print_piece(int piece_set[4][4], Block_space matrix[24][10])
{
    int i = 0;
    int j = 0;
    int m = rand() % 6;
    unsigned color = video_color[(rand() % 9)]; // Cor aleatória
    for (i = 0; i < SIZE_M; i++)
    {
        for (j = 0; j < SIZE_M; j++)
        {
            matrix[i][j + m].filled = piece_set[i][j];
            if (piece_set[i][j])
            {
                int x1 = matrix[i][j].top_left_point_x;
                int y1 = matrix[i][j].top_left_point_y;
                int x2 = matrix[i][j].top_left_point_x + block_side;
                int y2 = matrix[i][j].top_left_point_y + block_side;
                gen_block(x1, y1, x2, y2, color); // Desenha o bloco
            }
        }
    }
}



