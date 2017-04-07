
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "search.h"
#include "argaction.h"
#include "check.h"

static char toRemove[10][500];
static int i = 0;

int verifyPath(char *path) {

	DIR *dir;

	if(!(dir = opendir(path))) {
			fprintf(stderr, "Cannot open directory '%s': %s\n", path, strerror(errno));
			exit(EXIT_FAILURE);
		}

		return 0;

}


void pathPrinting(char *workingDir, char *d_name) {
	printf("%s/%s\n", workingDir, d_name);

}

void subDirName1(char* subDirName, char* d_name) {
	strcat(subDirName,"/");
	strcat(subDirName,d_name);
	strcat(subDirName,"\0");
	printf("CUCU:   \t %s\n", subDirName);
	//return subDirName;
}

void listFilesFromDir(char* workingDir, char* arg, char *value, char* action) {
	DIR *dir;
	struct dirent *entry;
	char * d_name;
	char subDirName[500];
	int status = 0;

	//char subDirName[]="";

	if(!(dir = opendir(workingDir))) {
		fprintf(stderr, "Cannot open directory '%s': %s\n", workingDir, strerror(errno));
		exit(EXIT_FAILURE);
	}

	strcpy(subDirName,workingDir);

	while ((entry = readdir(dir))) {
		d_name = entry->d_name;

		if (!strcmp(d_name, ".") || !strcmp(d_name, ".."))
		continue;

		strcat(subDirName,"/");
		strcat(subDirName,d_name);
		if (!strcmp(d_name, value)){
			if (!strcmp(action,ACTIONPRINT)){ pathPrinting(workingDir, d_name);}
			if (!strcmp(action,ACTIONDELETE)){
				//strcpy(toRemove[i], subDirName);
				remove(subDirName);

			strcpy(subDirName,workingDir);
				printf("Gravou %s para apagar %d\n", toRemove[i],i);
				i++;
			}
			if (!strcmp(action,ACTIONEXEC)){ printf("EXEC\n");}
			

			//exit(0);
			
		}
			//subDirName1(subDirName, d_name);

		if ( checkType(subDirName, 'd') == 1){
			if(fork()==0)
				listFilesFromDir(subDirName, ARGNAME, value, action);
		}
		strcpy(subDirName,workingDir);
	}
	closedir(dir);

	/*
	printf("i = %d\n",i);
	do {
printf("HDA %s", toRemove[i]);
		status = remove(toRemove[i]);
		
		if (status == 0)
		printf("DELETED %s\n", toRemove[i] );
		else {
			printf ("Unable to delete\n");
			perror("Error");
		}
	 	i--;
	 }while(i>0);
	 */
	exit(0);
//return;
}

void searchName(char *path, char *value, char *action) {

	listFilesFromDir(path, ARGNAME, value, action);
}

