.section .data
MEM_FD: .asciz "/dev/mem"              @ Caminho para o arquivo /dev/mem
ALT_LWFPGASLVS_OFST: .word 0xff200     @ Endereço da ponte FPGA
ALT_LWFPGASLVS_SPAN: .word 0x1000      @ Tamanho da região mapeada
ADDRESS_MAPPED: .space 4                @ Endereço virtual mapeado
ADDRESS_FD: .space 4                    @ Descritor de arquivo para /dev/mem
dataA: .word 0x80
dataB: .word 0x70
mensagem:       .asciz   "erro\n"

@ Mensagens de debug
DBG_OPEN: .asciz "Arquivo /dev/mem aberto com sucesso.\n"
DBG_MAP: .asciz "FPGA mapeado com sucesso.\n"
DBG_DRAW: .asciz "Desenhando quadrado 8x8 na posição.\n"
DBG_DONE: .asciz "Operação concluída.\n"

 .section .rodata
msg_mmap_error: .asciz "Mapping error\n"
msg_mmap_sucess: .asciz "Successful mapping\n"
mmsg_mmap_error2: .asciz "Mmap error\n"

.section .text
.global open_men_vgafpga
.type open_men_vgafpga, %function

.global mapping_fpga
.type mapping_fpga, %function

.global unmapping_fpga
.type unmapping_fpga, %function

.global draw_square_8x8
.type draw_square_8x8, %function

.global draw_square_16x16
.type draw_square_16x16, %function

.global erase_screen
.type erase_screen, %function

 .global is_button_pressed
 .type is_button_pressed, %function

@ Função: open_men_vgafpga
open_men_vgafpga:


    BX lr

