#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "check.c"
#include "search.c"
#include "exec.c"

int checkArgument (char *argument);
int checkAction (char *action);

int main(int argc, char *argv[]) {

	if (argc!=5)
		printf("Usage: sfind [path] [-argument] [file] [-action]\n");
 	else if(argc == 5) {
 		if (!(checkArgument(argv[2]) && checkAction(argv[4]))){}
 	else
 			printf ("Done\n");
 	}

	return 0;
}