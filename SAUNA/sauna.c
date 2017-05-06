#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "pedido.h"

#define RECEBIDO 0
#define REJEITADO 1
#define SERVIDO 2
#define TIMEOUT 5

const char entrada[] = "entrada";
const char rejeitados[] = "rejeitados";
const char * messageTip[] = {"RECEBIDO", "REJEITADO", "SERVIDO"};
char fich[30] = "/tmp/bal.";
int pedidosAtuais = 0;
int processados = 0;

FILE * f;

clock_t begin;

pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pedidos_lock = PTHREAD_MUTEX_INITIALIZER;

void* open_file_sauna(){
	void* result;
	int ft;

	if((ft=open(fich, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0600)) == -1){
		printf("%s \n",fich);
		printf("Sauna: Couldnt open file\n");
		return NULL;
	}

	if((result = fdopen(ft, "w")) == NULL){
		printf("Sauna: Couldnt open file\n");
		return NULL;
	}
	return result;
}

int open_fifo_entrada(){
	int fd;

	if((fd = open("/tmp/entrada",O_RDONLY)) == -1){
		printf("Sauna: Couldnt open fifo\n");
		return -1;
	}
	return fd;
}

int create_fifo_entrada(){

	mode_t permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP; //Read and write for file owner and group owner
	if(mkfifo("/tmp/entrada",permissions)!=0){
		if(errno != EEXIST) { //EEXIST would mean that couldn't make FIFO but only because it already exists
			printf("Sauna: Couldnt create fifo \n");
			return -1;
		}
	}
	return 0;
}

int readPedidosNo(int fd){
	int pedidosNo;
	if(read(fd,&pedidosNo,sizeof(int)) == -1){
		printf("Sauna: Falhou ler pedidos\n");
		return -1;
	}
	return pedidosNo;
}

int open_fifo_rejeitado(){
	int i, fd2;
	for(i = 0; i < TIMEOUT; i++){
		if((fd2 = open("/tmp/rejeitados", O_WRONLY)) == -1)
			sleep(1);
		else
			break;
	}

	if(i == TIMEOUT){
		printf("Sauna: Couldnt open fifo\n");
		return -1;
	}
	return fd2;
}


void* saunar(void * pedido){
	struct Pedido p = *(struct Pedido *) pedido;

	usleep(p.time);

	pthread_mutex_lock(&pedidos_lock);
	pedidosAtuais--;
	pthread_mutex_unlock(&pedidos_lock);

	return NULL;
}

void printMessage(pid_t pid, pthread_t tid, struct Pedido p, unsigned int tip){
	clock_t end;
	double t_dif;
	end = clock();
	t_dif = (double) (end-begin)/CLOCKS_PER_SEC;
	fprintf(f,"%f - %u - %u - %i: %c - %d - %s\n", t_dif, (unsigned int) pid, (unsigned int) tid, p.id, p.g, p.time, messageTip[tip]);
}

void rejectPedido(struct Pedido * p, int fd){
	write(fd, p, sizeof(struct Pedido));
	if(p->rNo == 2)
		processados++;
	printMessage(getpid(), pthread_self(),*p,REJEITADO);
}

