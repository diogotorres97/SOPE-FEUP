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

char entrada[] = "entrada";
char rejeitados[] = "rejeitados";
char fich[30] = "ger.txt";
int pedidosNo;
int tempoMax;
clock_t begin;
pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;
FILE * f;
int stats[6];   //CONTADORES PARA ESTATISTICA

void * gerar(void * arg){
   time_t t;
   clock_t end;
   srand((unsigned) time(&t));
   struct Pedido pedidos[pedidosNo];
   int i, g, dur, fd, timeout = 5;
   double t_dif; 

   //OPEN FILE
   /*
   char pid[20];
   sprintf(pid, "%u", (unsigned int) getpid());
   strcat(fich,pid);
   strcat(fich,".txt");
   printf("%s\n", fich);
   */
   /*
   int ft = open(fich,O_CREAT | O_WRONLY | O_APPEND | O_SYNC , 0600);
   if(ft == -1){
      printf("Gerador: Couldnt open file d\n");
      exit(1);
   }
   FILE * f = fdopen(ft, "w");
   if(f == NULL){
      printf("Gerador: Couldnt open file\n");
      exit(1);
   }
   */
      

   //OPEN FIFO
   for(i = 0; i < 5; i++){
     fd = open(entrada, O_WRONLY | O_NONBLOCK);
     if(fd == -1)
        usleep(500);
     else
        break;
   }
   if(i == timeout){
      printf("Gerador: Couldnt open fifo\n");
      exit(1);
   }

   int pNo = pedidosNo;
   write(fd,&pNo,sizeof(int));
     
   for(i = 0; i < pedidosNo; i++){
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
        write(fd,&pedidos[i],sizeof(struct Pedido));
        pthread_mutex_lock(&file_lock);
        end = clock();
        t_dif = (double) (end-begin)/CLOCKS_PER_SEC;
        fprintf(f,"%f - %u - %i: %c - %d - %s\n", t_dif, (unsigned int) getpid(), i, pedidos[i].g, dur, "PEDIDO");     
        pthread_mutex_unlock(&file_lock);
   }
   close(fd);
   return NULL;
}


void *recolocar(void * arg){
   clock_t end;
   int fd, fd2, i, timeout = 5;
   double t_dif;


   //OPEN FIFO ENTRADA
   for(i = 0; i < 5; i++){
     fd2 = open(entrada, O_WRONLY);
     if(fd2 == -1)
        usleep(500);
     else
        break;
   }
   if(i == timeout){
      printf("Gerador: Couldnt open fifo entrada\n");
      exit(1);
   }

   //CREATE FIFO REJEITADOS
   if(mkfifo(rejeitados,0660)){
     printf("Gerador: Couldnt create fifo\n");
   }

   //OPEN FIFO REJEITADOS
   fd = open(rejeitados,O_RDONLY);
   if(fd == -1){
      printf("Sauna: Couldnt open fifo rejeitados\n");
      exit(1);
   }

   /*
   int ft = open(fich,O_CREAT | O_WRONLY | O_SYNC , 0600);
   if(ft == -1){
      printf("Gerador: Couldnt open file d\n");
      exit(1);
   }
 
   FILE * f = fdopen(ft, "w");
   if(f == NULL){
      printf("Gerador: Couldnt open file\n");
      exit(1);
   }
   */

   struct Pedido p;
   int br;
   while((br = read(fd,&p,sizeof(struct Pedido)))){
      if(br == -1){
          printf("Gerador: erro ao ler\n");
          close(fd);
          close(fd2);
          exit(1);
      }
      //printf("Pedido - id: %d, gender; %c, time: %d\n",p.id,p.g,p.time);
      p.rNo++;
      
      if(p.rNo != 3)
          write(fd2,&p,sizeof(struct Pedido));
      else{
          if(p.g == 'M')    //AUMENTA DESCARTADOS M
             stats[4]++;
          else
             stats[5]++;     //AUMENTA DESCARTADOS F
          pthread_mutex_lock(&file_lock);
          end = clock();
          t_dif = (double) (end-begin)/CLOCKS_PER_SEC;
          fprintf(f,"%f - %u - %i: %c - %d - %s\n", t_dif, (unsigned int) getpid(), p.id, p.g, p.time, "DESCARTADO");
          pthread_mutex_unlock(&file_lock);
          continue;
      }
      
      if(p.g == 'M')
          stats[2]++;     //AUMENTA REJEITADOS M
      else
          stats[3]++;     //AUMENTA REJEITADOS F
      pthread_mutex_lock(&file_lock);
      end = clock();
      t_dif = (double) (end-begin)/CLOCKS_PER_SEC;
      fprintf(f,"%f - %u - %i: %c - %d - %s\n", t_dif, (unsigned int) getpid(), p.id, p.g, p.time, "REJEITADO");
      pthread_mutex_unlock(&file_lock);
   }
   printf("Gerador: recolocar terminou\n");
   close(fd2);
   close(fd);
   return NULL;
}


int main(int argc, char*argv[]){
   if(argc != 3)
      return 1;
   begin = clock();                //COMECAR A CONTAR O TEMPO
   pedidosNo = atoi(argv[1]);
   tempoMax = atoi(argv[2]);

   int ft = open(fich, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0600);
     if(ft == -1){
      printf("Gerador: Couldnt open file d\n");
      exit(1);
   }
 
   f = fdopen(ft, "w");
   if(f == NULL){
      printf("Gerador: Couldnt open file\n");
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
