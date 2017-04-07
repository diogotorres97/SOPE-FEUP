#ifndef SEARCH_H
#define SEARCH_H

int verifyPath(char *path);

void pathPrinting(char *workingDir, char *d_name);

void subDirName(char *subDirName, char *d_name);

void searchName(char *path, char *value, char *action);

void listFilesFromDir(char *path, char *arg, char *value, char *action);

#endif