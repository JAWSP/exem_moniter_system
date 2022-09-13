#ifndef SERVER_QUEUE_H
# define SERVER_QUEUE_H

#include "server.h"

#define QUEUE_MAX 30

typedef struct s_agentInfo agentInfo;

typedef struct serv_queue
{
	unsigned int front;
	unsigned int rear;
	pthread_mutex_t q_lock;
	struct s_agentInfo *agents[QUEUE_MAX];
} squeue;

squeue *init_squeue(squeue *q);

int s_empty(squeue *q);
int s_full(squeue *q);
squeue *s_enqueue(squeue *q, agentInfo *ag);
agentInfo *s_dequeue(squeue *q);
//void send_data(squeue *q, int sock);
void *pth_squeue_process(void *pq);

#endif
