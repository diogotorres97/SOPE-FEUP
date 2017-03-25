
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[]){
 DIR *dirp;
 struct dirent *direntp;
 struct stat stat_buf;
 char *str;

//Check Args
 if (argc >8 && *argv[1]=='~'){
  fprintf( stderr, "Usage: %s symbol -name file_name func_exec \n", argv[0]);
  exit(1);
}else  if(argc != 5 && *argv[1]== '.'){
 fprintf( stderr, "Usage: %s symbol -type type_symbol func_exec\n", argv[0]);
 exit(1);
}else  if(argc != 5 && *argv[1]== '/'){
 fprintf( stderr, "Usage: %s symbol -perm perm_number func_exec\n", argv[0]);
 exit(1);
}

exit(0);

}

int checkType(struct dirent *direntp, char type){
 struct stat stat_buf;

 if(lstat(direntp->d_name, &stat_buf) == -1){
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

