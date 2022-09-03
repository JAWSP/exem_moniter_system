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

//packet엔 더럽게 긴 data가 있고
//그 data엔 헤더랑 바디에 대한 포인터가 들어가게 됨
//근데 할당하고 넣어야 하는가? 이는 해봐야 알듯함
//데이터에 헤더를 넣고 바디는 데이터에 헤더만큼 옮긴 부분의 길이를 구하면 될듯

typedef struct s_packet
{
	//보낼때 data만 보낸다 패킷통채로 보내는게 아님
	unsigned char *data;
	int len;
} packet;

typedef struct s_header
{
	//대충 헤더
	char type_n_date[22];
	int count;
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
//	struct s_packUsage *next;
} packUsage;

/*
char *buf = malloc(len);
packet = (s_packUsage *)buf;
for (int i = 0; i < count; i++)
{
	packet-> ?? = !!;
	packet++;
}

char *buf = malloc(length);
s_packUsage *packet;

packet = (s_packUsage *)buf;
packet->in_bytes = 10;

len = fc()
char *buf = malloc(len);

packet = (s_packUsage *)buf;
packet->in_bytes = 10;
packet++; 
....
*/

typedef struct s_process
{
	char name[16];
	unsigned int pid;
	unsigned int ppid;
	unsigned int cpu_time;
	char user_name[32];
	//얜 최대가 3만바이트인데어찌 처리하지?
	//일단 저것에 대한 최대 길이를 측정해보는게 나을듯
	char cmd_line[526];
	struct s_process *next;
} procInfo;

#pragma pack(pop)


#endif
