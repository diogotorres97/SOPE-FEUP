#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "pedido.h"

#define PEDIDO 0
#define REJEITADO 1
#define DESCARTADO 2
#define TIMEOUT 5

const char entrada[] = "entrada";
const char rejeitados[] = "rejeitados";
const char * messageTip[] = {"PEDIDO", "REJEITADO", "DESCARTADO"};
char fich[30] = "ger.";
int pedidosNo;
int tempoMax;
int stats[6];   //CONTADORES PARA ESTATISTICA

clock_t begin;
pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;

FILE * f;

//TODO:FIX THIS LINES ABOVE e alterar path


int open_fifo_entrada(){
	int fd,i;
	for(i = 0; i < TIMEOUT; i++){
		//fd = open("/tmp/entrada", O_WRONLY);
		fd = open(entrada, O_WRONLY);
		if(fd == -1)
			usleep(500);
		else
			break;
	}

	if(i == TIMEOUT){
		printf("Gerador: Couldnt open fifo\n");
		return -1;
	}
	return fd;

}

int create_fifo_rejeitado(){
	//CREATE FIFO REJEITADOS
	if(mkfifo(rejeitados,0660)){
		printf("Gerador: Couldnt create fifo \n");
	}
	return 0;
}

int open_fifo_rejeitado(){
	int fd;
	//OPEN FIFO REJEITADOS
	fd = open(rejeitados,O_RDONLY);
	if(fd == -1){
		printf("Sauna: Couldnt open fifo rejeitados\n");
		return -1;
	}
	return fd;
}

void* open_file_gerador(){
	void* result;
	int ft;
	ft=open(fich, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0600);
	if(ft == -1){
		printf("Gerador: Couldnt open file d\n");
		return NULL;
	}

	result = fdopen(ft, "w");
	if(result == NULL){
		printf("Gerador: Couldnt open file\n");
		return NULL;
	}
	return result;

}


void gerarPedido(struct Pedido pedidos[], int i){
	time_t t;
	srand((unsigned) time(&t));
	int g,dur;

	pedidos[i].id = i;
	g = rand()%2;
	if(g){
		pedidos[i].g = 'M';
		stats[0]++;           //AUMENTA GERADOS M
	}
	else{
		pedidos[i].g = 'F';
		stats[1]++;           //AUMENTA GERADOS F;
	}
	dur = (rand()%tempoMax)+1;
	pedidos[i].time = dur;
	pedidos[i].rNo = 0;

}

void printMessage(pid_t pid, struct Pedido p, unsigned int tip){
	clock_t end;
	double t_dif;
	end = clock();
	t_dif = (double) (end-begin)/CLOCKS_PER_SEC;
	fprintf(f,"%f - %u - %i: %c - %d - %s\n", t_dif, (unsigned int) pid, p.id, p.g, p.time, messageTip[tip]);
}


void * gerar(void * arg){
	struct Pedido pedidos[pedidosNo];
	int i, g, dur, fd;
	double t_dif;

	fd=open_fifo_entrada();
	if(fd==-1){
		exit(1);
	}

	int pNo = pedidosNo;
	write(fd,&pNo,sizeof(int));

	for(i = 0; i < pedidosNo; i++){

		gerarPedido(pedidos, i);

		pthread_mutex_lock(&file_lock);
		printMessage(getpid(),pedidos[i],PEDIDO);
		pthread_mutex_unlock(&file_lock);

		write(fd,&pedidos[i],sizeof(struct Pedido));
	}

	close(fd);
	return NULL;
}


void *recolocar(void * arg){
	int fd, fd2;

	fd2=open_fifo_entrada();
	if(fd2==-1){
		exit(1);
	}

	create_fifo_rejeitado();


	fd=open_fifo_rejeitado();
	if(fd==-1){
		exit(1);
	}

	struct Pedido p;
	int br;

	while((br = read(fd,&p,sizeof(struct Pedido)))){
		if(br == -1){
			printf("Gerador: erro ao ler\n");
			close(fd);
			close(fd2);
			exit(1);
		}

		p.rNo++;

		if(p.rNo != 3){
			write(fd2,&p,sizeof(struct Pedido));

			if(p.g == 'M')
				stats[2]++;     //AUMENTA REJEITADOS M
			else
				stats[3]++;     //AUMENTA REJEITADOS F

			pthread_mutex_lock(&file_lock);
			printMessage(getpid(), p,REJEITADO);
			pthread_mutex_unlock(&file_lock);
		}
		else{
			if(p.g == 'M')    //AUMENTA DESCARTADOS M
				stats[4]++;
			else
				stats[5]++;     //AUMENTA DESCARTADOS F

			pthread_mutex_lock(&file_lock);
			printMessage(getpid(),p,DESCARTADO);
			pthread_mutex_unlock(&file_lock);
		}
	}

	printf("Gerador: recolocar terminou\n");
	close(fd2);
	close(fd);
	return NULL;
}


int main(int argc, char*argv[]){
	if(argc != 3)
		return -1;

	begin = clock();                //COMECAR A CONTAR O TEMPO
	pedidosNo = atoi(argv[1]);
	tempoMax = atoi(argv[2]);

	char pid[20];
	sprintf(pid, "%u", (unsigned int) getpid());
	strcat(fich,pid);

	f = open_file_gerador();
	if(f == NULL){
		exit(1);
	}

	pthread_t tid[2];
	pthread_create(&tid[0],NULL,gerar,NULL);
	pthread_create(&tid[1],NULL,recolocar,NULL);
	pthread_join(tid[0],NULL);
	pthread_join(tid[1],NULL);
	fprintf(f,"Gerados: %d(T) - %d(M) - %d(F)\nRejeitados: %d(T) - %d(M) - %d(F)\nDescartados: %d(T) - %d(M) - %d(F)",stats[0]+stats[1],stats[0],stats[1],stats[2]+stats[3],stats[2],stats[3],stats[4]+stats[5],stats[4],stats[5]);
	fclose(f);
	return 0;
}
