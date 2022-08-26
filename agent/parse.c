#include "object.h"

/*
TODO
해당 스레드를 죽이지 않고 지속적으로 살려야 한다
싱크 -> 걸린시간의 차를 이용하여 특정 시간대에 맞게 설정해야 하고
그러면서 한번 수집이 끝났으면 각각 패킷에 넣으면 됨
근데 접속이 끊겨도 그대로 전달해주기 위한 큐(혹은 연결리스트)가 필요하게됨
->이말인 즉슨 바로 연결이 된다면 1초간격으로 넣은 얘들을 죄다 집어 넣게 만들게 되는건가보네
*/

void *pth_parse_cpu(void *cp)
{
	char buf[BUFF_SIZE];
	FILE *fs = NULL;
	int i;
	double diff_usec = 0;
	struct timeval startTime, endTime;

	while (1)
	{	
		i = 0;
		diff_usec = 0;
		cpuUsage *cpu;
		//먼저 초기화 할껀 초기화

		cpu = (cpuUsage*)malloc(sizeof(cpuUsage));
		if (!cpu)
			err_by("malloc_error");
	//	double diff_sec; ->1초를 기준으로 한다면 그다지 필요없을듯해보임

		//측정 시작
		gettimeofday(&startTime, NULL);
		fs = open_fs(fs, "/proc/stat");

		fgets(buf, BUFF_SIZE, fs);
		if (ferror(fs))
			err_by("stat parse error");

		for (int count = 0; count < 6; count++)
		{
			if (count == 1)
				cpu->usr = indx_get_num(buf, i);
			else if (count == 3)
				cpu->sys = indx_get_num(buf, i);
			else if (count == 4)
				cpu->idle = indx_get_num(buf, i);
			else if (count == 5)
				cpu->iowait = indx_get_num(buf, i);
			i = indx_go_next(buf, i);
		}
		//시험
		//아마 요부분에서 얼추 큐에 넣든 패킷에 바로넣든 결정이 될 듯싶다
		printf("usr = %d,sys = %d, idle = %d, iowait = %d\n",
		cpu->usr, cpu->sys, cpu->idle, cpu->iowait);

		fclose(fs);
		free(cpu);
		cpu = NULL;

		//측정 끝
		gettimeofday(&endTime, NULL);
//		diff_sec = (endTime.tv_sec - startTime.tv_sec);
    	diff_usec = (endTime.tv_usec - startTime.tv_usec) / (double)1000000;

		usleep ((1000 * 1000) - diff_usec);
	}//cpu while

	return ((void*)0);
}

void *pth_parse_mem(void *me)
{
	memUsage *mem = me;
	char buf[BUFF_SIZE];
	int i;
	FILE *fs = NULL;
	double diff_usec = 0;
	struct timeval startTime, endTime;
	
	while (1)
	{
		i = 0;
		diff_usec = 0;
		gettimeofday(&startTime, NULL);
		mem = (memUsage*)malloc(sizeof(memUsage));
		if (!mem)
			err_by("malloc_error");

		fs = open_fs(fs, "/proc/meminfo");
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
		}// mem parse roop	
		
		if (ferror(fs))
			err_by("mem parse error");

		printf("total = %d, used = %d, free = %d, swap_toal = %d, swap_used = %d\n",
				mem->total, mem->used, mem->free, mem->swap_total, mem->swap_used);
		
		fclose(fs);
		free(mem);
		mem = NULL;


		gettimeofday(&endTime, NULL);
    	diff_usec = (endTime.tv_usec - startTime.tv_usec) / (double)1000000;
		usleep ((1000 * 1000) - diff_usec);	
	}//mem roop

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
	int i;
	FILE *fs = NULL;
	double diff_usec = 0;
	struct timeval startTime, endTime;
	
	//원하고자 하는 내용은 3번째 줄에 있다
	while (1)
	{
		i = 0;
		diff_usec = 0;
		gettimeofday(&startTime, NULL);
		pack = (packUsage*)malloc(sizeof(packUsage));
		if (!pack)
			err_by("malloc_error");
		(pack)->inter = NULL;
		(pack)->next = NULL;

		fs = open_fs(fs, "/proc/net/dev");
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
			}
			else
				insert_packet(buf, pack);
		}
		
		fclose(fs);


		packUsage *tmp = pack;
		while (tmp && tmp->inter)
		{
			printf("interface = %s, in byte : %d, pac : %d, out byte : %d, pac : %d\n",
					tmp->inter, tmp->in_bytes, tmp->in_packets, tmp->out_bytes, tmp->out_packets);
			tmp = tmp->next;
		}
		//이거 이따가 좀 초기화 시켜야 할듯
		
		pack_free(&pack);

		gettimeofday(&endTime, NULL);
    	diff_usec = (endTime.tv_usec - startTime.tv_usec) / (double)1000000;
		usleep ((1000 * 1000) - diff_usec);	
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
	DIR *dir = NULL;
	double diff_usec = 0;
	struct timeval startTime, endTime;

	while (1)
	{
		diff_usec = 0;
	
		proc = (procInfo*)malloc(sizeof(procInfo));
		if (!proc)
			err_by("malloc_error");	
		proc->name = NULL;
		proc->next = NULL;
		gettimeofday(&startTime, NULL);

		dir = open_dir(dir, "/proc");
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
		closedir(dir);

		procInfo *tmp2 = proc;
		while (tmp2 && tmp2->name)
		{
			printf("name = %s, pid : %d, ppid : %d, cpu usage : %d, username %s, cmdline %s\n",
					tmp2->name, tmp2->pid, tmp2->ppid, tmp2->cpu_time, tmp2->user_name, tmp2->cmd_line);
			tmp2 = tmp2->next;
		}
		
		gettimeofday(&endTime, NULL);
    	diff_usec = (endTime.tv_usec - startTime.tv_usec) / (double)1000000;
		usleep ((1000 * 1000) - diff_usec);	

		proc_free(&proc);
	}//proc roop
	return ((void*)0);
}
