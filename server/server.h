#ifndef SERVER_H
# define SERVER_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include "../utils/utils.h"

typedef struct serv_queue squeue;
typedef struct serv_global
{ 	
	int sock;
	int flag;
	int agent_fd;
	char ids[9][9];
	char keys[9][9];
	pthread_mutex_t g_lock;
	pthread_mutex_t g_comu_lock;
}g_serv;

typedef struct s_agentInfo
{
	//이후에 길이를 받은 즉시 할당으로 바뀔 수 있음
	char raw_data[1024 * 256];
	char id[8];
	char type;
	char date[20];
	char recv_date[20];
	int count;
	int indx;
}agentInfo;

//parse.c
void parse_cpu(agentInfo *ag, int fd);
void parse_mem(agentInfo *ag, int fd);
void parse_pack(agentInfo *ag, int fd);
void parse_proc(agentInfo *ag, int fd);

//recv_loop.c
void *pth_server_loop(void *arg);
int get_recv(char *buf, int size, agentInfo *ag, squeue *q);

//serv_utils.c
char *get_curr_day(void);
char *get_curr_time(void);
int certification(void);

#endif
