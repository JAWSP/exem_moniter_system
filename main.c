#include "object.h"

void parse_stat(FILE *fs)
{
	char buf[BUFF_SIZE], *res;
	res = fgets(buf, BUFF_SIZE, fs);
	if (ferror(fs))
		err_by("stat parse error");
	int i = 3; //cpu 하고 띄우기
	i = indx_spaces(&buf[i], i);
	//get user time
	i = indx_get_num(&buf[i], i);
	i = indx_spaces(&buf[i], i);
}

//구조체를 초기화 시키는 함수
void init(cpuUsage *stat, memUsage*mem, packUsage *pack, process *process)
{
}

int main(void)
{
        FILE *fs = NULL;
		cpuUsage *stat = NULL;
		memUsage *mem = NULL;
		packUsage *pack = NULL;
		process *process = NULL;


		init(stat, mem, pack, process);
        fs = read_cmd(fs, "cat /proc/stat");
		parse_stat(fs);
		//TODO
       // fs = read_cmd(fs, "free");
    	//fs = read_cmd(fs, "cat /proc/net/dev");
        pclose(fs);

        return (0);
}
