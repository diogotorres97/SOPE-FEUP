
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


int checkType(char * d_name, char type){
	struct stat stat_buf;

	if(lstat(d_name, &stat_buf) == -1){
		fprintf(stderr, "Could not read status from current directory");
		exit(-1);
	}

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


void listFilesFromDir(char* workingDir) {

	DIR *dir;
	//, *sDir;
	struct dirent *entry;
	char * d_name;
	//char * subDirName;
	char subDirName[] = "";

	if(!(dir = opendir(workingDir))) {
		fprintf(stderr, "Cannot open directory '%s': %s\n", workingDir, strerror(errno));
		exit(EXIT_FAILURE);
	}

	strcpy(subDirName,workingDir);

	while (1) {


		if(!(entry = readdir(dir)))
		break;

		d_name = entry->d_name;

		if (!strcmp(d_name, ".") || !strcmp(d_name, ".."))
		continue;
		else {
			printf("%s/%s\n", workingDir, d_name);
			strcat(subDirName,"/");
			strcat(subDirName,d_name);
			strcat(subDirName,"\0");

			if ( checkType(subDirName, 'd') == 1){
				listFilesFromDir(subDirName);

			}

		}
	}
	closedir(dir);

	exit(0);
}

void listFilesFromSubDir(char* workingDir) {

	printf("\n\nWORKINGDIR: %s \n", workingDir);
	DIR *dir;
	//, *sDir;
	struct dirent *entry;
	char * d_name;
	char subDirName[] = "";

	if(!(dir = opendir(workingDir))) {
		fprintf(stderr, "Cannot open directory '%s': %s\n", workingDir, strerror(errno));
		exit(EXIT_FAILURE);
	}

	strcpy(subDirName,workingDir);

	while (1) {

		if(!(entry = readdir(dir)))
		break;

		d_name = entry->d_name;

		if (!strcmp(d_name, ".") || !strcmp(d_name, ".."))
		continue;
		else {
			//printf("\nEntrei aqui2\n");
			printf("%s/%s\n", workingDir, d_name);
			strcat(subDirName,"/");
			strcat(subDirName,d_name);
			strcat(subDirName,"\0");
			if ( checkType(subDirName, 'd') ==1 ){
				printf ("\n\n\n");
				listFilesFromSubDir(subDirName);
			}
		}
	}
	closedir(dir);
	exit(0);
}
