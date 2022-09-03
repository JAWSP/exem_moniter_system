#include "object.h"
#include "packets.h"

void send_data(packet *pack, int sock)
{
	if (send(sock, pack->data, pack->len, 0) < 0)
		err_by("packet send error");
}

void *pth_parse_cpu(void *socket)
{
	char buf[BUFF_SIZE];
	FILE *fs = NULL;
	int i;
	double diff_usec = 0;
	struct timeval startTime, endTime;
	cpuUsage *cpu;
	int *sock = socket;


	while (1)
	{
		packet *packet;
		header *head;

		diff_usec = 0;
		//먼저 초기화 할껀 초기화
		//+1하는 이유는 맨 마지막 null값 넣을려고
		packet = init_packet(packet, 'c', 1);
		//이렇게 형변환을 시키면 알아서 시리얼라이즈가 된다고 한다
		head = (header *)packet->data;
		head = insert_header(head, 'c');
		cpu = (cpuUsage *)(packet->data + sizeof(header));

		//루프 돌면서 측정 시작			
		gettimeofday(&startTime, NULL);
		fs = open_fs(fs, "/proc/stat");
		for (int loop = 0; loop < head->count; loop++)
		{
			fgets(buf, BUFF_SIZE, fs);
			if (ferror(fs))
				err_by("stat parse error");
			i = 0;
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
		printf("usr = %d,sys = %d, idle = %d, iowait = %d\n",
		cpu->usr, cpu->sys, cpu->idle, cpu->iowait);
		/*
		 * sending time
		 */
		send_data(packet, *sock);

		fclose(fs);
		free(packet->data);
		packet->data = NULL;
		free(packet);
		packet = NULL;

		//측정 끝
		gettimeofday(&endTime, NULL);
    	diff_usec = (endTime.tv_usec - startTime.tv_usec) / (double)1000000;

		usleep ((1000 * 1000) - diff_usec);
	}//cpu while

	return ((void*)0);
}

void *pth_parse_mem(void *socket)
{
	memUsage *mem;
	int *sock = (int *)socket;
	char buf[BUFF_SIZE];
	int i;
	FILE *fs = NULL;
	double diff_usec = 0;
	struct timeval startTime, endTime;
	
	while (1)
	{
		packet *packet;
		header *head;

		packet = init_packet(packet, 'm', 1);
		//이렇게 형변환을 시키면 알아서 시리얼라이즈가 된다고 한다
		head = (header *)packet->data;
		head = insert_header(head, 'm');
		mem = (memUsage *)(packet->data + sizeof(header));

		diff_usec = 0;
		gettimeofday(&startTime, NULL);
		if (!mem)
			err_by("malloc_error");

		fs = open_fs(fs, "/proc/meminfo");
		for (int loop = 0; loop < head->count; loop++)
		{
			i = 0;
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
			}// mem parse loop
			if (ferror(fs))
				err_by("mem parse error");
		}//mem insert loop
		


		printf("total = %d, used = %d, free = %d, swap_toal = %d, swap_used = %d\n",
				mem->total, mem->used, mem->free, mem->swap_total, mem->swap_used);
		send_data(packet, *sock);
		
		fclose(fs);
		free(packet->data);
		packet->data = NULL;
		free(packet);
		packet = NULL;

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

void *pth_parse_packet(void *socket)
{
	packUsage *np;
	int *sock = (int *)socket;
	char buf[BUFF_SIZE];
	FILE *fs = NULL;
	double diff_usec;
	struct timeval startTime, endTime;
	
	//원하고자 하는 내용은 3번째 줄에 있다
	//구조체를 생성하는 부분은 따로 뺄까?
	//TODO
	//패킷의 길이를 할당하기 위해선 저 패킷이 몇개나 있는지 봐야하고
	//연결리스트가 아닌 연속적으로 버퍼에 저장시켜야 함
	//
	while (1)
	{
		packet *packet;
		header *head;
		int count = 0;
		count = get_count('n', "/proc/net/dev");
		init_packet(packet, 'n', count);
		
		head = (header *)packet->data;
		head = insert_header(head, 'n');
		head->count = count;
		np = (packUsage *)(packet->data + sizeof(header));

		diff_usec = 0;
		gettimeofday(&startTime, NULL);

		fs = open_fs(fs, "/proc/net/dev");
		fgets(buf, BUFF_SIZE, fs);
		fgets(buf, BUFF_SIZE, fs);
		for (int loop = 0; loop < count; loop++)
		{
			while (fgets(buf, BUFF_SIZE, fs) != 0)
			{
				if (ferror(fs))
					err_by("net socket parse error");
				insert_packet(buf, np);
				np++;
			}
		}
		
		fclose(fs);
		unsigned char *bb = packet->data;
		packUsage *tmp = (packUsage *)(bb + sizeof(header));
		for (int loop = 0; loop < count; loop++)
		{
			printf("interface = %s, in byte : %d, pac : %d, out byte : %d, pac : %d\n",
					tmp->inter, tmp->in_bytes, tmp->in_packets, tmp->out_bytes, tmp->out_packets);
			tmp++;
		}
		send_data(packet, *sock);
		//이거 이따가 좀 초기화 시켜야 할듯
		
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

void *pth_parse_process(void *socket)
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
			//else
			//	대충 카운트 하나씩 깎는 함수
		}
		closedir(dir);

		/*
		procInfo *tmp2 = proc;
		while (tmp2)
		{
			printf("name = %s, pid : %d, ppid : %d, cpu usage : %d, username %s, cmdline %s\n",
					tmp2->name, tmp2->pid, tmp2->ppid, tmp2->cpu_time, tmp2->user_name, tmp2->cmd_line);
			tmp2 = tmp2->next;
		}
		*/
		
		gettimeofday(&endTime, NULL);
    	diff_usec = (endTime.tv_usec - startTime.tv_usec) / (double)1000000;
		usleep ((1000 * 1000) - diff_usec);	

		proc_free(&proc);
	}//proc roop
	return ((void*)0);
}
