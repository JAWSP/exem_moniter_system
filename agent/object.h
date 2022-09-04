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
#include "packets.h"
#include "queue.h"

#define BUFF_SIZE 1024
#define LOCAL_ADDR 2130706433

typedef struct s_header header;
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
//아마 여기서 전역변수를 쓸 것 같은 느낌이 드는데
typedef struct s_data
{

} data;

//unsigned int

/*
 * include func
 */

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
header *insert_header(header *head, char type);

# endif
