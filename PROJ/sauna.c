  #include <unistd.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <sys/file.h>
  #include <pthread.h>
  #include <string.h>
  #include <stdbool.h>
  #include "pedido.h"

  #define RECEBIDO 0
  #define REJEITADO 1
  #define SERVIDO 2
  #define TIMEOUT 5

  const char entrada[] = "entrada";
  const char rejeitados[] = "rejeitados";
  const char * messageTip[] = {"RECEBIDO", "REJEITADO", "SERVIDO"};
  char fich[30] = "bal.";
  int pedidosAtuais = 0;
  int processados = 0;
  struct Pedido * pedidoEspera = NULL;
  FILE * f;

  clock_t begin;

  /*
  //variaveis que nao precisam de ser globais
  pthread_t tid[pedidosNo];
  unsigned int tidIndex = 0;
  struct Pedido pedidosAtivos[pedidosNo];
  */

  pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;
  //pthread_mutex_t processados_lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t pedidos_lock = PTHREAD_MUTEX_INITIALIZER;


  void* saunar(void * pedido){
     struct Pedido p = *(struct Pedido *) pedido;
     //printf("Pedido - id: %d, gender; %c, time: %d\n",p.id,p.g,p.time);

     /*
     pthread_mutex_lock(&processados_lock);
     printf("Sauna: Thread processados++\n");
     processados++;
     pthread_mutex_unlock(&processados_lock);
     */
     usleep(p.time);

     pthread_mutex_lock(&pedidos_lock);
     pedidosAtuais--;
     pthread_mutex_unlock(&pedidos_lock);

     printf("Thread terminou\n");

     return NULL;
  }


  /*
  void processPedido(struct Pedido p){
    pedidosAtivos[p.id] = p;
    tid[tidIndex] = tidIndex;
    pthread_mutex_lock(&pedidos_lock);
    pedidosAtuais++;
    pthread_mutex_unlock(&pedidos_lock);
    processados++;
    pthread_create(&tid[tidIndex], NULL, saunar, &pedidosAtivos[p.id]);
    tidIndex++;
  }
  */

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

     int fd, fd2, i;

     char pid[20];
     sprintf(pid, "%u", (unsigned int) getpid());
     strcat(fich,pid);

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

     //MAKE FIFO
     if(mkfifo(entrada,0660)){
        printf("Sauna: Couldnt create fifo\n");
        //exit(1);
     }

     fd = open(entrada,O_RDONLY);
     if(fd == -1){
        printf("Sauna: Couldnt open fifo\n");
        exit(1);
     }

     int pedidosNo;
     if(read(fd,&pedidosNo,sizeof(int)) == -1){
        printf("Sauna: Falhou ler pedidos\n");
        exit(1);
     }

     printf("Sauna: NoPedidos %d\n",pedidosNo);


     //OPEN FIFO
     for(i = 0; i < TIMEOUT; i++){
       fd2 = open(rejeitados, O_WRONLY);
       printf("Ciclo para abrir rejeitados\n");

       if(fd2 == -1)
          sleep(1);
       else
          break;
     }

     if(i == TIMEOUT){
        printf("Sauna: Couldnt open fifo\n");
        exit(1);
     }

     //printf("Sauna: Chegou aqui\n");
     char gender;
     int pedidosMax = atoi(argv[1]);
     pthread_t tid[pedidosNo];
     unsigned int tidIndex = 0;
     struct Pedido pedidosAtivos[pedidosNo];
     struct Pedido p;
     int br;

     while(1){
      if(pedidosAtuais == pedidosMax){
          if(pedidoEspera == NULL){
            br = read(fd,&p,sizeof(struct Pedido));
            //printf("Sauna: br - %d\n", br);
            printMessage(getpid(), pthread_self(),p,RECEBIDO);
            if(gender == p.g){
                printf("Sauna: pedido em espera\n");
                pedidoEspera = (struct Pedido *) malloc(sizeof(struct Pedido));
                *pedidoEspera = p;
                printf("Pedido - id: %d, gender; %c, time: %d\n",pedidoEspera->id,pedidoEspera->g,pedidoEspera->time);
            }
            else {
               rejectPedido(&p, fd2);
           }
         }
      }
      else if(pedidosAtuais == pedidosMax-1){
         if(pedidoEspera != NULL){
            pedidosAtivos[pedidoEspera->id] = *pedidoEspera;
            tid[tidIndex] = tidIndex;
            pthread_mutex_lock(&pedidos_lock);
            pedidosAtuais++;
            pthread_mutex_unlock(&pedidos_lock);
            processados++;
            pthread_create(&tid[tidIndex], NULL, saunar, &pedidosAtivos[pedidoEspera->id]);
            printMessage(getpid(), tid[tidIndex],p,SERVIDO);
            tidIndex++;
            free(pedidoEspera);
            pedidoEspera = NULL;
        }
        else{
  	        br = read(fd,&p,sizeof(struct Pedido));
            //printf("Sauna: br - %d\n", br);
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
      else if(pedidosAtuais == 0){
  	        br = read(fd,&p,sizeof(struct Pedido));
            //printf("Sauna: br - %d\n", br);
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
      else{
          //printf("Sauna: Before read\n");
          br = read(fd,&p,sizeof(struct Pedido));
          //printf("Sauna: After read\n");
          //printf("Sauna: br - %d\n", br);
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

      printf("Sauna: pedidosAtuais: %d\n", pedidosAtuais);
      printf("Sauna: processados: %d\n", processados);
      if(processados == pedidosNo){
          printf("Sauna: Final processados: %d\n", processados);
          close(fd2);
          break;
      }
     }
     close(fd);
     printf("Sauna: processados: %d\n", processados);
     pthread_exit(NULL);
     //return 0;
  }
