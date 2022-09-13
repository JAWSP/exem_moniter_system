#include "server_queue.h"
#include "server.h"
#include "../agent/packets.h"

extern g_serv *gs;

void parse_cpu(agentInfo *ag, int fd)
{
    char output[256];

    cpuUsage *cu = (cpuUsage *)(ag->raw_data + sizeof(header));
    sprintf(output, "id : %d, type : %c, date : %s, usr = %d,sys = %d, idle = %d, iowait = %d\n",
		ag->id, ag->type, ag->date, cu->usr, cu->sys, cu->idle, cu->iowait);
	pthread_mutex_lock(&gs->g_lock);
	write(fd, output, strlen(output));
	close(fd);
	pthread_mutex_unlock(&gs->g_lock);
}

void parse_mem(agentInfo *ag, int fd)
{
    char output[256];

    memUsage *mu = (memUsage *)(ag->raw_data + sizeof(header));
    sprintf(output, "id : %d, type : %c, date : %s, total = %d, used = %d, free = %d, swap_toal = %d, swap_used = %d\n",
				ag->id, ag->type, ag->date, mu->total, mu->used, mu->free, mu->swap_total, mu->swap_used);   
	pthread_mutex_lock(&gs->g_lock);
	write(fd, output, strlen(output));
	close(fd);
	pthread_mutex_unlock(&gs->g_lock);
}

void parse_pack(agentInfo *ag, int fd)
{
    char output[256];

    packUsage *pu = (packUsage *)(ag->raw_data + sizeof(header));
	int i = 0;
	while (i < ag->count)
	{
		sprintf(output, "id : %d, type : %c, date : %s, interface = %s, in byte : %d, pac : %d, out byte : %d, pac : %d\n",
					ag->id, ag->type, ag->date, pu->inter, pu->in_bytes, pu->in_packets, pu->out_bytes, pu->out_packets);

		pthread_mutex_lock(&gs->g_lock);
		write(fd, output, strlen(output));
		pthread_mutex_unlock(&gs->g_lock);
		
		i++;
		pu++;
	}
	
	close(fd);
}
