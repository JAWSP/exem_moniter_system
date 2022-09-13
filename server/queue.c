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
	if (s_empty(q))
		printf("queue is empty\n");
	else
		q->front = (q->front + 1) % QUEUE_MAX;
	return (q->agents[q->front]);
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

/*
void send_data(squeue *q, int sock)
{
	pthread_mutex_lock(&q->q_lock);

	packet *pack = dequeue(q);
	if (send(sock, pack->data, pack->len, 0) < 0)
	{
		printf("it's not access yet\n");
		usleep(920);
		pthread_mutex_unlock(&q->q_lock);
		return ;
		err_by("packet send error");
	}
	usleep(100);
	free(pack->data);
	pack->data = NULL;
	free(pack);
	pack = NULL;

	pthread_mutex_unlock(&q->q_lock);
}
*/

void *pth_squeue_process(void *pq)
{
	squeue *q = (squeue *)pq;

	while (1)
	{
		if (s_empty(q))
		{
			usleep(409);
			continue ;
		}
		else
		{
			//send_data(q, g->socket);
		}
	}
}
