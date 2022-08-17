#include "object.h"

void parse_cpu(FILE *fs, cpuUsage *cpu)
{
	char buf[BUFF_SIZE];
	int i = 0;

	fgets(buf, BUFF_SIZE, fs);
	if (ferror(fs))
		err_by("stat parse error");

	i = indx_go_next(buf, i);
	//get user time
	//atoi를 쓰면 core dumped하면서 seg에러가 뜸
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
		err_by("stat parse error");

	i = indx_go_next(buf, i);
	mem->total = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	mem->used = indx_get_num(buf, i);
	//free영역은 그대로 측정해야 할까?
	//아님 위의 두개의 값의 차이를 넣어야 할까?
	i = indx_go_next(buf, i);
	mem->free = indx_get_num(buf, i); //현재 mem 영역
	
	fgets(buf, BUFF_SIZE, fs);
	if (ferror(fs))
		err_by("stat parse error");
	i = indx_go_next(buf, i);
	mem->swap_total = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	mem->swap_used = indx_get_num(buf, i); //swap mem영역
//	printf("buf : %s", buf);
}

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
    	//fs = read_cmd(fs, "cat /proc/net/dev");
        pclose(fs);
		free(cpu);
		free(mem);
		free(pack);
		free(proc);

        return (0);
}
