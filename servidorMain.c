/*
Aluno: Marcos Augusto de Souza Pinto;
Mat: 21755102;
LPAV: TP10
*/

# include "servidorFuncoes.c"
int main(int argc, char *argv[]){
    struct sockaddr_in serverAddr;
    int porta = 5555;
    int socketdc;

    if(argc == 2) porta = strtol(argv[1], NULL, 0);

    if((socketdc = socket(AF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("Falha no socket");
        exit(1);
    }

    bindSocket(&serverAddr, socketdc, porta);
    if(listen(socketdc, 1) == -1)
    {
        perror("falha, finalizando chat....");
        exit(1);
    }

    iniciaChat(socketdc);
    
    close(socketdc);
}