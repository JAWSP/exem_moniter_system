#include "server.h"
#include "../agent/object.h"
#include "../agent/packets.h"

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

int main()
{

	int sock = 0;
	int res = 0;
	int agent_fd[13] = {0, };// 여러개의
	char buf[1024 * 128];
	struct sockaddr_in server_addr;

	//통신 설정 초기 셋팅
	int optval = 1;

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		err_by("server sock error");

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(1234);

	res = bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (res < 0)
		err_by("server bind error");

	listen(sock, 13);

	//TODO 여기서 연결 끊기고 다시 연결되어도 계속 되어야 한다

	//while recv 이부분 수정해야함
	int i = 0;
	while (1)
	{
		agent_fd[i] = accept_agent(sock, &i);
		while (1)
		{
			if (recv(agent_fd[i], buf, 1024 *128, 0) > 0)
				test(buf);
			else
				break ;
		}	
	}

	close(sock);
	close(agent_fd[i]);

	return (0);
}
