
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
			if (!strcmp(arg, ARGNAME)){
				if (!strcmp(action,ACTIONPRINT)){ pathPrinting(workingDir, d_name);}
				if (!strcmp(action,ACTIONDELETE)){
					strcpy(toRemove[i], subDirName);
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
			else if (!strcmp(arg, ARGTYPE)) {
				printf("TYPE!\n");
			}			
		};

		if ( checkType(subDirName, 'd') == 1){
			if(fork()==0)
				listFilesFromDir(subDirName, ARGNAME, value, action);
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