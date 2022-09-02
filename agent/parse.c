#include "object.h"
#include "packets.h"

/*
TODO
해당 스레드를 죽이지 않고 지속적으로 살려야 한다
싱크 -> 걸린시간의 차를 이용하여 특정 시간대에 맞게 설정해야 하고
그러면서 한번 수집이 끝났으면 각각 패킷에 넣으면 됨
근데 접속이 끊겨도 그대로 전달해주기 위한 큐(혹은 연결리스트)가 필요하게됨
->이말인 즉슨 바로 연결이 된다면 1초간격으로 넣은 얘들을 죄다 집어 넣게 만들게 되는건가보네
*/

void init_packet(packet **pack, header **head)
{
	*pack = (packet*)malloc(sizeof(packet));
	if (!*(pack))
		err_by("pack malloc_error");
	*head = (header*)malloc(sizeof(header));
	if (!*(head))
		err_by("head malloc_error");
}

header *insert_header(header *head, char type)
{
	sprintf(head->type_n_date, "%c %s", type, get_curr_time());
	if (type == 'c')
		head->count = 1;
	else if (type == 'm')
		head->count = 1;
	else if (type == 'n')
	{
		//얼추 fgets를 이용하여 갯수를 구하면 될 것 같음
	}
	else if (type == 'p')
	{
	}
	return (head);
}

void send_data(packet *pack, int sock)
{
	if (send(sock, pack->data, pack->len, 0) < 0)
		err_by("packet send error");
}

void *pth_parse_cpu(void *socks)
{
	char buf[BUFF_SIZE];
	FILE *fs = NULL;
	int i;
	double diff_usec = 0;
	struct timeval startTime, endTime;
	packet *pack_c = NULL;
	header *head_c = NULL;
	cpuUsage *cpu = NULL;
	int *sock = socks;


	while (1)
	{
		i = 0;
		diff_usec = 0;
		//먼저 초기화 할껀 초기화	
		cpu = (cpuUsage*)malloc(sizeof(cpuUsage));
		if (!cpu)
			err_by("malloc_error");
		init_packet(&pack_c, &head_c);
		head_c = insert_header(head_c, 'c');
		pack_c->len  = sizeof(header) + sizeof(cpuUsage);
		pack_c->data = (unsigned char *)malloc(pack_c->len);
		cpu = (cpuUsage *)(pack_c->data + sizeof(header));

		//루프 돌면서 측정 시작			
		gettimeofday(&startTime, NULL);
		fs = open_fs(fs, "/proc/stat");
		for (int loop = 0; loop < head_c->count; loop++)
		{


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
			}// collect loop;
		}//insert loop;
		 //
		printf("usr = %d,sys = %d, idle = %d, iowait = %d\n",
		cpu->usr, cpu->sys, cpu->idle, cpu->iowait);

		send_data(pack_c, *sock);
//		int a = 3;
//		if (send(*sock, &a, sizeof(int), 0) < 0)
//			err_by("cpu packet send error");

		fclose(fs);
		free(cpu);
		cpu = NULL;
		free(head_c);
		head_c = NULL;
		free(pack_c->data);
		pack_c->data = NULL;
		free(pack_c);
		pack_c = NULL;

		//측정 끝
		gettimeofday(&endTime, NULL);
    	diff_usec = (endTime.tv_usec - startTime.tv_usec) / (double)1000000;

		usleep ((1000 * 1000) - diff_usec);
	}//cpu while
	free(pack_c);

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
	strcpy(pack->inter, name);
	for (int count = 0; count < 9; count++)
		i = indx_go_next(buf, i);
	if (!sscanf(&buf[i], "%d %d", &pack->out_bytes, &pack->out_packets))
		err_by("tx packet sscanf error");
	return (pack);
}

void *pth_parse_packet(void *pac)
{
	packUsage *pack;
	char buf[BUFF_SIZE];
	int i;
	FILE *fs = NULL;
	double diff_usec = 0;
	struct timeval startTime, endTime;
	
	//원하고자 하는 내용은 3번째 줄에 있다
	//구조체를 생성하는 부분은 따로 뺄까?
	while (1)
	{
		i = 0;
		diff_usec = 0;
		gettimeofday(&startTime, NULL);
		pack_new(&pack);

		fs = open_fs(fs, "/proc/net/dev");
		fgets(buf, BUFF_SIZE, fs);
		fgets(buf, BUFF_SIZE, fs);
		while (fgets(buf, BUFF_SIZE, fs) != 0)
		{
			if (ferror(fs))
				err_by("net socket parse error");
			if (strlen(pack->inter))
			{
				packUsage *new;
				packUsage *tmp = pack;
				pack_new(&new);
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
		while (tmp)
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
			strcpy(proc->name ,name);
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
			strcpy(proc->user_name ,pwd.pw_name);
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
	fgets(buf, BUFF_SIZE, fs);
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
		proc->cmd_line[0] = '\0';
	else
		strcpy(proc->cmd_line, buf);

	fclose(fs);
	return (proc);
}

void *pth_parse_process(void *pro)
{
	procInfo *proc;
	struct dirent *buf = NULL;
	int pid = 0;
	DIR *dir = NULL;
	double diff_usec = 0;
	struct timeval startTime, endTime;

	while (1)
	{
		diff_usec = 0;
	
		proc_new(&proc);
		gettimeofday(&startTime, NULL);

		dir = open_dir(dir, "/proc");
		while ((buf = readdir(dir)) != NULL)
		{
			if ((pid = atoi(buf->d_name)) > 0)
			{
				if (strlen(proc->name))
				{
					procInfo *new;
					procInfo *tmp = proc;
					proc_new(&new);
					tmp->next = new;
					continue ;
				}
				insert_proc(pid, proc);
			}
		}
		closedir(dir);

		procInfo *tmp2 = proc;
		while (tmp2)
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
