#include <stdio.h>                        // Para funções de entrada/saída padrão
#include <time.h>                         // Para manipulação de data e hora
#include <stdint.h>                       // Para tipos de dados inteiros com tamanho definido (ex. uint8_t)
#include <unistd.h>                       // Para funções POSIX como usleep()
#include <signal.h>                       // Para manipular sinais como SIGINT (Ctrl+C)
#include <sys/mman.h>                     // Para mapear memória física
#include <string.h>                       // Para manipulação de strings
#include <fcntl.h>                        // Para controle de arquivos (open, close)
#include "../include/address_map_arm.h"   // Mapeamento de endereços físicos para DE1-SoC
#include "../include/graphics.h"          // Funções gráficas
#include "../include/input.h"             // Funções de entrada (acelerômetro e botões)
#include <intelfpgaup/KEY.h>              // Biblioteca para manipulação dos botões do DE1-SoC

// Variáveis globais que controlam o estado do jogo
extern volatile sig_atomic_t pause_, stop, game_over;

// Ponteiros para o mapeamento de memória física (I2C e System Manager)
extern unsigned int *i2c0_base_ptr;
extern unsigned int *sysmgr_base_ptr;
extern void *i2c0base_virtual;
extern void *sysmgrbase_virtual;
extern int fd_i2c0base;
extern int fd_sysmgr;

/**
 * @brief Abre o arquivo de mapeamento de memória física no Linux
 * 
 * @param fd int - Descritor de arquivo que será aberto
 * @return int - Retorna o descritor de arquivo, ou -1 se falhar
 */
int open_physical(int fd)
{
    if (fd == -1) // Se o descritor de arquivo ainda não estiver aberto
    {
        if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) // Tenta abrir o arquivo de memória física
        {
            printf("ERROR: could not open \"/dev/mem\"...\n"); // Exibe mensagem de erro se falhar
            return (-1);
        }
    }
    return fd; // Retorna o descritor de arquivo
}

/**
 * @brief Mapeia um endereço físico para um endereço virtual
 * 
 * @param fd int - Descritor de arquivo físico
 * @param base unsigned int - Endereço físico base a ser mapeado
 * @param span unsigned int - Intervalo de memória a ser mapeado
 * @return void* - Retorna o endereço virtual mapeado ou NULL se falhar
 */
void *map_physical(int fd, unsigned int base, unsigned int span)
{
    void *virtual_base;

    // Obtem um mapeamento de endereços físicos para endereços virtuais
    virtual_base = mmap(NULL, span, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, base);
    if (virtual_base == NULL) // Se o mapeamento falhar
    {
        printf("ERROR: mmap() failed...\n"); // Exibe uma mensagem de erro
        close(fd);
        return (NULL);
    }

    return virtual_base; // Retorna o endereço virtual mapeado
}

/**
 * @brief Desmapeia a memória física previamente mapeada
 * 
 * @param virtual_base void* - Endereço virtual mapeado
 * @param span unsigned int - Intervalo de memória a ser desmapeado
 * @return int - Retorna 0 se bem sucedido, -1 se falhar
 */
int unmap_physical(void *virtual_base, unsigned int span)
{
    if (munmap(virtual_base, span) != 0) // Chama munmap() para liberar o mapeamento
    {
        printf("ERROR: munmap() failed...\n"); // Exibe uma mensagem de erro se falhar
        return (-1);
    }
    return 0;
}

/**
 * @brief Fecha o arquivo de mapeamento de memória física
 * 
 * @param fd int - Descritor de arquivo a ser fechado
 */
void close_physical(int fd)
{
    close(fd); // Fecha o descritor de arquivo
}

/**
 * @brief Configura o MUX para conectar o ADXL345 ao controlador I2C0
 */
void mux_init()
{
    volatile unsigned int *gpio7_ptr, *gpio8_ptr, *i2c0fpga_ptr; // Ponteiros para o MUX

    gpio7_ptr = sysmgr_base_ptr + SYSMGR_GENERALIO7;  // Endereço do GPIO7
    gpio8_ptr = sysmgr_base_ptr + SYSMGR_GENERALIO8;  // Endereço do GPIO8
    i2c0fpga_ptr = sysmgr_base_ptr + SYSMGR_I2C0USEFPGA; // Endereço de controle do FPGA

    *i2c0fpga_ptr = 0;  // Configura o I2C0 para o HPS
    *gpio7_ptr = 1;     // Configura o pino 7 do GPIO para I2C0
    *gpio8_ptr = 1;     // Configura o pino 8 do GPIO para I2C0
    printf("gio7: %#x\ngio8: %#x\ni2c0fpga: %#x\n", *gpio7_ptr, *gpio8_ptr, *i2c0fpga_ptr);
}

