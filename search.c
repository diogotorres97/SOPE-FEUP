
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void listFilesFromDir(char *name);

int verifyPath(char *path) {

	DIR *dir;

	if(!(dir = opendir(path))) {
			fprintf(stderr, "Cannot open directory '%s': %s\n", path, strerror(errno));
			exit(EXIT_FAILURE);
		}

		return 0;

}


void listFilesFromDir(char* workingDir) {
	DIR *dir;
	struct dirent *entry;
	char * d_name;
	char subDirName[500];
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

		printf("%s/%s\n", workingDir, d_name);
		strcat(subDirName,"/");
		strcat(subDirName,d_name);
		strcat(subDirName,"\0");

		//if ( checkType(subDirName, 'd') == 1){
			if(fork()==0)
				listFilesFromDir(subDirName);
		//}
		strcpy(subDirName,workingDir);
	}
	closedir(dir);

exit(0);
//return;
}


