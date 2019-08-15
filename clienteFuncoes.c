/*
Aluno: Marcos Augusto de Souza Pinto;
Mat: 21755102;
LPAV: TP10
*/

# include "cliente.h"

void chatAtivo(char *string, int socketdc){
    fd_set cliente;
    char mensagemChat[1024];
    char bufferChat[1024];
    char bufferMensagem[1024];
    while(1){
        FD_ZERO(&cliente);
        FD_SET(socketdc, &cliente);
        FD_SET(0,&cliente);
        if(select(FD_SETSIZE,&cliente,NULL,NULL,NULL) != -1){
            for(int contador = 0; contador < FD_SETSIZE; contador++){
                if(FD_ISSET(contador, &cliente)){
                    if(contador == socketdc){
                        int quantidadeBytes = read(socketdc, bufferMensagem, 1024-1);
                        bufferMensagem[quantidadeBytes] = '\0';
                        printf("%s",bufferMensagem);
                        memset(&bufferMensagem,0,sizeof(bufferMensagem));
                    }else if(contador == 0){
                        fgets(bufferChat,1024-1,stdin);
                        if(strcmp(bufferChat,"/sair\n") == 0)
                            interromperH(-1);
                        else{
                            construirMensagem(mensagemChat,string,bufferChat);
                            if(write(socketdc,mensagemChat,1024-1) == -1) perror ("falha de escrita!");
                            memset(&bufferChat,0,sizeof(bufferChat));
                        }
                    
                    }
                }
            }
        }
    }
}

void construirMensagem(char *resultado, char *string, char *mensagem){
    memset(resultado, 0 , 1024);
    strcpy(resultado, string);
    strcat(resultado, ": ");
    strcat(resultado, mensagem);
}

void configurarConexao(struct sockaddr_in *serverAddr, struct hostent *host, int socketdc, int porta){
    memset(serverAddr, 0, sizeof(serverAddr));
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr = *((struct in_addr *) host->h_addr_list[0]);
    serverAddr->sin_port = htons(porta);
    if(connect(socketdc, (struct sockaddr *) serverAddr, sizeof(struct sockaddr)) < 0){
        perror ("impossivel se conectar!");
        exit(1);
    }
}

void auxiliarBloqueio(int descritor){
    int verificador = fcntl(descritor, F_GETFL);
    if(verificador < 0)
        perror("fcntl failed");

    verificador |= O_NONBLOCK;
    fcntl(descritor, F_SETFL, verificador);
}

void interromperH(int sinal){
    if(write(socketdc, "/sair\n", 1024 - 1) == -1)
        perror("write failed: ");

    close(socketdc);
    exit(1);
}






















































