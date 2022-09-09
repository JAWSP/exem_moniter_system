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
#include "../utils/utils.h"

typedef struct serv_global
{ 	
	int sock;
	int flag;
	int agent_fd;
	pthread_mutex_t mutx;
}g_serv;

typedef struct s_agentInfo
{
	char *raw_data;
	int id;
	char type;
	char date[20];
}agentInfo;

#endif
