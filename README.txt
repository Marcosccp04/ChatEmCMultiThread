A) Este trabalho foi implementado utilizando o conceito de TAD, para executar é necessário apenas compilar os arquivos servidorMain.c e o clienteMain.C:
    Porta para entrar no servidor: 5555

B)Instruções para executar este trabalho:
 Para compilar o servidor execute o comando (estando na pasta do servidormain.c) pelo terminal : gcc servidorMain.c -o servidor -lpthread
 Para compilar o cliente execute o comando (estando na pasta do cleinte.c) pelo terminal : gcc clienteMain.c -o cliente
 Para executar o servidor, basta executar o comando (estando na pasta do servidor) pelo terminal: ./servidor
 Para executar o servidor vc precisa do nome do user, do hostname e da porta: ./cliente nome hostame porta.
    exemplo: ./cliente Marcos lab30 5555
 O porta padrão é a 5555.
 Para desconetar um user, digite no chat \sair.   
