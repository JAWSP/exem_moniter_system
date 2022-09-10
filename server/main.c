#include "server.h"
#include "../agent/object.h"
#include "../agent/packets.h"

g_serv *gs;

int accept_agent(int sock, int *i)
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

int get_recv(char *buf, int size, agentInfo *ag)
{
	header *ttt;
	static int full_size = 0;
	static int curr_size = 0;

	if (curr_size == 0)
	{
		ttt = (header *)(buf);
		printf("test : %u, %s, %d, recv size : %d, full size : %d\n",
				ttt->id, ttt->type_n_date, ttt->count, size, ttt->size);
		ag->type = ttt->type_n_date[0];
		//만약에 받은 타입에 문제가 생기면 종료
		//TODO 나중에 여유가 생기면 id도 유효하지 않으면 지우는걸로
		if (ag->type != 'c' || ag->type != 'm' || ag->type != 'n' || ag->type != 'p')
		{
			free(ag);
			return (-1);
		}
		curr_size += size;
		full_size = ttt->size;
		ag->id = ttt->id;
		strcpy(ag->date, &(ttt->type_n_date[2]));
		memcpy(ag->raw_data, buf, size);
	}
	else
	{
		memcpy(ag->raw_data + curr_size, buf, size);
		curr_size += size;
	}
	if (curr_size == full_size)
	{
		//대충 유효한지 판단하거나 큐에다 집어넣는 함수
		curr_size = 0;
		free(ag);
	}
	return (0);
}

void *pth_server_loop(void *arg)
{
	char buf[1024 * 256];
	int agent_fd = gs->agent_fd;
	int size;

	while (1)
	{
		agentInfo *ag;
		if (!(ag = (agentInfo *)malloc(sizeof(agentInfo))))
			err_by("agent info malloc error");
		if ((size = recv(agent_fd, buf, 1024 *256, 0)) > 0)
		{
			if (get_recv(buf, size, ag) == -1)
				break ;
		}
		else
			break ; //연결이 끊기면 루프끝->스레드끝
	}
	return ((void*)0);
}

int main()
{
	int res = 0;
	pthread_t pid;
	struct sockaddr_in server_addr;

	//global init
	//mutex_init
	if (!(gs = (g_serv *)malloc(sizeof(g_serv))))
		err_by("global malloc error");

	//통신 설정 초기 셋팅
	int optval = 1;
	gs->sock = socket(PF_INET, SOCK_STREAM, 0);
	if (gs->sock < 0)
		err_by("server sock error");
	//재실행시 기존에 bind했었던 값을 다시 bind시키는 옵션
	if (setsockopt(gs->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		err_by("server socket set failed");

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1234);

	res = bind(gs->sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (res < 0)
		err_by("server bind error");

	listen(gs->sock, 13);
	//TODO 여기서 연결 끊기고 다시 연결되어도 계속 되어야 한다
	int i = 0;
	while (1)
	{
		gs->agent_fd = accept_agent(gs->sock, &i);
		if (pthread_create(&pid, NULL, pth_server_loop, (void *)gs) == -1)
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
