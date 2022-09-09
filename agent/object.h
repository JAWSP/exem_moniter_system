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
#include <time.h>
#include "../utils/utils.h"
#include "packets.h"
#include "queue.h"

#define BUFF_SIZE 1024
#define LOCAL_ADDR 2130706433

typedef struct s_header header;

typedef struct s_lobal
{
	unsigned int agent_id;
	struct sockaddr_in agent_addr;
	int socket;
} g_lobal;

//parse.c
void *pth_parse_cpu(void *pq);
void *pth_parse_mem(void *pq);
void *pth_parse_packet(void *pq);
void *pth_parse_process(void *pq);
void *pth_queue_process(void *pq);

//agent_utils.c
char *get_curr_time(void);
int get_count(char type, char *cmd);
packet *init_packet(packet *pack, char type, int count);
header *insert_header(header *head, char type, packet *p);

# endif
