
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void listDir(const char *name);

int main(int argc, char *argv[]) {

	listDir(argv[1]);
	return 0;
}

void listDir(const char* dir_name) {

	DIR *dir, *sub_dir;
	struct dirent *entry;
	const char* d_name;


	if(!(dir = opendir(dir_name))){
		fprintf(stderr, "Cannot open directory '%s': %s\n", dir_name, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1) {

		if (!(entry = readdir(dir)))
			//printf("PARTIREI");
			break;

		d_name = entry->d_name;


		if (!strcmp(d_name, ".") || !strcmp(d_name, ".."))
			continue;
		else {

	 		printf("%s/%s\n", dir_name, d_name);
			if ((sub_dir = opendir(d_name))){
				strcpy(sub_dir, dir_name);
				strcat(sub_dir, "/");
				strcat(sub_dir, d_name);
				listDir(sub_dir);
				printf("SUBDIR\n");
			}
		}

	}
}