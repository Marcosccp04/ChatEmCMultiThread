/*
Aluno: Marcos Augusto de Souza Pinto;
Mat: 21755102;
LPAV: TP10
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

void chatAtivo(char *string, int socketdc);
void construirMensagem(char *resultado, char *string, char *mensagem);
void configurarConexao(struct sockaddr_in *serverAddr, struct hostent *host, int socketdc, int porta);
void auxiliarBloqueio(int descritor);
void interromperH(int sinal);

static int socketdc;