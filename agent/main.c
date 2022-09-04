#include "object.h"
#include "packets.h"
#include "queue.h"


int main(void)
{
	//TODO 받은 내용 계산할 부분은 계산시키기
	//TODO 소켓화 시켜서 넘겨주기
	
	struct sockaddr_in agent_addr;
	queue *q = NULL;
	q = init_queue(q);

	pthread_t pid_c, pid_m, pid_n, pid_p, pid_q;

	//그 뿐만 아니라 각종 초기화를 이쪽에서 하면 좋을 듯해보인다
	q->socket = socket(PF_INET, SOCK_STREAM, 0);
	if (q->socket == -1)
		err_by("socket error");

	memset(&agent_addr, 0, sizeof(struct sockaddr_in));

	agent_addr.sin_family = AF_INET;
	agent_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	agent_addr.sin_port = htons(1234);

	if (connect(q->socket, (struct sockaddr*)&agent_addr, sizeof(agent_addr)) == -1)
		err_by("agent connect error");

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
