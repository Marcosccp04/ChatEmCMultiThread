/*
Aluno: Marcos Augusto de Souza Pinto;
Mat: 21755102;
LPAV: TP10
*/

# include "servidor.h"

void iniciaChat(int socketFd){
 dadosChat dados;
 dados.quantidadeClientes = 0;
 dados.socketFd = socketFd;
 dados.lista = iniciaLista();
 dados.ListaClientesMutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
 pthread_t conexao;
 if((pthread_create(&conexao,NULL,(void*)&clienteNovo,(void *)&dados))== 0){
 	fprintf(stderr,"Conexao iniciada!\n");
 }
 FD_ZERO(&(dados.descritores));
 FD_SET(socketFd,&(dados.descritores));

 pthread_t mensagen;
 
 if((pthread_create(&mensagen,NULL,(void*)&tratamentoMensagem,(void*)&dados)) == 0){
 	fprintf(stderr, "Pode iniciar as mensagens!\n");
 }
 pthread_join(conexao,NULL);
 pthread_join(mensagen,NULL);

 destroiLista(dados.lista);
 pthread_mutex_destroy(dados.ListaClientesMutex);
 free(dados.ListaClientesMutex);
}

lista* iniciaLista(void){
 lista *l = (lista*)malloc(sizeof(lista));
 if(l == NULL){
 	perror("falta memoria!\n");
 	exit(1);
 }
 l->vazia = 1;
 l->lotada = 0;
 l->inicio = 0;
 l->fim = 0;
 l->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
 if(l->mutex == NULL){
 	perror("Falta memoria!");
 	exit(1);
 }
 pthread_mutex_init(l->mutex, NULL);
 l->quaseCheia = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
 if(l->quaseCheia == NULL){
 	perror("Falta Memoria!");
 	exit(1);
 }
 pthread_cond_init(l->quaseCheia,NULL);
 l->quaseVazia = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
 if(l->quaseVazia == NULL){
	 perror ("Falta memoria!");
	 exit(1);
 }
 return l;
}

void destroiLista(lista *l){
	pthread_mutex_destroy(l->mutex);
	pthread_cond_destroy(l->quaseCheia);
	pthread_cond_destroy(l->quaseVazia);
	free(l->mutex);
	free(l->quaseCheia);
	free(l->quaseVazia);
	free(l);
}
void pushFila(lista *l, char* mensagem){
	l->buffer[l->fim] = mensagem;
	l->fim++;
	if(l->fim == 1024) l->fim = 0;
	if(l->fim == l->inicio) l->lotada = 1;
	l->vazia = 0;
}

char* popFila(lista *l){
	char* mensagem = l->buffer[l->inicio];
	l->inicio++;
	if(l->inicio == 1024) l->inicio = 0;
	if(l->inicio == l->fim) l->vazia = 1;
	l->lotada = 0;

	return mensagem;
}

void bindSocket( struct sockaddr_in *serverAddr, int socketFd, int porta){
	memset(serverAddr, 0, sizeof(*serverAddr));
	serverAddr->sin_family = AF_INET;
	serverAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr->sin_port = htons(porta);

	if(bind(socketFd,(struct sockaddr *)serverAddr,sizeof(struct sockaddr_in)) == -1){
		perror("Socket bind Falhou!");
		exit(1);
	}
}

void removeSocket(dadosChat *dados, int clienteSocket){
	pthread_mutex_lock(dados->ListaClientesMutex);
	for (int contador = 0; contador < 1024; contador++){
		if(dados->socketCliente[contador] == clienteSocket){
			dados->socketCliente[contador] = 0;
			close(clienteSocket);
			dados->quantidadeClientes = dados->quantidadeClientes - 1;
			contador = 1024;
		}
	}
	pthread_mutex_unlock(dados->ListaClientesMutex);
}

void *clienteNovo(void *dados){
	dadosChat *cDados = (dadosChat *) dados;
	while(1){
		int clienteSocket = accept(cDados->socketFd, NULL, NULL);
		if(clienteSocket > 0){
			fprintf(stderr, "Novo cliente aceito\n");
			pthread_mutex_lock(cDados->ListaClientesMutex);
			if(cDados->quantidadeClientes < 1024){
				for(int contador = 0; contador < 1024; contador++){
					if(!FD_ISSET(cDados->socketCliente[contador], &(cDados->descritores))){
						cDados->socketCliente[contador] = clienteSocket;
						contador = 1024;
					}
				}
				FD_SET(clienteSocket, &(cDados->descritores));
				clienteHandle chv;
				chv.socketClienteFd = clienteSocket;
				chv.dados = cDados;
				pthread_t clienteThread;
				if((pthread_create(&clienteThread, NULL, (void *)&clienteHand,(void *)&chv)) == 0){
					cDados->quantidadeClientes++;
					fprintf(stderr, "Cliente entrou no chat\n");
				}else close(clienteSocket);
			}			
			pthread_mutex_unlock(cDados->ListaClientesMutex);
		}
	}
}

void *clienteHand(void *chv){
	clienteHandle *var = (clienteHandle *)chv;
	dadosChat *dados = (dadosChat *) var->dados;
	lista *l = dados->lista;
	int clienteSocket = var->socketClienteFd;
	char msgBuffer[1024];
	while(1){
		int quantidadeBytes = read(clienteSocket, msgBuffer,1024 - 1);
		msgBuffer[quantidadeBytes] = '\0';
		if(strcmp(msgBuffer, "/sair\n") == 0){
			fprintf(stderr,"Cliente abandonou o chat.\n");
			removeSocket(dados, clienteSocket);
			return NULL;
		}else{
			while(l->lotada){
				pthread_cond_wait(l->quaseCheia, l->mutex);
			}
			pthread_mutex_lock(l->mutex);
			fprintf(stderr, "mensagem a ser enviada de %s\n", msgBuffer);
			pushFila(l,msgBuffer);
			pthread_mutex_unlock(l->mutex);
			pthread_cond_signal(l->quaseVazia);
		}
	}
}

void *tratamentoMensagem (void *dados){
	dadosChat *cDados = (dadosChat *)dados;
	lista *l = cDados->lista;
	int *clienteSocket = cDados->socketCliente;

	while(1){
		pthread_mutex_lock(l->mutex);
		while(l->vazia){
			pthread_cond_wait(l->quaseVazia, l->mutex);
		}
		char* mensagem = popFila(l);
		pthread_mutex_unlock(l->mutex);
		pthread_cond_signal(l->quaseCheia);
		//fprintf(stderr,"Broadcasting message: %s\n",mensagem);
		for(int contador = 0; contador < cDados->quantidadeClientes;contador++){
			int socket = clienteSocket[contador];
			if(socket != 0 && write(socket,mensagem,1024 -1) == -1)
				perror("falha com o socket\n");
		}
	}
}


















