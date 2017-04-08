
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


void listFilesFromDir(char* workingDir, char* arg, char *value, char* action) {
	DIR *dir;
	struct dirent *entry;
	char * d_name;
	char subDirName[500];
	int status = 0;

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



		int test=0;

		if (!strcmp(arg, ARGNAME)){
			test=checkName(d_name, value);
		}
		else if(!strcmp(arg, ARGPERM)){
			test=checkPerm(subDirName, value);
		} else if(!strcmp(arg, ARGTYPE)){
			test=checkType(subDirName, *value);
		} else {return;}

		if(test!=0){
			if(!strcmp(action,ACTIONPRINT)) {pathPrinting(workingDir, d_name);}
			if(!strcmp(action,ACTIONDELETE)) {
				status = remove(subDirName);
				if (status == 0)
				printf("File %s deleted.\n", subDirName);
				else {
					printf("Unable to delete.\n");
					perror ("Error");
				}

				strcpy(subDirName,workingDir);
			}
			if (!strcmp(action,ACTIONEXEC)){ printf("EXEC\n");}
		}

		if ( checkType(subDirName, 'd') == 1){
			if(fork()==0){
				listFilesFromDir(subDirName, arg, value, action);
			}
		}

		strcpy(subDirName,workingDir);
	}

	closedir(dir);

	exit(0);
}


void searchName(char *path, char *value, char *action) {

	listFilesFromDir(path, ARGNAME, value, action);
}

void searchType(char *path, char *value, char *action) {

	listFilesFromDir(path, ARGTYPE, value, action);
}

void searchPerm(char *path, char *value, char *action) {
	listFilesFromDir(path, ARGPERM, value, action);
}
