#include "server.h"
#include "../agent/object.h"
#include "../agent/packets.h"

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

void test(packet *a_packet)
{	
//	cpuUsage *cpu = NULL;
	printf("test : %s\n", a_packet->type_n_date);
//	cpu = a_packet->body;

//	printf("usr = %d,sys = %d, idle = %d, iowait = %d\n",
//	cpu->usr, cpu->sys, cpu->idle, cpu->iowait);
}

int main()
{

	int sock;
	int res = 0;
	int agent_fd;
//	char buf[1024];
	packet *a_packet = NULL;
	struct sockaddr_in server_addr;

	//통신 설정 초기 셋팅
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

	listen(sock, 5);

	agent_fd = accept_agent(sock);

//	int num;
//	recv(agent_fd, &num, sizeof(int), 0);
//	printf("a is %d\n", num);

	while (recv(agent_fd, a_packet, 1024 * 256, 0))
		test(a_packet);

//	printf("buf : %lu\n", strlen(buf));
//	if (strlen(buf) != 4)
//		printf("WTF\n");
//	int i = 0;
//	while (buf[i])
//		printf("%c\n", buf[i++]);
//	printf("recv : %s\n", buf);

//	close(sock);
//	close(agent_fd);



	return (0);
}
