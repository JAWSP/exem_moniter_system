#include "object.h"
#include "packets.h"
#include "queue.h"
#include <fcntl.h>

g_lobal *g;

int certification(char *arg)
{
	int res = 0;
	char buf[9];
	int fd = 0;
	strcpy(g->id, arg);

	if ((fd = open("./key", O_RDWR | O_TRUNC | O_CREAT, 0644)) < 0)
		err_by("open key failed");
	if ((res = read(fd, buf, 8)) < 0)
		err_by("read key failed");
	if (res != 0) //재접속시
	{
		if ((send(g->socket, buf, 8, 0)) < 0)
			err_by("send failed");
		strcpy(g->key, buf);
		if ((recv(g->socket, buf, 2, 0)) < 0)
			err_by("recv failed");
		if (strcmp(buf, "OK") == 0)
		{
			close(fd);
			return (1);
		}
		else
		{
			printf("your key is not certified\n");
			close(fd);
			return (-1);
		}
	}
	else //처음 접속이라면
	{
		if ((send(g->socket, arg, 8, 0)) < 0)
			err_by("send failed");
		else
			printf("success\n");
		if ((recv(g->socket, buf, 8, 0)) < 0)
			err_by("recv failed");
		buf[8] = 0;

		//만약 처음 접속이라면 서버에게 키값을 받음
		if (write(fd, buf, 8) < 0)
			err_by("write key failed");
		strcpy(g->key, buf);
	}
	close(fd);
	return (1);
}

int main(int argc, char **argv)
{
	//struct sockaddr_in agent_addr;
	if (argc > 2)
		err_by("plz input one argument");
	queue *q = NULL;
	q = init_queue(q);	
	if (!(g = (g_lobal *)malloc(sizeof(g_lobal))))
		err_by("global malloc error");


	pthread_t pid_c, pid_m, pid_n, pid_p, pid_q;

	g->socket = socket(PF_INET, SOCK_STREAM, 0);
	if (g->socket == -1)
		err_by("socket error");

	//id설정
//	srand(time(NULL));
//	g->agent_id = (rand() % 99999999);	

	memset(&(g->agent_addr), 0, sizeof(struct sockaddr_in));

	g->agent_addr.sin_family = AF_INET;
	g->agent_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	g->agent_addr.sin_port = htons(1234);

	if (connect(g->socket, (struct sockaddr*)&(g->agent_addr), sizeof(g->agent_addr)) == -1)
		err_by("agent connect error");

	if (!certification((argc == 2) ? argv[1] : "testtest"))
		err_by("certification failed!");

	pthread_create(&pid_c, NULL, pth_parse_cpu, (void *)q);
	pthread_create(&pid_m, NULL, pth_parse_mem, (void *)q);
	pthread_create(&pid_n, NULL, pth_parse_packet, (void *)q);
	pthread_create(&pid_p, NULL, pth_parse_process, (void *)q);
	pthread_create(&pid_q, NULL, pth_queue_process, (void *)q);

	//join을 살린 이유
	//이걸 안쓰면 main에서 먼저 끝내게 됨
	//그리고 스레드는 무한으로 돌아서 메인-서브 스레드간의 동기화문제는 상솬없다고 생각
	pthread_join(pid_c, NULL);
	pthread_join(pid_m, NULL);
	pthread_join(pid_n, NULL);
	pthread_join(pid_p, NULL);

        return (0);
}
