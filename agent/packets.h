#ifndef PACKETS_H
# define PACKETS_H

#include "object.h"
#include "../utils/utils.h"
#include "signal.h"

#pragma pack(push, 1)

typedef struct s_packet
{
	unsigned char *data;
	int len;
} packet;

typedef struct s_header
{
	char id[8];
	char type_n_date[22];
	int count;
	int size;
} header;

//여기가 바디
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
