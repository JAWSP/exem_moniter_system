#include "packets.h"
#include "object.h"
#include "queue.h"
#include <fcntl.h>

extern g_lobal *g;

queue *init_queue(queue *q)
{
	q = (queue *)malloc(sizeof(queue));
	if (!q)
		err_by("queue malloc error");
	q->front = 0;
	q->rear = 0;
	pthread_mutex_init(&q->q_lock, NULL);

	return (q);
}

int is_empty(queue *q)
{
    if (q->front == q->rear)
		return (1);
	return (0);
}

int is_full(queue *q)
{
	if (((q->rear + 1) % QUEUE_MAX) == q->front)
		return (1);
	return (0);
}

packet *dequeue(queue *q)
{
	if (is_empty(q))
		printf("queue is empty\n");
	else
		q->front = (q->front + 1) % QUEUE_MAX;
	return (q->packets[q->front]);
}

queue *enqueue(queue *q, packet *pack)
{
	pthread_mutex_lock(&q->q_lock);

	if (is_full(q))
	{
		printf("queue is full\n");
		packet *old = dequeue(q);
		free(old->data);
		old->data = NULL;
		free(old);
		old = NULL;
	}
	else
	{
		q->rear = (q->rear + 1) % QUEUE_MAX;
		q->packets[q->rear] = pack;
	}
	pthread_mutex_unlock(&q->q_lock);

	return (q);
}


void send_data(queue *q, int sock)
{
	pthread_mutex_lock(&q->q_lock);

	packet *pack = dequeue(q);
	if (send(sock, pack->data, pack->len, 0) < 0)
	{
		printf("it's not access yet\n");
		usleep(920);
		pthread_mutex_unlock(&q->q_lock);
		return ;
	}
	usleep(916);
	free(pack->data);
	pack->data = NULL;
	free(pack);
	pack = NULL;

	pthread_mutex_unlock(&q->q_lock);
}

void				signal_handle_p(int sig)
{
	//여기에 소켓 변수를 전역변수화 
	(void)sig;
	close(g->socket);
	g->socket = socket(PF_INET, SOCK_STREAM, 0);
	g->agent_addr.sin_family = AF_INET;
	g->agent_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	g->agent_addr.sin_port = htons(1234);
	if (g->socket == -1)
		err_by("socket error");
	usleep(1000 * 300);
	for (int i = 0; i < 15; i++)
	{
		if (connect(g->socket, (struct sockaddr*)&(g->agent_addr), sizeof(g->agent_addr)) == -1)
		{
			printf("waiting.....\n");
			usleep(1000 * 456);
		}
		else
		{
			char buf[9];
			int fd = 0;
			int res = 0;
			if ((send(g->socket, g->key, 8, 0)) < 0)
				err_by("send failed");
			if ((recv(g->socket, buf, 2, 0)) < 0)
				err_by("recv failed");
			if (strcmp(buf, "OK") != 0)
				err_by("your key is not certified\n");
			printf("reconnect success\n");
			return ;
		}
	}
	err_by("agent connect error");
}

void *pth_queue_process(void *pq)
{
	queue *q = pq;
	signal(SIGPIPE, signal_handle_p);

	while (1)
	{
		if (is_empty(q))
		{
			usleep(916);
			continue ;
		}
		else
		{
			send_data(q, g->socket);
		}
	}
}
