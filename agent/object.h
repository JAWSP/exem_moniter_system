#ifndef OBJECT_H
# define OBJECT_H

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
#include "../utils/utils.h"

#define BUFF_SIZE 1024
#define LOCAL_ADDR 2130706433


/*
 * structs
 * 각각의 수집한 정보를 구조체 형식으로 저장
 */

//OS사용 부분을 합친부분을 쓸지는 일단 보류

//임시로 만들 전역변수
/*
typedef struct s_lobal
{
	struct timeval start_time;
	int tmp;
} g_lobal;
*/

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
	char *inter;
	int in_bytes;
	int in_packets;
	int out_bytes;
	int out_packets;
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

//agent_utils.c
void pack_free(packUsage **head);
void proc_free(procInfo **head);

# endif
