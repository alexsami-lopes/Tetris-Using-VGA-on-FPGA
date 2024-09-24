#include <stdio.h>                        // Para funções de entrada/saída padrão
#include <unistd.h>                       // Para funções POSIX como usleep()
#include <stdlib.h>                       // Para funções de alocação de memória e controle de processos
#include <signal.h>                       // Para manipular sinais como SIGINT (Ctrl+C)
#include <fcntl.h>                        // Para controle de arquivos (open, close)
#include <stdbool.h>                      // Para usar o tipo bool (true/false)
#include <time.h>                         // Para gerar números aleatórios com srand()
#include <string.h>                       // Para manipulação de strings
#include <sys/mman.h>                     // Para mapear memória física
#include <intelfpgaup/KEY.h>              // Biblioteca do DE1-SoC para manipulação dos botões
#include <intelfpgaup/video.h>            // Biblioteca do DE1-SoC para manipulação da saída de vídeo
#include "../include/graphics.h"          // Funções gráficas para o jogo
#include "../include/input.h"             // Funções de entrada (botões e acelerômetro)
#include "../include/game_logic.h"        // Lógica principal do jogo
#include "../include/address_map_arm.h"   // Mapeamento de endereços de memória ARM

// Declare global variables
int screen_x, screen_y;      // Resolução da tela VGA (dimensões)
int char_x, char_y;          // Coordenadas dos caracteres no display de texto
int block_side;              // Tamanho lateral de cada bloco no Tetris
int colision = 0;            // Flag para indicar se houve colisão da peça com o fundo ou outra peça
int current_piece_pos_i = 0; // Posição atual da peça no eixo Y (vertical)
int current_piece_pos_j = 0; // Posição atual da peça no eixo X (horizontal)
int refresh_time = 5;        // Intervalo de tempo em milissegundos para atualizar o jogo
int refresh_time_accel;      // Tempo de atualização ajustado pelo acelerômetro
int accelered;               // Flag para indicar se o acelerômetro está "acelerado"
int total_score = 0;         // Pontuação total do jogador
int lines_cleared = 0;       // Linhas removidas no jogo
int insensibility = 9;       // Sensibilidade do acelerômetro ao movimento lateral
char msg_buffer[80];         // Buffer para mensagens de texto na tela

Block_space matrix[24][10];          // Matriz 24x10 que representa os blocos na tela (área do jogo)
volatile sig_atomic_t stop = 0;      // Flag para parar o jogo (usada para sinalizar o fim)
volatile sig_atomic_t pause_ = 0;    // Flag para pausar o jogo
volatile sig_atomic_t game_over = 0; // Flag para indicar o fim de jogo

unsigned int *i2c0_base_ptr;   // Ponteiro para a base do controlador I2C0
unsigned int *sysmgr_base_ptr; // Ponteiro para a base do System Manager
void *i2c0base_virtual;        // Endereço virtual mapeado do I2C0
void *sysmgrbase_virtual;      // Endereço virtual mapeado do System Manager
int fd_i2c0base = -1;          // Descritor de arquivo para I2C0
int fd_sysmgr = -1;            // Descritor de arquivo para o System Manager

// Função que captura SIGINT (Ctrl+C) para parar o jogo
void catchSIGINT(int signum)
{
    printf("Unmapping\n");  // Exibe mensagem ao detectar o sinal
    stop = 1;               // Define a flag de parada
}

