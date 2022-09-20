#ifndef PACKETS_H
# define PACKETS_H

#include "object.h"
#include "../utils/utils.h"
#include "signal.h"

#pragma pack(push, 1)

//before => header, UDP
//body => body, TCP
//after => tail, UDP
//tail은 따로 할 것인가..?

typedef struct s_packet
{
	unsigned char *data;
	int len;
} packet;

typedef struct s_header //나중에 before가 된다
{
	char id[8];
	char type_n_date[22];
	char agent_name[8];
	char ip[15];
	int port;
	int pid;
	int count;
	int size;
} header;

/*
typedef struct s_before
{
	char id[8];
	char type_n_date[22];
	char agent_name[8];
	char ip[15];
	int port;
	int pid;
	int size;
}
*/

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
	char inter[16];
	unsigned int in_bytes;
	unsigned int in_packets;
	unsigned int out_bytes;
	unsigned int out_packets;
} packUsage;


typedef struct s_process
{
	char name[16];
	unsigned int pid;
	unsigned int ppid;
	unsigned int cpu_time;
	char user_name[32];
	char cmd_line[526];
	struct s_process *next;
} procInfo;

#pragma pack(pop)


#endif