int main(int argc, char*argv[]){
	if(argc != 2)
		return 1;

	begin = clock();     //start counting time

	int fd, fd2, i, pedidosNo;

	char pid[20];
	sprintf(pid, "%u", (unsigned int) getpid());
	strcat(fich,pid);

	if((f = open_file_sauna()) == NULL)
		exit(1);

	create_fifo_entrada();

	if((fd = open_fifo_entrada()) == -1)
		exit(1);

	if((pedidosNo = readPedidosNo(fd)) == -1)
		exit(1);

	if ((fd2= open_fifo_rejeitado()) == -1)
		exit(1);

	char gender;
	int pedidosMax = atoi(argv[1]);
	pthread_t tid[pedidosNo];
	unsigned int tidIndex = 0;
	struct Pedido pedidosAtivos[pedidosNo];
	struct Pedido p;
	int br;

	struct Pedido* pedidoEspera =  (struct Pedido *) malloc(sizeof(struct Pedido));
	int flagEspera = 0;
	int pedidos;

	while(1){
		pthread_mutex_lock(&pedidos_lock);
		pedidos= pedidosAtuais;
		pthread_mutex_unlock(&pedidos_lock);

		if(pedidos == pedidosMax){
			if(flagEspera == 0){
				if((br = read(fd,&p,sizeof(struct Pedido)))==-1){
					printf("Sauna: Couldnt read pedido\n");
					exit(1);
				}

				if(br==0)
					break;

				printMessage(getpid(), pthread_self(),p,RECEBIDO);

				if(gender == p.g){
					*pedidoEspera = p;
					flagEspera=1;
				}
				else {
					rejectPedido(&p, fd2);
				}
			}
		}
		else  if(pedidos == 0){
			if(flagEspera == 1){
				gender = p.g;
				pedidosAtivos[pedidoEspera->id] = *pedidoEspera;
				tid[tidIndex] = tidIndex;
				pthread_mutex_lock(&pedidos_lock);
				pedidosAtuais++;
				pthread_mutex_unlock(&pedidos_lock);
				processados++;
				pthread_create(&tid[tidIndex], NULL, saunar, &pedidosAtivos[pedidoEspera->id]);
				printMessage(getpid(), tid[tidIndex],p,SERVIDO);
				tidIndex++;
				flagEspera=0;
			} else {
				if((br = read(fd,&p,sizeof(struct Pedido)))==-1){
					printf("Sauna: Couldnt read pedido\n");
					exit(1);
				}

				if(br==0)
					break;

				printMessage(getpid(), pthread_self(),p,RECEBIDO);

				gender = p.g;
				pedidosAtivos[p.id] = p;
				tid[tidIndex] = tidIndex;
				pthread_mutex_lock(&pedidos_lock);
				pedidosAtuais++;
				pthread_mutex_unlock(&pedidos_lock);
				processados++;
				pthread_create(&tid[tidIndex], NULL, saunar, &pedidosAtivos[p.id]);
				printMessage(getpid(), tid[tidIndex],p,SERVIDO);
				tidIndex++;
			}
		}
		else if (pedidos < pedidosMax){
			if(flagEspera == 1){
				pedidosAtivos[pedidoEspera->id] = *pedidoEspera;
				tid[tidIndex] = tidIndex;
				pthread_mutex_lock(&pedidos_lock);
				pedidosAtuais++;
				pthread_mutex_unlock(&pedidos_lock);
				processados++;
				pthread_create(&tid[tidIndex], NULL, saunar, &pedidosAtivos[pedidoEspera->id]);
				printMessage(getpid(), tid[tidIndex],p,SERVIDO);
				tidIndex++;
				flagEspera=0;
			}
			else{
				if((br = read(fd,&p,sizeof(struct Pedido)))==-1){
					printf("Sauna: Couldnt read pedido\n");
					exit(1);
				}

				if(br==0)
					break;

				printMessage(getpid(), pthread_self(),p,RECEBIDO);
				if(gender == p.g){
					pedidosAtivos[p.id] = p;
					tid[tidIndex] = tidIndex;
					pthread_mutex_lock(&pedidos_lock);
					pedidosAtuais++;
					pthread_mutex_unlock(&pedidos_lock);
					processados++;
					pthread_create(&tid[tidIndex], NULL, saunar, &pedidosAtivos[p.id]);
					printMessage(getpid(), tid[tidIndex],p,SERVIDO);
					tidIndex++;
				}
				else{
					rejectPedido(&p, fd2);
				}
			}
		}

		if(processados == pedidosNo){
			close(fd2);
			break;
		}
	}
	free(pedidoEspera);
	close(fd);
	printf("Sauna: processados: %d\n", processados);
	pthread_exit(NULL);

	//fprintf(f,"Recebidos: %d(T) - %d(M) - %d(F)\nRejeitados: %d(T) - %d(M) - %d(F)\nServidos: %d(T) - %d(M) - %d(F)",stats[0]+stats[1],stats[0],stats[1],stats[2]+stats[3],stats[2],stats[3],stats[4]+stats[5],stats[4],stats[5]);
	//fclose(f);

	return 0;
}
