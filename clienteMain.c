/*
Aluno: Marcos Augusto de Souza Pinto;
Mat: 21755102;
LPAV: TP10
*/


# include "clienteFuncoes.c"

int main(int argc, char *argv[])
{
    char *string;
    struct sockaddr_in serverAddr;
    struct hostent *host;
    int porta;

    if(argc != 4)
    {
        fprintf(stderr, "./cliente [Nome] [Host] [Porta]\n");
        exit(1);
    }
    string = argv[1];
    if((host = gethostbyname(argv[2])) == NULL)
    {
        fprintf(stderr, "Couldn't get host name\n");
        exit(1);
    }
    porta = strtol(argv[3], NULL, 0);
    if((socketdc = socket(AF_INET, SOCK_STREAM, 0))== -1)
    {
        fprintf(stderr, "Couldn't create socket\n");
        exit(1);
    }

    configurarConexao(&serverAddr, host, socketdc, porta);
    auxiliarBloqueio(socketdc);
    auxiliarBloqueio(0);

    //Set a handler for the interrupt signal
    signal(SIGINT, interromperH);

    chatAtivo(string, socketdc);
}