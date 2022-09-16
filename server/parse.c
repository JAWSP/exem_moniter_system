#include "server_queue.h"
#include "server.h"
#include "../agent/packets.h"

extern g_serv *gs;

void write_header(agentInfo *ag, int fd, char *msg)
{
	char output[256];

	sprintf(output, "-------------------- id %d's %s, collect time : %s ---------------------\n\n",
		ag->id,
		msg,
		ag->date
	);
	pthread_mutex_lock(&gs->g_lock);
	write(fd, output, strlen(output));
	pthread_mutex_unlock(&gs->g_lock);	
}

void write_tail(agentInfo *ag, int fd, char *msg)
{
	char output[256];

	sprintf(output, "\n---------------- id %d's %s, write time : %s | raw : %d ----------------\n\n\n",
		ag->id,
		msg,
		get_curr_time(),
		ag->count
	);
	pthread_mutex_lock(&gs->g_lock);
	write(fd, output, strlen(output));
	pthread_mutex_unlock(&gs->g_lock);	
}

void parse_cpu(agentInfo *ag, int fd)
{
    char output[256];

	write_header(ag, fd, "cpuUsageTime");
    cpuUsage *cu = (cpuUsage *)(ag->raw_data + sizeof(header));

    sprintf(output, "[%s], usr = %d, sys = %d, idle = %d, iowait = %d\n",
		 ag->recv_date, cu->usr, cu->sys, cu->idle, cu->iowait);

	pthread_mutex_lock(&gs->g_lock);
	write(fd, output, strlen(output));
	write_tail(ag, fd, "cpuUsageTime");
	close(fd);
	pthread_mutex_unlock(&gs->g_lock);	
}

void parse_mem(agentInfo *ag, int fd)
{
    char output[256];

	write_header(ag, fd, "memUsage");
    memUsage *mu = (memUsage *)(ag->raw_data + sizeof(header));

    sprintf(output, "[%s], total = %d, used = %d, free = %d | swap_toal = %d, swap_used = %d\n",
				ag->recv_date, mu->total, mu->used, mu->free, mu->swap_total, mu->swap_used);  

	pthread_mutex_lock(&gs->g_lock);
	write(fd, output, strlen(output));
	write_tail(ag, fd, "memUsage");
	close(fd);
	pthread_mutex_unlock(&gs->g_lock);
}

void parse_pack(agentInfo *ag, int fd)
{
    char output[256];

	write_header(ag, fd, "Net packet in/out");
    packUsage *pu = (packUsage *)(ag->raw_data + sizeof(header));
	int i = 0;

	while (i < ag->count)
	{
		sprintf(output, "[%s] interface : %s | in bytes : %d, in packets : %d | out bytes : %d, out packets : %d\n",
					ag->recv_date,
					pu->inter,
					pu->in_bytes,
					pu->in_packets,
					pu->out_bytes,
					pu->out_packets
				);
		pthread_mutex_lock(&gs->g_lock);
		write(fd, output, strlen(output));
		pthread_mutex_unlock(&gs->g_lock);

		i++;
		pu++;
	}
	pthread_mutex_lock(&gs->g_lock);
	write_tail(ag, fd, "Net packet in/out");
	pthread_mutex_unlock(&gs->g_lock);
	close(fd);
}

void parse_proc(agentInfo *ag, int fd)
{
	char output[1024];
	write_header(ag, fd, "Process Information");
	procInfo *pu = (procInfo *)(ag->raw_data + sizeof(header));
	int i = 0;
		while (i < ag->count)
	{
		sprintf(output, "[%s] process name = %s | pid : %d, ppid : %d, cpu usage : %d, username %s, cmdline %s\n",
					ag->recv_date,
					pu->name,
					pu->pid,
					pu->ppid,
					pu->cpu_time,
					pu->user_name,
					pu->cmd_line
				);
		pthread_mutex_lock(&gs->g_lock);
		write(fd, output, strlen(output));
		pthread_mutex_unlock(&gs->g_lock);

		i++;
		pu++;
	}
	pthread_mutex_lock(&gs->g_lock);
	write_tail(ag, fd, "Process Information");
	pthread_mutex_unlock(&gs->g_lock);
	
	close(fd);
}
