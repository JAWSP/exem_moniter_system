#include "object.h"

void init_structs(cpuUsage **cpu, memUsage **mem, packUsage **pack, procInfo **proc)
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

void pack_free(packUsage **head)
{
    packUsage *del = NULL;
    packUsage *tmp = *head;
    while (tmp)
    {
        del = tmp;
        tmp = tmp->next;
		free(del->inter);
		del->inter = NULL;
        free(del);
		del = NULL;
    }
	*head = NULL;
}

void proc_free(procInfo **head)
{
    procInfo *del = NULL;
    procInfo *tmp = *head;
    while (tmp)
    {
        del = tmp;
        tmp = tmp->next;
		//인자들 free
		free(del->name);
		del->name = NULL;
		free(del->user_name);
		del->user_name = NULL;
		free(del->cmd_line);
		del->cmd_line = NULL;
        free(del);
		del = NULL;
    }
	*head = NULL;
}

void test(cpuUsage *cpu, memUsage *mem, packUsage *pack, procInfo *proc)
{
		printf("usr = %d,sys = %d, idle = %d, iowait = %d\n", cpu->usr, cpu->sys, cpu->idle, cpu->iowait);
		printf("total = %d, used = %d, free = %d, swap_toal = %d, swap_used = %d\n",
				mem->total, mem->used, mem->free, mem->swap_total, mem->swap_used);
		packUsage *tmp = pack;
		while (tmp)
		{
			printf("interface = %s, in byte : %d, pac : %d, out byte : %d, pac : %d\n",
					tmp->inter, tmp->in_bytes, tmp->in_packets, tmp->out_bytes, tmp->out_packets);
			tmp = tmp->next;
		}
		procInfo *tmp2 = proc;
		while (tmp2)
		{
			printf("name = %s, pid : %d, ppid : %d, cpu usage : %d, username %s, cmdline %s\n",
					tmp2->name, tmp2->pid, tmp2->ppid, tmp2->cpu_time, tmp2->user_name, tmp2->cmd_line);
			tmp2 = tmp2->next;
		}
}

int main(void)
{
        FILE *fs = NULL;
		cpuUsage *cpu = NULL;
		memUsage *mem = NULL;
		packUsage *pack = NULL;
		procInfo *proc = NULL;

		init_structs(&cpu, &mem, &pack, &proc);
//		fs = read_cmd(fs, "cat Makefile");
		fs = read_cmd(fs, "cat /proc/stat");
		parse_cpu(fs, cpu);
		//TODO
		fs = read_cmd(fs, "free");
//		fs = read_cmd(fs, "ls -al");
		parse_mem(fs, mem);
    	fs = read_cmd(fs, "cat /proc/net/dev");
		parse_packet(fs, pack);

		fs = read_cmd(fs, "ls /proc");
		parse_process(fs, proc);
		
		test(cpu, mem, pack, proc);


		//닫기
        pclose(fs);
		free(cpu);
		cpu = NULL;
		free(mem);
		mem = NULL;
		pack_free(&pack);
		proc_free(&proc);

        return (0);
}
