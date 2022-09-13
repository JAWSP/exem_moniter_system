#include "server.h"
#include "server_queue.h"
#include "../agent/object.h"

g_serv *gs;

int accept_agent(int sock)
{
	struct sockaddr_in agent_addr;
	socklen_t as;
	int agent_fd = 0;

	as = sizeof(agent_addr);
	agent_fd  = accept(sock, (struct sockaddr *)&agent_addr, &as);
	if (agent_fd < 0)
		err_by("accept failed");

	return (agent_fd);
}

int get_recv(char *buf, int size, agentInfo *ag, squeue *q)
{
	header *ttt;
	static int full_size = 0;
	static int curr_size = 0;

	if (curr_size == 0)
	{
		ttt = (header *)(buf);

		ag->id = ttt->id;
		ag->type = ttt->type_n_date[0];
		ag->count = ttt->count;
		strcpy(ag->date, &(ttt->type_n_date[2]));
		printf("test : %u, %s, %d, recv size : %d, full size : %d\n",
				ttt->id, ttt->type_n_date, ttt->count, size, ttt->size);
		//만약에 받은 타입에 문제가 생기면 종료
		//TODO 나중에 여유가 생기면 id도 유효하지 않으면 지우는걸로
		if (ag->type != 'c' && ag->type != 'm' && ag->type != 'n' && ag->type != 'p')
		{
			printf("TYPE %c IS NOT VERIFIED\n", ag->type);
			free(ag);
			return (-1);
		}
		curr_size += size;
		full_size = ttt->size;
		memcpy(ag->raw_data, buf, size);
	}
	else
	{
		memcpy(ag->raw_data + curr_size, buf, size);
		curr_size += size;
	}
	if (curr_size == full_size)
	{
		//for test
		/*
		if (ag->type == 'p')
		{
			ttt = (header *)(ag->raw_data);
			procInfo *tmp2 = (procInfo *)(ag->raw_data + sizeof(header));
			for (int loop = 0; loop < ttt->count; loop++)
			{
				printf("name = %s, pid : %d, ppid : %d, cpu usage : %d, username %s, cmdline %s\n",
						tmp2->name, tmp2->pid, tmp2->ppid, tmp2->cpu_time, tmp2->user_name, tmp2->cmd_line);
				tmp2++;
			}
		}
		*/
		q = s_enqueue(q, ag);
		//대충 유효한지 판단하거나 큐에다 집어넣는 함수
		//근데 큐가 막히면 어쩌지

		curr_size = 0;
		full_size = 0;
		free(ag);
		ag = NULL;
	}
	return (0);
}

void *pth_server_loop(void *arg)
{
	char buf[1024 * 256];
	int agent_fd = gs->agent_fd;
	int size;
	squeue *q = (squeue *)arg;

	while (1)
	{
		agentInfo *ag;
		if (!(ag = (agentInfo *)malloc(sizeof(agentInfo))))
			err_by("agent info malloc error");
		if ((size = recv(agent_fd, buf, 1024 *256, 0)) > 0)
		{
			if (get_recv(buf, size, ag, q) == -1)
				break ;
		}
		else
			break ; //연결이 끊기면 루프끝->스레드끝
	}
	return ((void*)0);
}

void init_serv(squeue **q, struct sockaddr_in *server_addr)
{
	//global init
	//queue init
	if (!(gs = (g_serv *)malloc(sizeof(g_serv))))
		err_by("global malloc error");
	pthread_mutex_init(&gs->g_lock, NULL);
	*q = init_squeue(*q);

	//통신 설정 초기 셋팅
	//소켓 설정
	int optval = 1;

	gs->sock = socket(PF_INET, SOCK_STREAM, 0);
	if (gs->sock < 0)
		err_by("server sock error");
	//재실행시 기존에 bind했었던 값을 다시 bind시키는 옵션
	if (setsockopt(gs->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		err_by("server socket set failed");

	//설정한 소켓에 ip와 port를 설정하고 bind
	memset(server_addr, 0, sizeof(*server_addr));
	server_addr->sin_family = AF_INET;
	server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr->sin_port = htons(1234);

	int res = 0;
	
	res = bind(gs->sock, (struct sockaddr*)server_addr, sizeof(*server_addr));
	if (res < 0)
		err_by("server bind error");
	listen(gs->sock, 13);
}

int main(void)
{
	pthread_t pid, q_pid;
	struct sockaddr_in server_addr;
	squeue *q;

	init_serv(&q, &server_addr);


	if (pthread_create(&q_pid, NULL, pth_squeue_process, (void *)gs) == -1)
		err_by("queue pthread create failed");
	while (1)
	{
		gs->agent_fd = accept_agent(gs->sock);
		if (pthread_create(&pid, NULL, pth_server_loop, (void *)q) == -1)
		{
			printf("recv pthread create failed\n");
			continue ;
		}
		pthread_detach(pid);
	}

	close(gs->sock);
	close(gs->agent_fd);

	return (0);
}
