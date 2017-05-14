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
unsigned int pedidosAtuais = 0;
unsigned int processados = 0;
unsigned int stats[6];

FILE * f;

//clock_t begin;
struct timespec begin;

pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pedidos_lock = PTHREAD_MUTEX_INITIALIZER;

void* open_file_sauna();

int open_fifo_entrada();

int create_fifo_entrada();

int readPedidosNo(int fd);

int open_fifo_rejeitado();

int read_real_time_fifo(int fd);

void* saunar(void * pedido);

void printMessage(pid_t pid, pthread_t tid, struct Pedido p, unsigned int tip);

void rejectPedido(struct Pedido * p, int fd);

int main(int argc, char*argv[]){
	if(argc != 2)
		return 1;

	/*
	if(clock_gettime(CLOCK_REALTIME, &begin)){    //start counting time
		printf("Couldnt read correct time\n");
		exit(1);
	}
	 */

	int fd, fd2, pedidosNo;

	char pid[20];
	sprintf(pid, "%u", (unsigned int) getpid());
	strcat(fich,pid);

	if((f = open_file_sauna()) == NULL)
		exit(1);

	if(create_fifo_entrada()==-1)
		exit(1);

	if((fd = open_fifo_entrada()) == -1)
		exit(1);

	if((pedidosNo = readPedidosNo(fd)) == -1)
		exit(1);

	if ((fd2= open_fifo_rejeitado()) == -1)
		exit(1);

	if(read_real_time_fifo(fd)==-1)
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

	unsigned int i;
	for(i = 0; i < tidIndex; i++){
		pthread_join(tid[i], NULL);
	}

	fprintf(f,"Recebidos: %04d(T) - %04d(M) - %04d(F)\nRejeitados: %04d(T) - %04d(M) - %04d(F)\nServidos: %04d(T) - %04d(M) - %04d(F)",stats[0]+stats[1],stats[0],stats[1],stats[2]+stats[3],stats[2],stats[3],stats[4]+stats[5],stats[4],stats[5]);
	fclose(f);

	printf("Sauna:\nRecebidos: %04d(T) - %04d(M) - %04d(F)\nRejeitados: %04d(T) - %04d(M) - %04d(F)\nServidos: %04d(T) - %04d(M) - %04d(F)\n\n",stats[0]+stats[1],stats[0],stats[1],stats[2]+stats[3],stats[2],stats[3],stats[4]+stats[5],stats[4],stats[5]);

	return 0;
}

void* open_file_sauna(){
	void* result;
	int ft;

	if((ft=open(fich, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1){
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

int read_real_time_fifo(int fd){
	if(read(fd,&begin,sizeof(struct timespec)) == -1){
		printf("Sauna: Coulnt read begin time\n");
		return -1;
	}
	else
		return 0;
}

void* saunar(void * pedido){
	struct Pedido p = *(struct Pedido *) pedido;

	usleep(p.time*1000);

	pthread_mutex_lock(&pedidos_lock);
	pedidosAtuais--;
	pthread_mutex_unlock(&pedidos_lock);

	printMessage(getpid(), pthread_self(),p,SERVIDO);

	return NULL;
}

void printMessage(pid_t pid, pthread_t tid, struct Pedido p, unsigned int tip){
	pthread_mutex_lock(&file_lock);

	struct timespec end;
	double t_dif;

	if(clock_gettime(CLOCK_REALTIME, &end))
		t_dif = -1;
	else {
		t_dif = (end.tv_sec-begin.tv_sec)*1e3 + (end.tv_nsec - begin.tv_nsec)/1e6;
	}

	fprintf(f,"%.2f - %u - %u - %04i: %c - %05d - %s\n", t_dif, (unsigned int) pid, (unsigned int) tid, p.id, p.g, p.time, messageTip[tip]);

	if (tip == 0) {
		if (p.g == 'M')
			stats[0]++;
		else if (p.g == 'F')
			stats[1]++;
	}
	else if (tip == 1) {
		if (p.g == 'M')
			stats[2]++;
		else if (p.g == 'F')
			stats[3]++;
	}
	else {
		if (p.g == 'M')
			stats[4]++;
		else if (p.g == 'F')
			stats[5]++;
	}

	pthread_mutex_unlock(&file_lock);
}

void rejectPedido(struct Pedido * p, int fd){
	if(write(fd, p, sizeof(struct Pedido))==-1)
		printf("Sauna: Couldnt reject pedido\n");
	if(p->rNo == 2)
		processados++;
	printMessage(getpid(), pthread_self(),*p,REJEITADO);
}
