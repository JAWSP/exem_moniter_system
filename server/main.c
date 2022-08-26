#include "server.h"

int main()
{

	int sock;
	int res = 0;

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

	return (0);
}
