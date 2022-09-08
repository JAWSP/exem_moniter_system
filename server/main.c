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

void test(char *buf)
{
	header *ttt;
	ttt = (header *)buf;
	printf("test : %s, %d\n", ttt->type_n_date, ttt->count);
}

void *pth_server_loop(void *arg)
{
	char buf[1024 * 128];
	int agent_fd = gs->agent_fd;

	while (1)
	{
		if (recv(agent_fd, buf, 1024 *128, 0) > 0)
			test(buf);
		else
			break ;
	}
	return ((void*)0);
}

int main()
{
	int res = 0;
	int agent_fds[13] = {0, };// 여러개의
	char buf[1024 * 128];
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
		for (int c = 0; c < 13; c++)
			agent_fds[c] = gs->agent_fd;
		pthread_create(&pid, NULL, pth_server_loop, (void *)gs);
		/*
		while (1)
		{
			if (recv(agent_fd, buf, 1024 *128, 0) > 0)
				test(buf);
			else
				break ;
		}
		*/
	}

	close(gs->sock);
	close(gs->agent_fd);

	return (0);
}
