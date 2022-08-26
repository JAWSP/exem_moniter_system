#ifndef UTILS_H
# define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pwd.h>

//util.c
void err_by(char *reason);
FILE *open_fs(FILE *fs, char *root);
DIR *open_dir(DIR *dir, char *root);


//index_utils.c
int indx_space(char *line, int i);
int indx_go_next(char *line, int i);
int indx_get_num(char *line, int i); //sscanf를 이용함에따라 사라질 예정?
#endif