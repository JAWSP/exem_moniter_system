#include "server.h"
#include "server_queue.h"
#include "../agent/object.h"

extern g_serv *gs;

int get_recv(char *buf, int size, agentInfo *ag, squeue *q, phase *phase)
{
	header *ttt;
	static int full_size = 0;
	static int curr_size = 0;

	if (curr_size == 0)
	{
		ttt = (header *)(buf);

		strcpy(ag->id, ttt->id);
		if (strncmp(ag->id, gs->ids[ag->indx], 8) != 0)
		{
			printf("id is not same : you : %s, serv : %s\n", ag->id, gs->ids[ag->indx]);
			return (-1);
		}
		ag->type = ttt->type_n_date[0];
		ag->count = ttt->count;
		//TODO ag->id의 값이 갑자기 변한 이유 찾기
		strcpy(ag->date, &(ttt->type_n_date[2]));
		strcpy(ag->recv_date, get_curr_day());
		printf("test : %s, %s, %d, recv size : %d, full size : %d\n",
				gs->ids[ag->indx], ttt->type_n_date, ttt->count, size, ttt->size);
		
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
		q = s_enqueue(q, ag);

		curr_size = 0;
		full_size = 0;
		phase++;
	}
	return (0);
}

void *pth_server_loop(void *arg)
{
	char buf[1024 * 256];
	char ubuf[256];
	int agent_fd = gs->agent_fd;
	int size;
	squeue *q = (squeue *)arg;
	phase phase;

	int res = 0;
	pthread_mutex_lock(&gs->g_comu_lock);
	if ((res = certification()) < 0)
	{
		pthread_mutex_unlock(&gs->g_comu_lock);
		printf("certification error\n");
		if (res == -1)
			printf("your id is 8 character");
		else if (res == -2)
			printf ("your id is not in table");
		return ((void*)0);
	}
	pthread_mutex_unlock(&gs->g_comu_lock);

	phase = PHASE_READY;
	while (1)
	{
		struct sockaddr_in agt_adr;
		socklen_t agt_adr_len;
		agentInfo *ag;
		int ress = 0;
		if (phase == PHASE_READY)
		{
			if (!(ag = (agentInfo *)malloc(sizeof(agentInfo))))
				err_by("agent info malloc error");
			
			agt_adr_len = sizeof(agt_adr);
			phase = PHASE_BEFORE;
		}
		if (phase == PHASE_BEFORE)
		{
			if ((ress = (recvfrom(gs->usock, ubuf, 256, 0, (struct sockaddr *)&agt_adr, &agt_adr_len))) < 0)
				err_by("before packet recv error");
			printf("before : %d\n", res);
			phase = PHASE_MAIN;
		}
		ag->indx = res;
		if (phase == PHASE_MAIN)
		{
			if ((size = recv(agent_fd, buf, 1024 *256, 0)) > 0)
			{
				if (get_recv(buf, size, ag, q, &phase) == -1)
					break ;
			}
			else
				break ; //연결이 끊기면 루프끝->스레드끝
		}
		if (phase == PHASE_AFTER)
		{
			if ((ress = (recvfrom(gs->usock, ubuf, 256, 0, (struct sockaddr *)&agt_adr, &agt_adr_len))) < 0)
				err_by("before packet recv error");
			printf("after : %d\n", ress);
			phase = PHASE_BEFORE;
		}
	}
	return ((void*)0);
}
