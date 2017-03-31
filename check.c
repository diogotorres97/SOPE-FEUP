
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int verifyCheck(char * flag){
	if( !(!strcmp(flag,"-name") || !strcmp(flag,"-type") || !strcmp(flag,"-perm"))){
		fprintf(stderr, "Flag '%s' is  invalid: %s\n",flag,  strerror(EINVAL));
		exit(EXIT_FAILURE);
	}
	return 0;
}

int checkType(char * d_name, char type){
	struct stat stat_buf;

	if(lstat(d_name, &stat_buf) == -1){
		fprintf(stderr, "Could not read status from current directory\n %s",d_name);
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



int checkName(struct dirent *direntp, char name[]){


  if(strcmp(direntp->d_name,name)==0)
    return 1;
  else
    return 0;

}

int checkPerm(struct dirent *direntp, int perm){

 struct stat stat_buf;

 if(lstat(direntp->d_name, &stat_buf) == -1){
  fprintf(stderr, "Could not read status from current directory");
  exit(-1);
}

unsigned long mask = S_IRWXU|S_IRWXG|S_IRWXO;
mask &=  stat_buf.st_mode;
if(mask == perm)
  return 1;
else
  return 0;


}
