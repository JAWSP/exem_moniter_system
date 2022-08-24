#include "object.h"

/*
TODO
해당 스레드를 죽이지 않고 지속적으로 살려야 한다
싱크 -> 걸린시간의 차를 이용하여 특정 시간대에 맞게 설정해야 하고
그러면서 한번 수집이 끝났으면 각각 패킷에 넣으면 됨
*/

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
	FILE *fs = NULL;
	struct passwd pwd;
    struct passwd *result;
	char cmd[40], buf[BUFF_SIZE], name[BUFF_SIZE];
	int utime, stime, uid;
	int i = 0;
	//TODO 경과시간 jiffies으로 나누어서 구하시오

	//stat parse
	
	//get pid,ppid,raw name
	sprintf(cmd, "/proc/%d/status", pid);
	fs = open_fs(fs, cmd);
	while (fgets(buf, BUFF_SIZE, fs) != 0)
	{
		if (i == 0)
		{
			if (!sscanf(buf, "%*s %s", name))
				err_by("process name sscanf error");
			proc->name = strdup(name);
		}
		else if (i == 5)
		{
			if (!sscanf(buf, "%*s %d", &proc->pid))
				err_by("process pid sscanf error");
		}
		else if (i == 6)
		{
			if (!sscanf(buf, "%*s %d", &proc->ppid))
				err_by("process ppid sscanf error");
		}
		else if (i == 8)
		{
			if (!sscanf(buf, "%*s %d", &uid))
				err_by("process uid sscanf error");
			getpwuid_r(uid, &pwd, name, BUFF_SIZE, &result);
			if (result == NULL)
				err_by("getpwuid_r error");
			proc->user_name = strdup(pwd.pw_name);
			break ;
		}
		i++;
	}

	//cpu time은 stat에 위치해있다
	fclose(fs);
	i = 0;
	buf[0] = '\0';
	sprintf(cmd, "/proc/%d/stat", pid);
	fs = open_fs(fs, cmd);
	for (int count = 0; count < 13; count++)
		i = indx_go_next(buf, i);
	if (!sscanf(&buf[i], "%d %d", &utime, &stime))
		err_by("process cputime  sscanf error");
	proc->cpu_time = utime + stime;
	
	fclose(fs);
	//cmdline parse
	sprintf(cmd, "/proc/%d/cmdline", pid);
	fs = open_fs(fs, cmd);
	buf[0] = '\0';
	fgets(buf, BUFF_SIZE, fs);
	if (strlen(buf) == 0)
		proc->cmd_line = strdup("");
	else
		proc->cmd_line = strdup(buf);

	fclose(fs);
	return (proc);
}

void *pth_parse_process(void *pro)
{
	procInfo *proc = pro;
	struct dirent *buf = NULL;
	int pid = 0;
	DIR *dir = proc->dir;

	while ((buf = readdir(dir)) != NULL)
	{
		if ((pid = atoi(buf->d_name)) > 0)
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
	}
	
	return ((void*)0);
}
