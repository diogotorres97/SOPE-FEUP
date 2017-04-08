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

	if(lstat(d_name, &stat_buf) == -1){return 1;}


	switch (type){
		case 'c': //character (unbuffered) special
		if (S_ISCHR(stat_buf.st_mode)) return 1;
		break;
		case 'd': //directory
		if (S_ISDIR(stat_buf.st_mode)) return 1;
		break;

		case 'p': // named pipe (FIFO)
		if (S_ISFIFO(stat_buf.st_mode)) return 1;
		break;

		case 'f': // regular file
		if (S_ISREG(stat_buf.st_mode)) return 1;
		break;

		case 'l': //symbolic link;
		if (S_ISLNK(stat_buf.st_mode)) return 1;
		break;

		case 's': //socket
		if (S_ISSOCK(stat_buf.st_mode)) return 1;
		break;
		case 'D': //door (Solaris)
		if (S_ISBLK(stat_buf.st_mode)) return 1;
		break;

		default:
		break;
	}
	return 0;

}
int checkName(char *d_name, char* name){


	if(strcmp(d_name,name)==0)
	return 1;
	else
	return 0;

}

int checkPerm(char * d_name, char* perm){

	struct stat stat_buf;

	if(lstat(d_name, &stat_buf) == -1){
		fprintf(stderr, "Could not read status from current directory");
		exit(-1);
	}


	int mask = S_IRWXU|S_IRWXG|S_IRWXO;
	mask &=  stat_buf.st_mode;

	char octal[4];
	sprintf(octal,"%#o", mask);

	if(strcmp(perm, octal)==0)
	return 1;
	else
	return 0;
}
