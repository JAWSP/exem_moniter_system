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
	(*pack)->inter = NULL;
	(*pack)->next = NULL;
	*proc = (procInfo*)malloc(sizeof(procInfo));
	if (!(*proc))
		err_by("malloc_error");
	(*proc)->name = NULL;
	(*proc)->next = NULL;
}

void parse_cpu(FILE *fs, cpuUsage *cpu)
{
	char buf[BUFF_SIZE];
	int i = 0;

	fgets(buf, BUFF_SIZE, fs);
	if (ferror(fs))
		err_by("stat parse error");

	i = indx_go_next(buf, i);
	//get user time
	cpu->usr = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	i = indx_go_next(buf, i);
	//sys는 3번째에 위치해있으니 첫번째 위치에서 2번넘긴다
	cpu->sys = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	cpu->idle = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	cpu->iowait = indx_get_num(buf, i);
}

void parse_mem(FILE* fs, memUsage *mem)
{
	char buf[BUFF_SIZE];
	int i = 0;
	
	//원하고자 하는 정보는 2번째 줄에 있으니 
	//개행까지 읽어들이는 fgets를 2번 불러온다
	fgets(buf, BUFF_SIZE, fs);
	fgets(buf, BUFF_SIZE, fs);
	if (ferror(fs))
		err_by("mem parse error");

	i = indx_go_next(buf, i);
	mem->total = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	mem->used = indx_get_num(buf, i);
	//free영역은 그대로 측정해야 할까?
	//아님 위의 두개의 값의 차이를 넣어야 할까?
	i = indx_go_next(buf, i);
	mem->free = indx_get_num(buf, i); //현재 mem 영역

	i = 0;
	fgets(buf, BUFF_SIZE, fs);
	if (ferror(fs))
		err_by("swap_mem parse error");
	i = indx_go_next(buf, i);
	mem->swap_total = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	mem->swap_used = indx_get_num(buf, i); //swap mem영역
}

packUsage *insert_packet(char *buf, packUsage *pack)
{
	int i = 0;
	char name[30];

	i = indx_space(buf, i);
	if (!sscanf(&buf[i], "%s %d %d", name, &pack->in_bytes, &pack->in_packets))
		err_by("rx packet sscanf error");
	name[strlen(name) - 1] = '\0'; //해당 단어 뒤 : 문자 제거를 위함;
	pack->inter = strdup(name);
	for (int count = 0; count < 9; count++)
		i = indx_go_next(buf, i);
	if (!sscanf(&buf[i], "%d %d", &pack->out_bytes, &pack->out_packets))
		err_by("tx packet sscanf error");
	return (pack);
}

void parse_packet(FILE *fs, packUsage *pack)
{
	char buf[BUFF_SIZE];
	
	fgets(buf, BUFF_SIZE, fs);
	fgets(buf, BUFF_SIZE, fs);
	while (fgets(buf, BUFF_SIZE, fs) != 0)
	{
		if (ferror(fs))
			err_by("net socket parse error");
		if (pack->inter)
		{
			packUsage *new;
			packUsage *tmp = pack;
			new = (packUsage*)malloc(sizeof(packUsage));
			if (!new)
				err_by("new pack malloc_error");
			new->inter = NULL;
			new->next = NULL;
			insert_packet(buf, new);
			while (tmp->next)
				tmp = tmp->next;
			tmp->next = new;
			//대충 새로운 노드를 만들어서 뒤에 붙이는 함수
		}
		else
			insert_packet(buf, pack);
	}
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
        free(del);
		del = NULL;
    }
	*head = NULL;
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
		printf("usr = %d,sys = %d, idle = %d, iowait = %d\n", cpu->usr, cpu->sys, cpu->idle, cpu->iowait);
		//TODO
		fs = read_cmd(fs, "free");
//		fs = read_cmd(fs, "ls -al");
		parse_mem(fs, mem);
		printf("total = %d, used = %d, free = %d, swap_toal = %d, swap_used = %d\n", mem->total, mem->used, mem->free, mem->swap_total, mem->swap_used);
    	fs = read_cmd(fs, "cat /proc/net/dev");
		parse_packet(fs, pack);
		packUsage *tmp = pack;
		while (tmp)
		{
			printf("interface = %s, in byte : %d, pac : %d, out byte : %d, pac : %d\n", tmp->inter, tmp->in_bytes, tmp->in_packets, tmp->out_bytes, tmp->out_packets);
			tmp = tmp->next;
		}

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
