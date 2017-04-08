#ifndef CHECK_H_
#define CHECK_H_

int checkArgument(char *argument);

int checkAction(char *action);

int checkName(struct dirent *direntp, char *name);

int checkType(char *d_name, char type);

int checkPerm(char *perm);

#endif