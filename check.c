#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "check.h"
#include "argaction.h"

int checkArgument(char *argument) {

	if ((strcmp(argument,ARGNAME) * strcmp(argument, ARGTYPE) * strcmp(argument, ARGPERM))!=0){
		printf("Invalid argument.\n");
		return 0;
	}
	else return 1;
}

int checkAction(char *action) {

	if ((strcmp(action,ACTIONPRINT) * strcmp(action, ACTIONDELETE) * strcmp(action, ACTIONEXEC))!=0) {
		printf("Invalid action.\n");
 
		return 0;
	}
	else return 1;
}

int checkType(char * d_name, char type){
	struct stat stat_buf;

	if(lstat(d_name, &stat_buf) == -1){
		fprintf(stderr, "Could not read status from current directory\n %s",d_name);
		exit(-1);
	}

	switch (type){
		case 'c': //character (unbuffered) special
		if (S_ISCHR(stat_buf.st_mode)) printf("\nc\n");
		break;
		case 'd': //directory
		if (S_ISDIR(stat_buf.st_mode)) printf("\nd\n");
		break;

		case 'p': // named pipe (FIFO)
		if (S_ISFIFO(stat_buf.st_mode)) printf("\np\n");
		break;

		case 'f': // regular file
		if (S_ISREG(stat_buf.st_mode)) printf("\nf\n");
		break;

		case 'l': //symbolic link;
		if (S_ISLNK(stat_buf.st_mode)) printf("\nlink\n");
		break;

		case 's': //socket
		if (S_ISSOCK(stat_buf.st_mode)) printf("\ns\n");
		break;
		case 'D': //door (Solaris)
		if (S_ISBLK(stat_buf.st_mode)) printf("\nD\n");
		break;

		default:
		break;
	}
	return 0;

}



int checkName(struct dirent *direntp, char name[]){


  if(strcmp(direntp->d_name,name)==0)
    return 1;
  else
    return 0;

}

int checkPerm(struct dirent *direntp, int perm){

 struct stat stat_buf;

 if(lstat(direntp->d_name, &stat_buf) == -1){
  fprintf(stderr, "Could not read status from current directory");
  exit(-1);
}

unsigned long mask = S_IRWXU|S_IRWXG|S_IRWXO;
mask &=  stat_buf.st_mode;
if(mask == perm)
  return 1;
else
  return 0;


}
