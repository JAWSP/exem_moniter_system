#ifndef PACKETS_H
# define PACKETS_H
//위에 이름 바꾸셈

#include "object.h"
#include "../utils/utils.h"
/*
 * 어찌 짤것임?
 * -> 구조체를 만들어야지
 *  header, body를 어찌 만들것인가
 */

/*
typedef enum e_type
{
	cpu,
	mem,
	pack,
	proc
} types;
*/
#pragma pack(push, 1)

typedef struct s_packet
{
	//대충 헤더
	char type_n_date[22];
	int size; 
} packet;

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
	struct s_packUsage *next;
} packUsage;

typedef struct s_process
{
	char name[16];
	unsigned int pid;
	unsigned int ppid;
	unsigned int cpu_time;
	char user_name[32];
	//얜 최대가 3만바이트인데어찌 처리하지?
	//일단 저것에 대한 최대 길이를 측정해보는게 나을듯
	char cmd_line[1024];
	struct s_process *next;
} procInfo;

#pragma pack(pop)


#endif
