#include "object.h"

//TODO 파싱 과정 어느정도 획일화 하게
//TODO popen 대신 다른 파싱 방법 찾아보기
void *pth_parse_cpu(void *cp)
{
	cpuUsage *cpu = cp;
	char buf[BUFF_SIZE];
	int i = 0;
	FILE *fs = cpu->cf;

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

	return ((void*)0);
}

void *pth_parse_mem(void *me)
{
	memUsage *mem = me;
	char buf[BUFF_SIZE];
	int i = 0;
	FILE *fs = mem->mf;
	
	while (fgets(buf, BUFF_SIZE, fs))
	{
		if (i == 0)
		{
			if (!sscanf(buf, "%*s %d", &mem->total))
				err_by("total mem  sscanf error");
		}
		else if (i == 1)
		{
			if (!sscanf(buf, "%*s %d", &mem->free))
				err_by("free mem sscanf error");
			mem->used = mem->total - mem->free;
		}
		else if (i == 14)
		{
			if (!sscanf(buf, "%*s %d", &mem->swap_total))
				err_by("swap total mem sscanf error");
		}
		else if (i == 15)
		{
			int tmp;
			if (!sscanf(buf, "%*s %d", &tmp))
				err_by("swap free sscanf error");
			mem->swap_used = mem->swap_total - tmp;
		}
		else if (i >= 16)
			break ;
		i++;
	}	
		if (ferror(fs))
			err_by("mem parse error");
	/*
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
	*/
	
	return ((void*)0);
}

packUsage *insert_packet(char *buf, packUsage *pack)
{
	int i = 0;
	char name[30];

	i = indx_space(buf, i);
	if (!sscanf(&buf[i], "%s %d %d", name, &pack->in_bytes, &pack->in_packets))
		err_by("rx packet sscanf error");
	name[strlen(name) - 1] = '\0'; //해당 단어 뒤 : 문자 제거를 위함;
	//해당 변수를 pack->inter[30]처럼 크기를 바로 할당을 할지 아니면 이후에 dup시킬지 고민함
	//dup이 더 안정적일 것 같아서 넣음
	pack->inter = strdup(name);
	for (int count = 0; count < 9; count++)
		i = indx_go_next(buf, i);
	if (!sscanf(&buf[i], "%d %d", &pack->out_bytes, &pack->out_packets))
		err_by("tx packet sscanf error");
	return (pack);
}

void *pth_parse_packet(void *pac)
{
	packUsage *pack = pac;
	char buf[BUFF_SIZE];
	FILE *fs = pack->nf;
	
	//원하고자 하는 내용은 3번째 줄에 있다
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
	
	return ((void*)0);
}

procInfo *insert_proc(int pid, procInfo *proc)
{
	//TODO 함수를 쪼개야 함
	FILE *fc = NULL;
	char cmd[40], buf[BUFF_SIZE], name[BUFF_SIZE];
	int utime, stime;

	//stat parse
	sprintf(cmd, "cat /proc/%d/stat", pid);
	fc = read_cmd(fc, cmd);
	fgets(buf, BUFF_SIZE, fc);
	if (ferror(fc))
		err_by("proc/pidstat get error");
	if (!sscanf(buf, "%d %s %*s %d", &proc->pid, name, &proc->ppid))
		err_by("proc/pid/stat sscanf error");

	//get name
	name[strlen(name) -1] = '\0';
	proc->name = strdup(&name[1]);

	//get cpu time
	int i = 0;
	for (int count = 0; count < 14; count++)
		i = indx_go_next(buf, i);
	if (!sscanf(&buf[i], "%d %d", &utime, &stime))
		err_by("process cputime  sscanf error");
	//TODO 경과시간 jiffies으로 나누어서 구하시오
	proc->cpu_time = utime + stime;

	//username parse	
	sprintf(cmd, "ps -u -p %d", pid);
	fc = read_cmd(fc, cmd);
	fgets(buf, BUFF_SIZE, fc);
	fgets(buf, BUFF_SIZE, fc);
	if (ferror(fc))
		err_by("usrname get error");
	if (!sscanf(buf, "%s",name))
		err_by("username sscanf error");
	proc->user_name = strdup(name);	

	//cmdline parse
	sprintf(cmd, "cat /proc/%d/cmdline", pid);
	fc = read_cmd(fc, cmd);
	buf[0] = '\0';
	fgets(buf, BUFF_SIZE, fc);
	if (strlen(buf) == 0)
		proc->cmd_line = strdup("NULL");
	else
		proc->cmd_line = strdup(buf);

	pclose(fc);
	return (proc);
}

void *pth_parse_process(void *pro)
{
	procInfo *proc = pro;
	char buf[BUFF_SIZE];
	int pid = 0;
	FILE *fs = proc->pf;

	while (!feof(fs))
	{
		fgets(buf, BUFF_SIZE, fs);
		if (ferror(fs))
			err_by("pid parse error");
		if ((pid = atoi(buf)) > 0)
		{
			if (proc->name)
			{
				procInfo *new;
				procInfo *tmp = proc;
				new = (procInfo*)malloc(sizeof(procInfo));
				if (!new)
					err_by("new pack malloc_error");
				new->name = NULL;
				new->next = NULL;
				insert_proc(pid, new);
				while (tmp->next)
					tmp = tmp->next;
				tmp->next = new;
				continue ;
			}
			insert_proc(pid, proc);
		}
		else
			break ; //이후로는 Pid가 안나오므로
	}
	
	return ((void*)0);
}
