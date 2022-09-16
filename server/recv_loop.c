#include "server.h"
#include "server_queue.h"
#include "../agent/object.h"

extern g_serv *gs;

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
		strcpy(ag->recv_date, get_curr_day());
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

		curr_size = 0;
		full_size = 0;
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