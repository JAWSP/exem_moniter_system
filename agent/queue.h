#ifndef QUEUE_H
# define QUEUE_H

#include "packets.h"
#include "object.h"

#define QUEUE_MAX 30

typedef struct s_packet packet;

typedef struct s_queue
{
	unsigned int front;
	unsigned int rear;
	int socket;
	pthread_mutex_t q_lock;
	struct s_packet *packets[QUEUE_MAX];
} queue;

queue *init_queue(queue *q);

int is_empty(queue *q);
int is_full(queue *q);
queue *enqueue(queue *q, packet *pack);
packet *dequeue(queue *q);
void send_data(queue *q, int sock);

#endif