/**
 * @brief Habilita ou desabilita o I2C0 usando polling
 * 
 * @param onoff unsigned int - Define se o I2C0 será habilitado (1) ou desabilitado (0)
 * @return int - Retorna 1 se o controlador foi habilitado/desabilitado corretamente, 0 se falhou
 */
int I2C0_onoff(unsigned int onoff)
{
    int ti2c_poll = 2.5;   // Tempo de polling (em ms)
    int MAX_T_POLL_COUNT = 100; // Número máximo de polls
    int poll_count = 0;
    int good = 0;

    *(i2c0_base_ptr + I2C0_ENABLE) = onoff;  // Habilita ou desabilita o I2C

    // Verifica o status de habilitação do I2C com polling
    while (((*(i2c0_base_ptr + I2C0_ENABLE_STATUS) & 0x1) == (onoff - 1)) & (poll_count < MAX_T_POLL_COUNT))
    {
        poll_count++;
        sleep(ti2c_poll);  // Atraso para evitar polling agressivo
        *(i2c0_base_ptr + I2C0_ENABLE) = onoff;  // Reconfirma a habilitação
    }
    if (poll_count < 10) // Se a habilitação ocorrer rapidamente, é um bom sinal
        good = 1;

    return good;
}

/**
 * @brief Inicializa o controlador I2C0 para comunicação com o ADXL345
 * 
 * @return int - Retorna 1 se o controlador foi inicializado corretamente, 0 se falhou
 */
int I2C0_Init()
{
    // Aborta transmissões e desabilita o controlador para configuração
    printf("%d\n%d\n", I2C0_TAR, I2C0_FS_SCL_HCNT * 1);
    printf("%p\n%p\n", i2c0_base_ptr, i2c0_base_ptr + (I2C0_FS_SCL_HCNT * 1));
    *(i2c0_base_ptr + I2C0_ENABLE) = 0x2;  // Coloca o I2C no estado de desativação

    // Verifica se o controlador foi desativado corretamente
    if (!(I2C0_onoff(2)))
    {
        printf("Unable to disable\n");  // Exibe mensagem de erro se falhar
        return 0;
    }
    printf("Configuring\n");

    // Configuração: 7 bits, FastMode (400kb/s), modo Master
    *(i2c0_base_ptr + I2C0_CON) = 0x65;

    // Define o endereço de destino (ADXL345) como 0x53 (7 bits)
    *(i2c0_base_ptr + I2C0_TAR) = 0x53;

    // Define os períodos de alta e baixa do sinal de clock (SCL)
    *(i2c0_base_ptr + I2C0_FS_SCL_HCNT) = 60 + 30; // Tempo de clock alto (Fast Mode)
    *(i2c0_base_ptr + I2C0_FS_SCL_LCNT) = 130 + 30; // Tempo de clock baixo (Fast Mode)

    // Habilita o controlador I2C
    printf("Enabling controller\n");
    if (!(I2C0_onoff(1)))  // Verifica se foi habilitado corretamente
    {
        printf("Unable to enable\n");
        return 0;
    }
    return 1;
}

/**
 * @brief Leitura de um único byte do ADXL345
 * 
 * @param address uint8_t - Endereço do registrador a ser lido
 * @param value uint8_t* - Ponteiro para armazenar o valor lido
 */
void ADXL345_REG_READ(uint8_t address, uint8_t *value)
{
    // Envia o endereço de leitura e o sinal de início
    *(i2c0_base_ptr + I2C0_DATA_CMD) = address + 0x400;

    // Envia o sinal de leitura
    *(i2c0_base_ptr + I2C0_DATA_CMD) = 0x100;

    // Espera pela resposta (espera até que os dados estejam prontos no buffer de recepção)
    while (*(i2c0_base_ptr + I2C0_RXFLR) == 0)
    {
        continue;
    }
    *value = *(i2c0_base_ptr + I2C0_DATA_CMD) & 0xFF; // Armazena o valor lido no ponteiro fornecido
}

/**
 * @brief Escrita de um único byte no ADXL345
 * 
 * @param address uint8_t - Endereço do registrador a ser escrito
 * @param value uint8_t - Valor a ser escrito no registrador
 */
void ADXL345_REG_WRITE(uint8_t address, uint8_t value)
{
    *(i2c0_base_ptr + I2C0_DATA_CMD) = address + 0x400;  // Envia o endereço do registrador
    *(i2c0_base_ptr + I2C0_DATA_CMD) = value;            // Envia o valor para escrever no registrador
}

/**
 * @brief Leitura de múltiplos bytes do ADXL345
 * 
 * @param address uint8_t - Endereço do registrador inicial
 * @param values uint8_t[] - Array para armazenar os valores lidos
 * @param len uint8_t - Número de bytes a serem lidos
 */
