#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "argaction.h"
#include "search.h"
#include "check.h"


int main(int argc, char *argv[]) {

printf("\n");
	int hasError = 0;

	if (argc!=5){
		printf("Usage: sfind [path] [-argument] [stuff] [-action]\n");
		return -1;
	}

 	else if(argc == 5) {
 		if (!checkArgument(argv[2])){ hasError = 1; }
 		if(!checkAction(argv[4])){ hasError = 1; }
 		if (hasError) { return -1; }
 	}
 	else{ printf ("Done\n"); }

 	char *path = argv[1];
 	char *arg = argv[2];
 	char *value = argv[3];
 	char *action = argv[4];


 	if(!strcmp(arg,ARGNAME)){
 		searchName( path, value, action);

 	}
 	else if (!strcmp(arg,ARGTYPE)){
 		searchType(path, value, action);
 	}
 	else 	if(!strcmp(arg,ARGPERM)) {
			searchPerm(path, value, action);
		}
 		else
 			return -1;

	return 0;
}
