//Codigo que pode ser util

if ((dirp = opendir( argv[1])) == NULL)
{
  perror(argv[1]);
  exit(2);
}
while ((direntp = readdir( dirp)) != NULL)
{
  if(lstat(direntp->d_name, &stat_buf) == -1){
    fprintf(stderr, "Could not read status from current directory");
    exit(-1);
  }
  if (S_ISREG(stat_buf.st_mode)) str = "regular";
  else if (S_ISDIR(stat_buf.st_mode)) str = "directory";
  else str = "other";
  printf("%-25s - %s\n", direntp->d_name, str);
}
closedir(dirp);
exit(0);


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_CMD_SIZE 256
#define MAX_CMD_ARGS 10

int main(int argc, char** argv){
  char* current_cmd = (char*)malloc(MAX_CMD_SIZE);
  char** args = (char**)malloc(MAX_CMD_ARGS);
  const char* delim = " ";
  char* token = (char*)malloc(256);

  int STDOUT_FILENO_CPY = dup(STDOUT_FILENO);
  int DST_FILENO;

  do{
    int index;
    for(index = 0; index < 10; index++)
      args[index] = NULL;

    index = 0;
    write(STDOUT_FILENO, "minish> ", 8);
    read(STDIN_FILENO, current_cmd, MAX_CMD_SIZE);
    /* get the first cmd arg (program name) */
    token = strtok(current_cmd, delim);
    /* walk through other (cmd args) */
    while(token != NULL){
      args[index++] = token;
      token = strtok(NULL, delim);
    }

    args[index-1][strlen(args[index-1])-1] = 0;

    //User wants to exit?
    if(strncmp(args[0], "quit", 4) == 0)
      exit(0);

    /* PRINT ARGS
    index = 0;
    while(args[index] != NULL){
      write(STDOUT_FILENO, args[index], strlen(args[index]));
      write(STDOUT_FILENO, "/next/", 6);
      index++;
    }*/

    //User wants to redirect output?
    if(index >= 3){
      if(strncmp(args[index-2],"-o",2) == 0){
        DST_FILENO = open(args[index-1], O_CREAT | O_EXCL | O_SYNC | O_WRONLY, 0777);
        if(DST_FILENO == -1)
          write(STDOUT_FILENO, "RIP ficheiro\n", 13);
        else
          dup2(DST_FILENO, STDOUT_FILENO);
        args[index-2] = NULL;
      }
    }

    //Execute program
    pid_t pid = fork();
    int trash;
    if(pid == -1)
      write(STDOUT_FILENO, "RIP current cmd\n", 16);
    else if(pid == 0)
      execvp(args[0],args);
    else
      wait(&trash);

    //Reset STDOUT_FILENO to its original state (even if it hasn't changed)
    dup2(STDOUT_FILENO_CPY, STDOUT_FILENO);
  }while(1);
}


#include <stdio.h>
#include <string.h>

void split_sub_command(char* command){
  char *token;
  token = strtok(command, " ");
  while(token != NULL){
   printf( "%s\n", token );
   token = strtok(NULL, " ");
 }
}

int main(int argc, char** argv){
 char str[256];
 printf("Insert the command to be split: "),
 fgets(str, 256, stdin);
     /* Determine which delimiter is being used */
 char* delimiter;
 char* search;
 search = strchr(str,'|');
 delimiter = (search == NULL ? ";" : "|");
     /* Split sub-commands */
 char* commands[10] = {0,0,0,0,0,0,0,0,0,0};
 char* temp;
 temp = strtok(str, delimiter);
 int i = 0;
 while(temp != NULL){
  commands[i++] = temp;
  temp = strtok(NULL, delimiter);
}

     /* Split each sub-command */
for(i = 0; i < 10; i++){
 if(commands[i] != NULL)
  split_sub_command(commands[i]);
}

return 0;
}

