#include "object.h"

void parse_stat(FILE *fs, cpuUsage *stat)
{
	char buf[BUFF_SIZE], *res;
	res = fgets(buf, BUFF_SIZE, fs);
	if (ferror(fs))
		err_by("stat parse error");
	int i = 3; //cpu 하고 띄우기
	i = indx_space(&buf[i], i);
	//get user time
	stat->usr = atoi(&buf[i]);
	i = indx_go_next(&buf[i], i);
	i = indx_go_next(&buf[i], i);
	//sys는 3번째에 위치해있으니 첫번째 위치에서 2번넘긴다
	stat->sys = atoi(&buf[i]);
	i = indx_go_next(&buf[i], i);
	stat->idle = atoi(&buf[i]);
	i = indx_go_next(&buf[i], i);
	stat->iowait = atoi(&buf[i]);

	printf("usr = %d,sys = %d, idle = %d, iowait = %d\n", stat->usr, stat->sys, stat->idle, stat->iowait);
}

void init_structs(cpuUsage *stat, memUsage*mem, packUsage *pack, procInfo *proc)
{
	stat = malloc(sizeof(cpuUsage));
	if (!stat)
		err_by("malloc_error");
	mem = malloc(sizeof(memUsage));
	if (!mem)
		err_by("malloc_error");
	pack = malloc(sizeof(packUsage));
	if (!pack)
		err_by("malloc_error");
	proc = malloc(sizeof(procInfo));
	if (!proc)
		err_by("malloc_error");
}

int main(void)
{
        FILE *fs = NULL;
		cpuUsage *stat = NULL;
		memUsage *mem = NULL;
		packUsage *pack = NULL;
		procInfo *proc = NULL;


		init_structs(stat, mem, pack, proc);
//		fs = read_cmd(fs, "ls -al");
		fs = read_cmd(fs, "cat /proc/stat");
		parse_stat(fs, stat);
		printf("usr = %d,sys = %d, idle = %d, iowait = %d\n", stat->usr, stat->sys, stat->idle, stat->iowait);
		//TODO
       // fs = read_cmd(fs, "free");
    	//fs = read_cmd(fs, "cat /proc/net/dev");
        pclose(fs);

        return (0);
}
