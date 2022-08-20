#ifndef OBJECT_H
# define OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BUFF_SIZE 1024


/*
 * structs
 * 각각의 수집한 정보를 구조체 형식으로 저장
 */

//OS사용 부분을 합친부분을 쓸지는 일단 보류
typedef struct s_data
{
	struct s_cpuUsage *cpu;
	struct s_memUsage *mem;
	struct s_packUsage *pack;
	struct s_process *proc;
} data;

typedef struct s_cpuUsage
{
	int usr;
	int sys;
	int iowait;
	int idle;
	FILE *cf;
} cpuUsage;

typedef struct s_memUsage
{
	int free;
	int total;
	int used;
	int swap_total;
	int swap_used;
	FILE *mf;
} memUsage;

typedef struct s_packUsage
{
	char *inter;
	int in_bytes;
	int in_packets;
	int out_bytes;
	int out_packets;
	FILE *nf;
	struct s_packUsage *next;
} packUsage;

typedef struct s_process
{
	char *name;
	int pid;
	int ppid;
	int cpu_time;
	char *user_name;
	char *cmd_line;
	FILE *pf;
	struct s_process *next;
} procInfo;

/*
 * include func
 */

//parse.c
void *pth_parse_cpu(void *cp);
void *pth_parse_mem(void *me);
void *pth_parse_packet(void *pac);
void *pth_parse_process(void *pro);

//util.c
void err_by(char *reason);
FILE *read_cmd(FILE *fp, char *cmd);

//index_utils.c
int indx_space(char *line, int i);
int indx_go_next(char *line, int i);
int indx_get_num(char *line, int i);

# endif
