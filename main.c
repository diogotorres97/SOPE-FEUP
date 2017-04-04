#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "check.c"
#include "search.c"
#include "exec.c"

#define ARGNAME "-name"
#define ARGTYPE "-type"
#define ARGPERM "-perm"
#define ACTIONPRINT "-print"
#define ACTIONDELETE "-delete"
#define ACTIONEXEC "-exec"

int checkArgument (char *argument);
int checkAction (char *action);

int main(int argc, char *argv[]) {

	if (argc!=5)
		printf("Usage: sfind [path] [-argument] [file] [-action]\n");
 	else if(argc == 5) {
 		if (!(checkArgument(argv[2]) && checkAction(argv[4]))){}
 	else
 			printf ("Done\n");
 	}

	return 0;
}

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
