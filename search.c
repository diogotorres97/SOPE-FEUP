
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "search.h"
#include "argaction.h"
#include "check.h"

void sigint_handler2(int signo){
  pause();
}

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


void listFilesFromDir(char* workingDir, char* arg, char *value, char* action, char *exec[], int execSize) {
	DIR *dir;
	struct dirent *entry;
	char * d_name;
	char subDirName[500];
	int status = 0;
	int pid;

	if(!(dir = opendir(workingDir))) {
		fprintf(stderr, "Cannot open directory '%s': %s\n", workingDir, strerror(errno));
		exit(EXIT_FAILURE);
	}

	strcpy(subDirName,workingDir);

	while ((entry = readdir(dir))) {
		d_name = entry->d_name;
		//printf("Name: %s\n",d_name);

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
			if (!strcmp(action,ACTIONEXEC)){
					if((pid=fork()) == 0){
					char filePath[500];
					strcpy(filePath,workingDir);
					strcat(filePath, "/");
					strcat(filePath, d_name);
					exec[execSize] = filePath;
					exec[execSize+1] = NULL;
					execvp(exec[0],exec);
					printf("Exec Failed\n");
				} else if ((pid = fork()) < 0) {
					 fprintf(stderr,"fork error\n");
					 exit(1);
				}
			}
		}

		if ( checkType(subDirName, 'd') == 1){

			wait(NULL); //TODO:CHECK THIS WAIT

			if ((pid = fork()) < 0) {
				 fprintf(stderr,"fork error\n");
				 exit(1);
			}
				else 	if(pid==0){
				struct sigaction funct;
				// prepare the 'sigaction struct' for ignoring SIGINT
					funct.sa_handler = sigint_handler2;
					sigemptyset(&funct.sa_mask);
					funct.sa_flags = 0;
				// ignore SIGINT and get the original handler
					sigaction(SIGINT,&funct,NULL);

				listFilesFromDir(subDirName, arg, value, action, exec, execSize);
			}

		}

		strcpy(subDirName,workingDir);
	}

	closedir(dir);

	exit(0);
}


void searchName(char *path, char *value, char *action, char *exec[], int execSize) {

	listFilesFromDir(path, ARGNAME, value, action, exec, execSize);
}

void searchType(char *path, char *value, char *action, char *exec[], int execSize) {

	listFilesFromDir(path, ARGTYPE, value, action, exec, execSize);
}

void searchPerm(char *path, char *value, char *action, char *exec[], int execSize) {
	listFilesFromDir(path, ARGPERM, value, action, exec, execSize);
}
