
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void listFilesFromDir(char *name);
void listFilesFromSubDir(char *name);

int main(int argc, char *argv[]) {

	listFilesFromDir(argv[1]);
	return 0;
}


void listFilesFromDir(char* workingDir) {

	DIR *dir, *sDir;
	struct dirent *entry;
	char * d_name;
	//char * subDirName;
	char subDirName[] = "";

	if(!(dir = opendir(workingDir))) {
		fprintf(stderr, "Cannot open directory '%s': %s\n", workingDir, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	while (1) {

		if(!(entry = readdir(dir)))
			break;

		d_name = entry->d_name;

		if (!strcmp(d_name, ".") || !strcmp(d_name, ".."))
			continue;
		else {
			printf("%s/%s\n", workingDir, d_name);
			if ((sDir = opendir(d_name))){
				strcpy(subDirName,workingDir);
				strcat(subDirName,"/");
				strcat(subDirName,d_name);
				//printf("ZZZZZ\n%s\n\n",subDirName);
				listFilesFromSubDir(subDirName);
			}
		}
	}
}

void listFilesFromSubDir(char* workingDir) {

	printf("\n\n\n\n\nWORKINGDIR: %s", workingDir);
	DIR *dir, *sDir;
	struct dirent *entry;
	char * d_name;
	char subDirName[] = "";

	if(!(dir = opendir(workingDir))) {
		fprintf(stderr, "Cannot open directory '%s': %s\n", workingDir, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	while (1) {

		if(!(entry = readdir(dir)))
			break;

		d_name = entry->d_name;

		if (!strcmp(d_name, ".") || !strcmp(d_name, ".."))
			continue;
		else {
			printf("%s/%s\n", workingDir, d_name);
			if ((sDir = opendir(d_name))){
				strcpy(subDirName,workingDir);
				strcat(subDirName,"/");
				strcat(subDirName,d_name);
				listFilesFromSubDir(subDirName);
			}
		}
	}
}