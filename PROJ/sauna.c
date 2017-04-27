#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/file.h> 
#include <pthread.h>
#include "pedido.h"

char entrada[] = "entrada";
char rejeitados[] = "rejeitados";
int pedidosNo;
int processados;

int main(int argc, char*argv[]){
   if(argc != 1)
      return 1;
   int fd, fd2, i, timeout = 5;

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
   
   if(read(fd,&pedidosNo,sizeof(int)) == -1){
      printf("Sauna: Falhou ler pedidos\n");
      exit(1);
   }

   printf("Sauna: NoPedidos %d\n",pedidosNo);	
   processados = 0;


   //OPEN FIFO
   for(i = 0; i < 5; i++){
     fd2 = open(rejeitados, O_WRONLY);
     printf("Ciclo para abrir rejeitados\n");

     if(fd2 == -1)
        sleep(1);
     else
        break;
   }

   if(i == timeout){
      printf("Sauna: Couldnt open fifo\n");
      exit(1);
   }
   
  
   struct Pedido p;
   while(read(fd,&p,sizeof(struct Pedido))){
      printf("Pedido - id: %d, gender; %c, time: %d\n",p.id,p.g,p.time); 
      write(fd2, &p, sizeof(struct Pedido));
      if(p.rNo+1==3)
          processados++;
      if(processados == pedidosNo){
          close(fd2);
          break;
      }
   }
   close(fd);
   printf("Sauna: processados: %d\n", processados);
   return 0;
}
