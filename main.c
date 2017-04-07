#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "argaction.h"
#include "search.h"
#include "check.h"


//void listFilesFromDir(char *path, char *arg, char *value, char *action);
//void searchName (char *path, char* value, char* action);

int main(int argc, char *argv[]) {


	int hasError = 0;

	if (argc!=5){
		printf("Usage: sfind [path] [-argument] [stuff] [-action]\n");
		return 0;
	}

 	else if(argc == 5) {
 		if (!checkArgument(argv[2])){ hasError = 1; }
 		if(!checkAction(argv[4])){ hasError = 1; }
 		if (hasError) { return 0; }
 	}
 	else{ printf ("Done\n"); }
 	char *path = argv[1];
 	char *arg = argv[2];
 	char *value = argv[3];
 	char *action = argv[4];

 	if(!strcmp(arg,ARGNAME)){
 		searchName( path, value, action);
 		//listFilesFromDir(path, ARGNAME, value, action);
 	}
 	else if (!strcmp(arg,ARGTYPE)){
 		printf ("Tipo\n");
 	}
 	else { printf ("Perm\n");};

	return 0;
}