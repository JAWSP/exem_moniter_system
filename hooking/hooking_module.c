#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <pwd.h>

#define BUFF_SIZE 1024

typedef struct s_packet
{
	unsigned char *data;
	int len;
} packet;
//시간상 id는 못넣었으나, 이부분은 메인 패킷의 헤더에 있음
typedef struct s_before
{
	char agent_name[8];
	char ip[15];
	char begin_time[20];
	int port;
	int pid;
} before;

typedef struct s_after
{
	char agent_name[8];
	double elapse_time;
	int pid;
	int size;
} after;

ssize_t (*real_send)(int socket, const void *buffer, size_t length, int flags);

static int sock;
static struct sockaddr_in s_serv_addr;
static int s_port;
static char s_ip[15];
static int s_pid;
static char s_name[9];

/*
여기서부터 함수들
*/

void err_by(char *reason)
{
	perror(reason);
	exit(-1);
}

DIR *open_dir(DIR *dir, char *root)
{
	dir = opendir(root);
	if (dir == NULL)
		err_by("diropen failed");

	return (dir);
}

FILE *open_fs(FILE *fs, char *root)
{
	fs = fopen(root, "r");
	if (fs == NULL)
		err_by("fopen failed");

	return (fs);
}

char *get_curr_time(void)
{
	time_t curr;
	struct tm *t;
	static char res[20];

	curr = time(NULL);
	t = localtime(&curr);

	sprintf(res, "%d-%d-%d %d:%d:%d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	return (res);
}

char *get_curr_day(void)
{
	time_t curr;
	struct tm *t;
	static char res[11];

	curr = time(NULL);
	t = localtime(&curr);

	sprintf(res, "%d-%d-%d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);

	return (res);
}

/*
여기까지가 각종 유틸리티들
*/


void get_this_info(void)
{
	struct dirent *buf = NULL;
	DIR *dir = NULL;
	FILE *fs = NULL;
	int pid = 0;
	char cmd[40], tmp[1024], name[512];

	//net info
	s_port = ntohs(s_serv_addr.sin_port);
	inet_ntop(AF_INET, &s_serv_addr.sin_addr, s_ip, INET_ADDRSTRLEN);
	printf("port : %d, ip : %s\n", s_port, s_ip);
	//process info
	dir = open_dir(dir, "/proc");
	while ((buf = readdir(dir)) != NULL)
	{
		if ((pid = atoi(buf->d_name)) > 0)
		{
			sprintf(cmd, "/proc/%d/status", pid);
			fs = open_fs(fs, cmd);	
			fgets(tmp, BUFF_SIZE, fs);
			if (!sscanf(tmp, "%*s %s", name))
				err_by("process name sscanf error");
			name[strlen(name)] = '\0';
			if (strcmp("monitor", name) == 0)
			{
					strcpy(s_name ,name);
					s_pid = pid;
					fclose(fs);
					closedir(dir);
					return ;
			}
			fclose(fs);
		}
	}
	err_by("not found process info");
}

__attribute__((constructor))
void before_run()
{
	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == -1)
		err_by("socket error");

	memset(&(s_serv_addr), 0, sizeof(struct sockaddr_in));

	s_serv_addr.sin_family = AF_INET;
	s_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_serv_addr.sin_port = htons(5678);

	get_this_info();	

	real_send = (ssize_t (*)(int, const void *, size_t, int))dlsym(RTLD_NEXT, "send");
}

/*
* 이 아래부터 후킹 관련
*/

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
	if (flags == 123123)
	{
		ssize_t result = 0;
		result = (*real_send)(socket, buffer, length, 0);
		return (result);
	}

	//각각 udp 패킷을 할당
	packet *before_p;
	before_p = (packet*)malloc(sizeof(packet));

	packet *after_p;
	after_p = (packet*)malloc(sizeof(packet));

	if (!before_p || !after_p)
		err_by("udp pack malloc_error");
	
	before_p->len  = sizeof(before) + 1;
	before_p->data = (unsigned char *)malloc(before_p->len);

	after_p->len  = sizeof(after) + 1;
	after_p->data = (unsigned char *)malloc(after_p->len);
	//시간설정
	double start, end;

	//before값 넣고 보내기
	before *b;
	b = (before *)before_p->data;
	strcpy(b->agent_name, s_name);
	strcpy(b->ip, s_ip);
	strcpy(b->begin_time, get_curr_time());
	b->port = 1234;
	b->pid = s_pid;

	start = (double)clock() / CLOCKS_PER_SEC;
	sendto(sock, before_p->data, before_p->len, 0,
		(struct sockaddr*)&s_serv_addr, sizeof(s_serv_addr));

	//본체 보내기
	ssize_t result = 0;
	result = (*real_send)(socket, buffer, length, flags);
	printf ("send size : %ld\n", length);

	//after값 넣고 보내기
	after *a;
	a = (after *)after_p->data;
	strcpy(a->agent_name, s_name);
	//a->elapse_time =
	a->pid = s_pid;
	a->size = length;

	end = (double)clock() / CLOCKS_PER_SEC;
	a->elapse_time = end - start;

	sendto(sock, after_p->data, after_p->len, 0,
		(struct sockaddr*)&s_serv_addr, sizeof(s_serv_addr));

	//NULL TIME
	free(before_p->data);
	before_p->data = NULL;
	free(before_p);
	before_p = NULL;
	free(after_p->data);
	after_p->data = NULL;
	free(after_p);
	after_p = NULL;
	return (result);
}
