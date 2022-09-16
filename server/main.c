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


	if (pthread_create(&q_pid, NULL, pth_squeue_process, (void *)q) == -1)
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