@ Função: mapping_fpga
mapping_fpga:
    sub sp, sp, #32
    str lr, [sp, #28]
    str r7, [sp, #24]
    str r5, [sp, #20]
    str r4, [sp, #16]
    str r3, [sp, #12]
    str r2, [sp, #8]
    str r1, [sp, #4]
    str r0, [sp, #0]


    @abre o arquivo /dev/mem
    MOV r7, #5 @ syscall open
    LDR r0, =MEM_FD @ caminho do arquivo
    MOV r1, #2 @ para leitura e escrita 
    MOV r2, #0 @ sem flags
    SWI 0 @ chama o sistema para executar

    ldr r1, =ADDRESS_FD
    str r0, [r1]
    mov r4, r0 @guarda em r4

    @ configurar o mmap
    mov r7, #192 @ syscall do mmap2
    mov r0, #0 @ para deixar o kernel decidir o enderço virtual
    ldr r1, =ALT_LWFPGASLVS_SPAN @ tamanho da pagina
    ldr r1, [r1]
    mov r2, #3 @ leitura/escrita
    mov r3, #1 @ compartilhado com outros processos
    ldr r5, =ALT_LWFPGASLVS_OFST @carrega o endereço base da FPGA 
    ldr r5, [r5] @ carrega o valor real do enderço da FPGA
    SWI 0 @ kernel é chamado para executar a syscall

    ldr r1, =ADDRESS_MAPPED @endereco e carregado aqui
    str r0, [r1]

    ldr lr, [sp, #28]
    ldr r11, [sp, #24]
    ldr r7, [sp, #20]
    ldr r5, [sp, #16]
    ldr r3, [sp, #12]
    ldr r2, [sp, #8]
    ldr r1, [sp, #4]
    ldr r0, [sp, #0]
    add sp, sp, #32

    bx lr

@ Função: draw_square_8x8
draw_square_8x8:
    sub sp, sp, #24
    str lr, [sp, #20]
    str r3, [sp, #16]
    str r7, [sp, #12]
    str r2, [sp, #8]
    str r1, [sp, #4]
    str r0, [sp, #0]
    ldr r3, =ADDRESS_MAPPED
    ldr r3, [r3]

    @Desenha o primeiro bloco
    bl wait_wr_full

    @Zera o start
    mov r0, #0
    strd r0, [r3, #0xc0]

    @dataA
    mov r0, #0b0010 @opcode -> WBM
    ldr r1, [sp, #0] @Bloco escolhido
    lsl r1, r1, #4
    add r1, r1, r0
    str r1, [r3, #0x80]

    @dataB
    ldr r2, [sp, #4] @RGB
    str r2, [r3, #0x70]

    @Inicia o start para escrever na tela
    mov r0, #1
    strd r0, [r3, #0xc0]



    BX lr                            


@ Função: draw_square_16x16
draw_square_16x16:
    @ PUSH {lr}                 @ Salva o link register
    @ MOV r2, r1                @ Copia a cor para r2 (a ser usado em draw_square_8x8)
    
    @ BL draw_square_8x8        @ desenha o primeiro quadrado com posição em r0 e cor em r2
    @ ADD r0, r0, #1            @ Incrementa a posição
    @ BL draw_square_8x8        @ desenha o segundo quadrado
    
    @ ADD r0, r0, #79           @ Incrementa a posição para o próximo bloco
    @ BL draw_square_8x8        @ desenha o terceiro quadrado
    
    @ ADD r0, r0, #1            @ Incrementa a posição para o último bloco
    @ BL draw_square_8x8        @ desenha o quarto quadrado
    
    @ POP {lr}                  @ Restaura o link register
    @ BX lr                     @ Retorna da função

    sub sp, sp, #24
    str lr, [sp, #20]
    str r3, [sp, #16]
    str r7, [sp, #12]
    str r2, [sp, #8]
    str r1, [sp, #4]
    str r0, [sp, #0]
    ldr r3, =ADDRESS_MAPPED
    ldr r3, [r3]

    @Desenha o primeiro bloco
    bl wait_wr_full

    @Zera o start
    mov r0, #0
    strd r0, [r3, #0xc0]

    @dataA
    mov r0, #0b0010 @opcode -> WBM
    ldr r1, [sp, #0] @Bloco escolhido
    lsl r1, r1, #4
    add r1, r1, r0
    str r1, [r3, #0x80]

    @dataB
    ldr r2, [sp, #4] @RGB
    str r2, [r3, #0x70]

    @Inicia o start para escrever na tela
    mov r0, #1
    strd r0, [r3, #0xc0]

    @Desenhar o segundo bloco
    bl wait_wr_full

    @Zera o start
    mov r0, #0
    strd r0, [r3, #0xc0]

    @dataA
    mov r0, #0b0010 @opcode -> WBM
    ldr r1, [sp, #0] @Bloco escolhido
    add r1, r1, #1
    lsl r1, r1, #4
    add r1, r1, r0
    str r1, [r3, #0x80]

    @dataB
    ldr r2, [sp, #4] @RGB
    str r2, [r3, #0x70]

    @Inicia o start para escrever na tela
    mov r0, #1
    strd r0, [r3, #0xc0]

    @Desenhar o terceiro bloco
    bl wait_wr_full

    @Zera o start
    mov r0, #0
    strd r0, [r3, #0xc0]

    @dataA
    mov r0, #0b0010 @opcode -> WBM
    ldr r1, [sp, #0] @Bloco escolhido
    add r1, r1, #80
    lsl r1, r1, #4
    add r1, r1, r0
    str r1, [r3, #0x80]

    @dataB
    ldr r2, [sp, #4] @RGB
    str r2, [r3, #0x70]

    @Inicia o start para escrever na tela
    mov r0, #1
    strd r0, [r3, #0xc0]

    @Desenhar o quarto bloco
    bl wait_wr_full

    @Zera o start
    mov r0, #0
    strd r0, [r3, #0xc0]

    @dataA
    mov r0, #0b0010 @opcode -> WBM
    ldr r1, [sp, #0] @Bloco escolhido
    add r1, r1, #81
    lsl r1, r1, #4
    add r1, r1, r0
    str r1, [r3, #0x80]

    @dataB
    ldr r2, [sp, #4] @RGB
    str r2, [r3, #0x70]

    @Inicia o start para escrever na tela
    mov r0, #1
    strd r0, [r3, #0xc0]

    ldr lr, [sp, #20]
    ldr r3, [sp, #16]
    ldr r7, [sp, #12]
    ldr r2, [sp, #8]
    ldr r1, [sp, #4]
    ldr r0, [sp, #0]
    add sp, sp, #24

    bx lr


@ Função: unmapping_fpga
unmapping_fpga:
  @salva o valor dos registradores na pilha
  sub sp, sp, #12
  str r0, [sp, #8]
  str r1, [sp, #4]
  str r7, [sp, #0]

  ldr r0, =ADDRESS_MAPPED
  ldr r0, [r0]
  mov r1, #4096           @tamanho da página mapeada
  mov r7, #91             @system call: munmap
  SWI 0 @ chama o sistema para executar
  
  ldr r0, =ADDRESS_FD
  ldr r0, [r0]
  mov r7, #6              @system call: close
  SWI 0 @ chama o sistema para executar
  
  @carrega o valor dos registradores da pilha
  ldr r0, [sp, #8]
  ldr r1, [sp, #4]
  ldr r7, [sp, #0]

  add sp, sp, #12

  bx lr

@ Função: erase_screen
erase_screen:
    sub sp, sp, #28
    str lr, [sp, #24]
    str r3, [sp, #20]
    str r5, [sp, #16]
    str r4, [sp, #12]
    str r2, [sp, #8]
    str r1, [sp, #4]
    str r0, [sp, #0]
    ldr r3, =ADDRESS_MAPPED
    ldr r3, [r3]

    mov r4, #4800 @número de linhas
    mov r5, #0b0 @contador de pixel
erase_loop:
    bl wait_wr_full
    @reset
    mov r0, #0
    strd r0, [r3, #0xc0]

    @dataA
    mov r0, #0b0010 @opcode -> WBM
    mov r1, r5 @Bloco escolhido
    lsl r1, r1, #4
    add r1, r1, r0
    str r1, [r3, #0x80]

    @dataB
    mov r0, #0b000 @R
    mov r1, #0b000 @G
    mov r2, #0b000 @B
    lsl r2, r2, #6
    lsl r1, r1, #3
    add r2, r2, r1
    add r2, r2, r0
    str r2, [r3, #0x70]

    @Inicia o start para escrever na tela
    mov r0, #1
    strd r0, [r3, #0xc0]

    add r5, r5, #0b1 @Incrementar 1 no valor que representa o pixel

    sub r4, r4, #1
    cmp r4, #0
    bgt erase_loop

    ldr lr, [sp, #24]
    ldr r3, [sp, #20]
    ldr r5, [sp, #16]
    ldr r4, [sp, #12]
    ldr r2, [sp, #8]
    ldr r1, [sp, #4]
    ldr r0, [sp, #0]
    add sp, sp, #28

    bx lr                    @ retorna

wait_wr_full:
    ldr r0, [r3, #0xb0]
    cmp r0, #0
    bne wait_wr_full

    bx lr

    @ LDR r6, =ADDRESS_MAPPED
    @ LDR r6, [r6]
    @ LDR r5, [r6, #0xb0]
    @ CMP r5, #1              @Testa o bit 0 de r0 (bit de status do buffer)
    @ BEQ wait_loop   
    @ BX lr

is_button_pressed: @botão 1 = 7 // botão 2 = 11 // botão 3 = 13 // botão 4 = 14 // nada = 15
 sub sp, sp, #4
 str r1, [sp, #0]

 ldr r1, =ADDRESS_MAPPED
 ldr r1, [r1]
 ldr r0, [r1, #0x0]

 ldr r1, [sp, #0]
 add sp, sp, #4
 
 bx lr

