#define _GNU_SOURCE
#include <stdio.h>
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

typedef struct s_before //나중에 before가 된다?
{
	char id[8];
	char agent_name[8];
	char ip[15];
	int port;
	int pid;
	int count;
	int size;
} before;

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


ssize_t (*real_send)(int socket, const void *buffer, size_t length, int flags);

static int sock;
static struct sockaddr_in s_agent_addr;
static int s_port;
static int s_ip;
static int s_pid;
static char s_name[9];

void err_by(char *reason)
{
	perror(reason);
	exit(-1);
}

void get_this_info(void)
{
	struct dirent *buf = NULL;
	DIR *dir = NULL;
	FILE *fs = NULL;
	int pid = 0;
	char cmd[40], tmp[1024], name[512];

	//net info
	s_port = ntohs(s_agent_addr.sin_port);
	inet_ntop(AF_INET, &s_agent_addr.sin_addr, s_ip, INET_ADDRSTRLEN);
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
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		err_by("socket error");

	memset(&(s_agent_addr), 0, sizeof(struct sockaddr_in));

	s_agent_addr.sin_family = AF_INET;
	s_agent_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_agent_addr.sin_port = htons(1234);

	get_this_info();	

	printf("YEAAAAAAHHHHHH\n");

	real_send = (ssize_t (*)(int, const void *, size_t, int))dlysm(RTLD_NEXT, send);
}

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
	if (socket == 0)
		socket = sock;
	return (*real_send)(socket, buffer, length, flags);
}
