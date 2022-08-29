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

//unsigned int
typedef struct s_cpuUsage
{
	unsigned int usr;
	unsigned int sys;
	unsigned int iowait;
	unsigned int idle;
} cpuUsage;

typedef struct s_memUsage
{
	unsigned int free;
	unsigned int total;
	unsigned int used;
	unsigned int swap_total;
	unsigned int swap_used;
} memUsage;

typedef struct s_packUsage
{
	char *inter;
	unsigned int in_bytes;
	unsigned int in_packets;
	unsigned int out_bytes;
	int out_packets;
	struct s_packUsage *next;
} packUsage;

typedef struct s_process
{
	char *name;
	unsigned int pid;
	unsigned int ppid;
	unsigned int cpu_time;
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