// Função principal do jogo
int main(void)
{
    uint8_t devid = 0;         // ID do dispositivo (ADXL345)
    float mg_per_lsb = 3.2;    // Fator de conversão do ADXL345 (miligrama por LSB)
    int16_t XYZ[3];            // Array para armazenar os valores do acelerômetro nos eixos X, Y e Z
    stop = 0;                  // Inicializa o estado de parada como falso
    pause_ = 0;                // Inicializa o estado de pausa como falso
    game_over = 0;             // Inicializa o estado de fim de jogo como falso
    accelered = 0;             // Inicializa o estado acelerado como falso
    refresh_time_accel = 2;    // Tempo de atualização quando acelerado (2 milissegundos)

    int KEY_data; // Variável para armazenar o estado do botão do DE1-SoC

    time_t t; // Variável para o tempo (usada para semente de rand)

    signal(SIGINT, catchSIGINT); // Associa a função catchSIGINT ao sinal SIGINT (Ctrl+C)
    srand((unsigned)time(&t));   // Inicializa o gerador de números aleatórios com a semente atual do tempo

    // Configura o MUX para conectar o controlador I2C0 ao acelerômetro ADXL345
    if ((fd_sysmgr = open_physical(fd_sysmgr)) == -1)  // Abre acesso físico ao System Manager
    {
        return (-1);  // Retorna erro se falhar
    }

    if ((sysmgrbase_virtual = map_physical(fd_sysmgr, SYSMGR_BASE, SYSMGR_SPAN)) == NULL) // Mapeia a memória do System Manager
    {
        return (-1);  // Retorna erro se falhar
    }

    sysmgr_base_ptr = (unsigned int *)sysmgrbase_virtual;  // Define o ponteiro para a base do System Manager

    if ((fd_i2c0base = open_physical(fd_i2c0base)) == -1)  // Abre acesso físico ao I2C0
    {
        return (-1);  // Retorna erro se falhar
    }

    if ((i2c0base_virtual = map_physical(fd_i2c0base, I2C0_BASE, I2C0_SPAN)) == NULL) // Mapeia a memória do I2C0
    {
        return (-1);  // Retorna erro se falhar
    }

    i2c0_base_ptr = (unsigned int *)i2c0base_virtual;  // Define o ponteiro para a base do I2C0
    printf("Begining I2C0 communication...\n");        // Inicia a comunicação com o I2C0
    I2C0_Init();                                       // Inicializa o controlador I2C0
    printf("Getting ID...\n");                         // Exibe mensagem para indicar que está lendo o ID do acelerômetro
    ADXL345_IdRead(&devid);                            // Lê o ID do dispositivo ADXL345
    printf("Device ID: %#x\n", devid);                 // Exibe o ID do dispositivo

    //////////////////////////////////////////////////////////TETRIS CODE ////////////////////////////////////////////////////////////////////

    // Abre os drivers de dispositivo para os botões e a saída de vídeo
    if (!KEY_open() || !video_open())
    {
        printf("Erro ao abrir drivers KEY ou video.\n");
        return -1; // Sai se não conseguir abrir os drivers
    }

    video_read(&screen_x, &screen_y, &char_x, &char_y); // Lê as dimensões da tela e do texto
    block_side = (screen_y * 0.8) / 24;                 // Calcula o tamanho lateral de cada bloco com base na tela

    video_clear();       // Limpa o buffer da tela (Back buffer)
    video_erase();       // Limpa o conteúdo atual da tela
    video_show();        // Mostra a tela limpa
    video_show();        // Mostra a tela limpa
    fill_matrix(matrix); // Preenche a matriz com blocos

    Tetris_Piece generated_Piece = gen_piece(matrix); // Gera uma peça na tela

    if (devid == 0xE5)  // Verifica se o ID do dispositivo corresponde ao acelerômetro ADXL345
    {
        ADXL345_init(devid);  // Inicializa o acelerômetro

        // Loop principal que continua até o usuário pressionar Ctrl+C ou o segundo botão
        while (!stop)
        {
            if (ADXL345_IsDataReady())  // Verifica se há novos dados prontos no acelerômetro
            {
                while (!stop)
                {
                    while (!colision && !stop)
                    {
                        ADXL345_XYZ_Read(XYZ);    // Lê os dados do acelerômetro nos eixos X, Y e Z

                        if (KEY_read(&KEY_data))  // Lê o estado dos botões do DE1-SoC
                        {
                            if (KEY_data & KEY1)  // Se o botão KEY1 for pressionado
                            {
                                pause_ = 1;       // Pausa o jogo
                            }
                            if (KEY_data & KEY3)  // Se o botão KEY3 for pressionado
                            {
                                stop = 1;         // Encerra o jogo
                            }
                        }

                        if (pause_)  // Verifica se o jogo está pausado
                        {
                            printf("Jogo pausado. Pressione o primeiro botão para continuar...\n");
                            draw_word_PAUSE_();          // Desenha "PAUSE" na tela
                            video_show();                // Mostra a tela atualizada com "PAUSE"
                            while ((pause_) && (!stop))  // Enquanto o jogo estiver pausado e não for finalizado
                            {
                                if (KEY_read(&KEY_data))  // Verifica se algum botão foi pressionado para sair da pausa
                                {
                                    if (KEY_data & KEY1)  // Se o botão KEY1 for pressionado novamente
                                    {
                                        pause_ = 0;       // Sai da pausa
                                    }
                                    if (KEY_data & KEY3)  // Se o botão KEY3 for pressionado
                                    {
                                        stop = 1;         // Encerra o jogo
                                    }
                                }
                                usleep(100000);           // Evita uso excessivo de CPU durante a pausa
                            }
                        }

                        // Movimenta a peça para baixo
                        generated_Piece = push_piece_down(generated_Piece, matrix);

                        // Movimenta a peça lateralmente com base nos valores do acelerômetro
                        if ((XYZ[0] * mg_per_lsb) > insensibility)
                        {
                            generated_Piece = push_piece_sides(true, generated_Piece, matrix);  // Move para a direita
                        }
                        if ((XYZ[0] * mg_per_lsb) < (insensibility * (-1)))
                        {
                            generated_Piece = push_piece_sides(false, generated_Piece, matrix);  // Move para a esquerda
                        }
                        if ((XYZ[1] * mg_per_lsb) > ((insensibility + 6) * (-1)))
                        {
                            accelered = 0;  // Ajusta para o tempo de atualização normal
                        }
                        if ((XYZ[1] * mg_per_lsb) <= ((insensibility + 6) * (-1)))
                        {
                            accelered = 1;  // Ajusta para o tempo de atualização acelerado
                        }                        

                        // Atualiza o jogo em intervalos regulares (mais rápido ou mais devagar dependendo da inclinação Y)
                        if(accelered) {
                            usleep(100000 * refresh_time_accel);
                        }
                        else
                        {
                            usleep(100000 * refresh_time);
                        }
                        
                        printf("\033[A\033[K"); // Limpa a saída anterior no console
                        printf("\033[A\033[K");

                        // Verifica se o jogo acabou (se as peças atingiram o topo)
                        if (check_game_over(matrix))
                        {
                            game_over = 1;
                            draw_word_GAME_OVER(); // Desenha "GAME OVER!" na tela
                            video_show();  // Atualiza a tela

                            // Loop para tratar o fim de jogo (aguarda o jogador reiniciar ou sair)
                            while (game_over && !stop)
                            {
                                if (KEY_read(&KEY_data))
                                {
                                    if (KEY_data & KEY2)  // Se o botão KEY2 for pressionado
                                    {
                                        game_over = 0;    // Reinicia o jogo
                                        total_score = 0;
                                        main();           // Reinicia chamando a função main() novamente
                                    }
                                    if (KEY_data & KEY3)  // Se o botão KEY3 for pressionado
                                    {
                                        stop = 1;         // Encerra o jogo
                                    }
                                }
                                usleep(100000);           // Espera 100ms antes de verificar novamente
                            }
                        }
                    }

                    // Reseta a colisão e gera uma nova peça
                    colision = 0;
                    current_piece_pos_j = 0;
                    current_piece_pos_i = 0;
                    generated_Piece = gen_piece(matrix);      // Gera uma nova peça
                    total_score += clear_full_lines(matrix);  // Limpa as linhas completas e atualiza a pontuação
                    lines_cleared = 0;
                }
            }
        }
        printf("Jogo encerrado.\n");  // Exibe uma mensagem ao encerrar o jogo
    }

    // Fecha os drivers de vídeo e do botão
    video_close();
    KEY_close();

    //////////////////////////////////////////////////////////TETRIS CODE END/////////////////////////////////////////////////////////////////

    // Limpeza da memória mapeada e fechamento de arquivos
    unmap_physical(i2c0base_virtual, I2C0_SPAN);
    close_physical(fd_i2c0base);
    unmap_physical(sysmgrbase_virtual, SYSMGR_SPAN);
    close_physical(fd_sysmgr);

    printf("\nExiting sample program\n");  // Exibe mensagem final de saída do programa
    return 0;
}