void ADXL345_REG_MULTI_READ(uint8_t address, uint8_t values[], uint8_t len)
{
    int i = 0;
    int nth_byte = 0;

    *(i2c0_base_ptr + I2C0_DATA_CMD) = address + 0x400;  // Envia o endereço inicial de leitura

    // Envia o sinal de leitura várias vezes para garantir a leitura de múltiplos bytes
    for (i = 0; i < len; i++)
        *(i2c0_base_ptr + I2C0_DATA_CMD) = 0x100;

    // Aguarda até que todos os bytes sejam lidos
    while (len)
    {
        if (*(i2c0_base_ptr + I2C0_RXFLR) > 0)  // Verifica se há dados prontos no buffer de recepção
        {
            values[nth_byte] = *(i2c0_base_ptr + I2C0_DATA_CMD) & 0xFF; // Armazena o byte lido
            nth_byte++;
            len--;
        }
    }
}

/**
 * @brief Inicializa o ADXL345
 * 
 * @param devid uint8_t - ID do dispositivo ADXL345
 * @return int - Retorna 1 se o ADXL345 foi inicializado corretamente, 0 se falhou
 */
int ADXL345_init(uint8_t devid)
{
    if (devid == 0xE5)  // Verifica se o dispositivo encontrado é o ADXL345
    {
        printf("Found Accelerometer\n");

        uint8_t data_format = 0x03;  // Define o formato de dados (+/-16g)
        uint8_t bw_rate = 0x07;      // Define a taxa de amostragem (100 Hz)

        ADXL345_REG_WRITE(ADXL345_DATA_FORMAT, data_format);  // Configura o formato de dados
        ADXL345_REG_WRITE(ADXL345_BW_RATE, bw_rate);          // Configura a taxa de amostragem

        // Configura thresholds e tempos para detecção de atividade/inatividade
        ADXL345_REG_WRITE(ADXL345_THRESH_ACT, 0x04);          // Threshold de atividade
        ADXL345_REG_WRITE(ADXL345_THRESH_INACT, 0x02);        // Threshold de inatividade
        ADXL345_REG_WRITE(ADXL345_TIME_INACT, 0x02);          // Tempo de inatividade
        ADXL345_REG_WRITE(ADXL345_ACT_INACT_CTL, 0xFF);       // Ativa controle de atividade/inatividade
        ADXL345_REG_WRITE(ADXL345_INT_ENABLE, 0x18);          // Habilita interrupções

        // Ativa o modo de medição
        ADXL345_REG_WRITE(ADXL345_POWER_CTL, 0x00); // Põe o dispositivo em standby
        ADXL345_REG_WRITE(ADXL345_POWER_CTL, 0x08); // Ativa o modo de medição
        return 1;
    }
    return 0;
}

/**
 * @brief Verifica se há novos dados prontos no ADXL345
 * 
 * @return int - Retorna 1 se os dados estiverem prontos, 0 se não estiverem
 */
int ADXL345_IsDataReady()
{
    int bReady = 0;
    uint8_t data8;

    ADXL345_REG_READ(ADXL345_INT_SOURCE, &data8); // Lê a fonte da interrupção
    if (data8 & ADXL345_ACTIVITY)  // Verifica se houve detecção de atividade
    {
        ADXL345_REG_READ(ADXL345_INT_SOURCE, &data8); // Lê novamente para verificar se os dados estão prontos
        if (data8 & ADXL345_DATAREADY)  // Verifica se os dados estão prontos
        {
            bReady = 1;  // Define que os dados estão prontos
        }
    }
    return bReady;
}

/**
 * @brief Lê os dados de aceleração dos três eixos (X, Y, Z)
 * 
 * @param szData16 int16_t[3] - Array para armazenar os dados de aceleração lidos
 */
void ADXL345_XYZ_Read(int16_t szData16[3])
{
    uint8_t szData8[6];  // Array para armazenar os dados lidos (6 bytes: 2 por eixo)
    ADXL345_REG_MULTI_READ(0x32, (uint8_t *)&szData8, sizeof(szData8)); // Lê os valores de X, Y e Z

    // Combina os dois bytes de cada eixo para formar os valores completos (16 bits)
    szData16[0] = (szData8[1] << 8) | szData8[0];  // Eixo X
    szData16[1] = (szData8[3] << 8) | szData8[2];  // Eixo Y
    szData16[2] = (szData8[5] << 8) | szData8[4];  // Eixo Z
}

/**
 * @brief Lê o ID do dispositivo ADXL345
 * 
 * @param pId uint8_t* - Ponteiro para armazenar o ID do dispositivo
 */
void ADXL345_IdRead(uint8_t *pId)
{
    ADXL345_REG_READ(ADXL345_DEVID, pId);  // Lê o ID do registrador ADXL345_DEVID
}



