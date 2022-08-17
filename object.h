#ifndef OBJECT_H
# define OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024
/*
 * 각각의 수집한 정보를 구조체 형식으로 저장
 */

//OS사용 부분을 합친부분을 쓸지는 일단 보류
typedef struct s_osUsage
{
	struct s_cpuUsage *cpu;
	struct s_memUsage *mem;
	struct s_packUsage *pack;
}osUsage;

typedef struct s_cpuUsage
{
	int usr;
	int sys;
	int iowait;
	int idle;
} cpuUsage;

typedef struct s_memUsage
{
	int free;
	int total;
	int used;
	int swap_total;
	int swap_used;
} memUsage;

typedef struct s_packUsage
{
	int in_bytes;
	int in_pakcets;
	int out_bytes;
	int out_packets;
	struct pack_Usage *next;
} packUsage;

typedef struct s_process
{
	char *name;
	int pid;
	int ppid;
	int cpu_time;
	char *user_name;
	char *cmd_line;
	struct pack_Usage *next;
} process;

/*
 * include func
 */

//util.c
void err_by(char *reason);
FILE *read_cmd(FILE *fp, char *cmd);

//index_utils.c
int indx_spaces(char *line, int i);
int indx_get_num(char *line, int i);

# endif
