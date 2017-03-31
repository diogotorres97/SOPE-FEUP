
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "check.c"
#include "search.c"
#include "exec.c"

int main(int argc, char *argv[]){
 if (!verifyPath(argv[1]))
  printf ("Hey\n");

if(!verifyCheck(argv[2]) ){
  printf("Hey2\n");
}
  return 0;
}
