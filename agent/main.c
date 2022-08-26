#include "object.h"

/*
void init_structs(cpuUsage **cpu, memUsage **mem, packUsage **pack, procInfo **proc )
{
	*cpu = (cpuUsage*)malloc(sizeof(cpuUsage));
	if (!*(cpu))
		err_by("malloc_error");
	*mem = (memUsage*)malloc(sizeof(memUsage));
	if (!*(mem))
		err_by("malloc_error");
	*pack = (packUsage*)malloc(sizeof(packUsage));
	if (!(*pack))
		err_by("malloc_error");
	*proc = (procInfo*)malloc(sizeof(procInfo));
	if (!(*proc))
		err_by("malloc_error");
	
	(*pack)->inter = NULL;
	(*pack)->next = NULL;
	(*proc)->name = NULL;
	(*proc)->next = NULL;
}
*/


/*
void test(cpuUsage *cpu, memUsage *mem, packUsage *pack, procInfo *proc)
{
		printf("usr = %d,sys = %d, idle = %d, iowait = %d\n",
				cpu->usr, cpu->sys, cpu->idle, cpu->iowait);
		printf("total = %d, used = %d, free = %d, swap_toal = %d, swap_used = %d\n",
				mem->total, mem->used, mem->free, mem->swap_total, mem->swap_used);
		packUsage *tmp = pack;
		while (tmp && tmp->inter)
		{
			printf("interface = %s, in byte : %d, pac : %d, out byte : %d, pac : %d\n",
					tmp->inter, tmp->in_bytes, tmp->in_packets, tmp->out_bytes, tmp->out_packets);
			tmp = tmp->next;
		}
		procInfo *tmp2 = proc;
		while (tmp2 && tmp2->name)
		{
			printf("name = %s, pid : %d, ppid : %d, cpu usage : %d, username %s, cmdline %s\n",
					tmp2->name, tmp2->pid, tmp2->ppid, tmp2->cpu_time, tmp2->user_name, tmp2->cmd_line);
			tmp2 = tmp2->next;
		}
}
*/



int main(void)
{
	//TODO 받은 내용 계산할 부분은 계산시키기
	//TODO 가독성 있게 맞추기
	//TODO 소켓화 시켜서 넘겨주기
	
	int sock;
	struct sockaddr_in agent_addr;

	/*
	cpuUsage *cpu = NULL;
	memUsage *mem = NULL;
	packUsage *pack = NULL;
	procInfo *proc = NULL;
	
	pthread_t pid_c, pid_m, pid_n, pid_p;
	*/

	//TODO 이쪽에 socket생성
	//그 뿐만 아니라 각종 초기화를 이쪽에서 하면 좋을 듯해보인다
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		err_by("socket error");

	memset(&agent_addr, 0, sizeof(struct sockaddr_in));

	agent_addr.sin_family = AF_INET;
	agent_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	agent_addr.sin_port = htons(1234);

	if (connect(sock, (struct sockaddr*)&agent_addr, sizeof(agent_addr)) == -1)
		err_by("agent connect error");

	for (int i = 0; i < 5; i++)
		usleep(1000* 1000);
	char *ex = strdup("asd\n");
	send(sock, ex, strlen(ex), 0);
	free(ex);
	close(sock);


	//요 whilea문을 부수는게 맞을까?
//	while (1)
//	{
//		init_structs(&cpu, &mem, &pack, &proc);
		
		/*
		pthread_create(&pid_c, NULL, pth_parse_cpu, (void *)cpu);
		pthread_create(&pid_m, NULL, pth_parse_mem, (void *)mem);
		pthread_create(&pid_n, NULL, pth_parse_packet, (void *)pack);
		pthread_create(&pid_p, NULL, pth_parse_process, (void *)proc);
	
		//join을 살린 이유
		//이걸 안쓰면 main에서 먼저 끝내게 됨
		//그리고 스레드는 무한으로 돌아서 메인-서브 스레드간의 동기화문제는 상솬없다고 생각
		pthread_join(pid_c, NULL);
		pthread_join(pid_m, NULL);
		pthread_join(pid_n, NULL);
		pthread_join(pid_p, NULL);
		*/

//		usleep(1000 * 1000);
//	} //main while	


        return (0);
}
