#include "object.h"

void parse_stat(FILE *fs, cpuUsage *stat)
{
	char buf[BUFF_SIZE];
	fgets(buf, BUFF_SIZE, fs);
	if (ferror(fs))
		err_by("stat parse error");
	int i = 0;
	i = indx_go_next(buf, i);
	//get user time
	//atoi를 쓰면 core dumped하면서 seg에러가 뜸
	stat->usr = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	i = indx_go_next(buf, i);
	//sys는 3번째에 위치해있으니 첫번째 위치에서 2번넘긴다
	stat->sys = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	stat->idle = indx_get_num(buf, i);
	i = indx_go_next(buf, i);
	stat->iowait = indx_get_num(buf, i);

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
		parse_stat(fs, cpu);
		printf("usr = %d,sys = %d, idle = %d, iowait = %d\n", cpu->usr, cpu->sys, cpu->idle, cpu->iowait);
		//TODO
       // fs = read_cmd(fs, "free");
    	//fs = read_cmd(fs, "cat /proc/net/dev");
        pclose(fs);
		free(cpu);
		free(mem);
		free(pack);
		free(proc);

        return (0);
}
