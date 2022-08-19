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

	(*cpu)->cf = NULL;
	(*mem)->mf = NULL;
	(*pack)->paf = NULL;
	(*proc)->prf = NULL;

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
		while (tmp && tmp->inter)
		{
			printf("interface = %s, in byte : %d, pac : %d, out byte : %d, pac : %d\n",
					tmp->inter, tmp->in_bytes, tmp->in_packets, tmp->out_bytes, tmp->out_packets);
			tmp = tmp->next;
		}
		procInfo *tmp2 = proc;
		while (tmp2 && tmp2->name)
		{
			printf("name = %s, pid : %d, ppid : %d, cpu usage : %d, username %s, cmdline %s\n",
					tmp2->name, tmp2->pid, tmp2->ppid, tmp2->cpu_time, tmp2->user_name, tmp2->cmd_line);
			tmp2 = tmp2->next;
		}
}

int main(void)
{
		cpuUsage *cpu = NULL;
		memUsage *mem = NULL;
		packUsage *pack = NULL;
		procInfo *proc = NULL;

		init_structs(&cpu, &mem, &pack, &proc);
	
		cpu->cf = read_cmd(cpu->cf, "cat /proc/stat");
		parse_cpu(cpu->cf, cpu);
	
		mem->mf = read_cmd(mem->mf, "free");
		parse_mem(mem->mf, mem);

		pack->paf = read_cmd(pack->paf, "cat /proc/net/dev");
		parse_packet(pack->paf, pack);

		proc->prf = read_cmd(proc->prf, "ls /proc");
		parse_process(proc->prf, proc);
		
		test(cpu, mem, pack, proc);


		//닫기
        pclose(cpu->cf);
        pclose(mem->mf);
        pclose(pack->paf);
        pclose(proc->prf);
		free(cpu);
		cpu = NULL;
		free(mem);
		mem = NULL;
		pack_free(&pack);
		proc_free(&proc);

        return (0);
}
