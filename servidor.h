/*
Aluno: Marcos Augusto de Souza Pinto;
Mat: 21755102;
LPAV: TP10
*/
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <netdb.h>
# include <sys/select.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <pthread.h>


typedef struct{
	char *buffer[1024];
	int inicio; 
	int fim;
	int lotada; 
	int vazia;
	pthread_mutex_t *mutex; //Estrutura para o Mutex criada.
	pthread_cond_t *quaseCheia, *quaseVazia;
} lista;

typedef struct{
	fd_set descritores;
	int socketFd;
	int socketCliente [1024];
	int quantidadeClientes;
	pthread_mutex_t *ListaClientesMutex;
	lista *lista;
}dadosChat;

typedef struct{
	dadosChat *dados;
	int socketClienteFd;
}clienteHandle;

void *tratamentoMensagem (void *dados);
void *clienteHand(void *chv);
void *clienteNovo(void *dados);
void removeSocket(dadosChat *dados, int clienteSocket);
void bindSocket( struct sockaddr_in *serverAddr, int socketFd, int porta);
char* popFila(lista *lista);
void pushFila(lista *lista, char* mensagem);
void destroiLista(lista *lista);
void iniciaChat(int socketFd);
lista* iniciaLista(void);