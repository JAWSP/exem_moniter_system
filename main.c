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
	(*pack)->nf = NULL;
	(*proc)->dir = NULL;

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
		printf("usr = %d,sys = %d, idle = %d, iowait = %d\n",
				cpu->usr, cpu->sys, cpu->idle, cpu->iowait);
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
	//TODO 받은 내용 계산할 부분은 계산시키기
	//TODO 가독성 있게 맞추기
	//TODO 소켓화 시켜서 넘겨주기
	//TODO 저기에 스레드 뿐만 아니라 여러번 측정해야 하니 그부분도 신경써줘야함

		cpuUsage *cpu = NULL;
		memUsage *mem = NULL;
		packUsage *pack = NULL;
		procInfo *proc = NULL;
		pthread_t pid_c;
		pthread_t pid_m;
		pthread_t pid_n;
		pthread_t pid_p;;

		while (1)
		{
			init_structs(&cpu, &mem, &pack, &proc);
		
			cpu->cf = open_fs(cpu->cf, "/proc/stat");
			pthread_create(&pid_c, NULL, pth_parse_cpu, (void *)cpu);
		
			mem->mf = open_fs(mem->mf, "/proc/meminfo");
			pthread_create(&pid_m, NULL, pth_parse_mem, (void *)mem);

			pack->nf = open_fs(pack->nf, "/proc/net/dev");
			pthread_create(&pid_n, NULL, pth_parse_packet, (void *)pack);

			proc->dir = open_dir(proc->dir, "/proc");
			pthread_create(&pid_p, NULL, pth_parse_process, (void *)proc);
			
			pthread_join(pid_c, 0);
			pthread_join(pid_m, 0);
			pthread_join(pid_n, 0);
			pthread_join(pid_p, 0);
			test(cpu, mem, pack, proc);


			//닫기

			free(cpu);
			cpu = NULL;
			free(mem);
			mem = NULL;
			pack_free(&pack);
			proc_free(&proc);

		}	
		pclose(cpu->cf);
		pclose(mem->mf);
		pclose(pack->nf);
		closedir(proc->dir);

        return (0);
}
