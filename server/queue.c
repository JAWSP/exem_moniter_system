#include "../agent/object.h"
#include "server_queue.h"
#include "server.h"

squeue *init_squeue(squeue *q)
{
	q = (squeue *)malloc(sizeof(squeue));
	if (!q)
		err_by("queue malloc error");
	q->front = 0;
	q->rear = 0;
	pthread_mutex_init(&q->q_lock, NULL);

	return (q);
}

int s_empty(squeue *q)
{
    if (q->front == q->rear)
		return (1);
	return (0);
}

int s_full(squeue *q)
{
	if (((q->rear + 1) % QUEUE_MAX) == q->front)
		return (1);
	return (0);
}

agentInfo *s_dequeue(squeue *q)
{
	pthread_mutex_lock(&q->q_lock);
	
	if (s_empty(q))
		printf("queue is empty\n");
	else
		q->front = (q->front + 1) % QUEUE_MAX;
	return (q->agents[q->front]);
	
	pthread_mutex_unlock(&q->q_lock);
}

squeue *s_enqueue(squeue *q, agentInfo *ag)
{
	pthread_mutex_lock(&q->q_lock);

	if (s_full(q))
	{
		printf("queue is full\n");
	}
	else
	{
		q->rear = (q->rear + 1) % QUEUE_MAX;
		q->agents[q->rear] = ag;
	}
	pthread_mutex_unlock(&q->q_lock);

	return (q);
}

void *pth_parse_loop(void *pq)
{
	int fd;
	char path[16];

	squeue *q = (squeue *)pq;
	agentInfo *ag = NULL;
	ag = s_dequeue(q);
	sprintf(path, "../data/%d", ag->id);

	fd = open(path, O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd == -1)
		err_by("open failed");
	if (ag->type == 'c')
		parse_cpu(ag, fd);
	/*
	else if (ag->type == 'm')
		parse_mem(ag, fd);
	else if (ag->type == 'n')
		parse_pack(ag ,fd);
	else if (ag->type == 'p')
		parse_proc(ag ,fd);
	*/
	free(ag);
	ag = NULL;
	return ((void *)0);
}

void *pth_squeue_process(void *pq)
{
	squeue *q = (squeue *)pq;
	pthread_t p_pid;

	while (1)
	{
		if (s_empty(q))
		{
			usleep(409);
			continue ;
		}
		else
		{
			//TODO 현재 상황은 큐에 새로운 값이 들어올때마다 스레드를 생성
			//임시로 이렇게 만들었지만 이에 대한 리펙토링이 필요함
			if (pthread_create(&p_pid, NULL, pth_parse_loop, (void *)q) == -1)
			{
				err_by("parse thread create error");
				continue ;
			}
			pthread_detach(p_pid);
		}
	}
}
