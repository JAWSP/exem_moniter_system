#include "object.h"
#include "packets.h"
#include <time.h>

char *get_curr_time(void)
{
	time_t curr;
	struct tm *t;
	static char res[20];

	curr = time(NULL);
	t = localtime(&curr);

	sprintf(res, "%d-%d-%d %d:%d:%d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	return (res);
}

packet *init_packet(packet *pack, char type, int count)
{
	pack = (packet*)malloc(sizeof(packet));
	if (!pack)
		err_by("pack malloc_error");
	if (type == 'c')
	{
		pack->len  = sizeof(header) + sizeof(cpuUsage) + 1;
		pack->data = (unsigned char *)malloc(pack->len);
		pack->data[pack->len - 1] = 0;
	}
	else if (type == 'm')
	{
		pack->len  = sizeof(header) + sizeof(memUsage) + 1;
		pack->data = (unsigned char *)malloc(pack->len);
		pack->data[pack->len - 1] = 0;
	}
	else if (type == 'n')
	{
		pack->len  = sizeof(header) + (sizeof(packUsage) * count) + 1;
		pack->data = (unsigned char *)malloc(pack->len);
		pack->data[pack->len - 1] = 0;
	}
	else if (type == 'p')
	{
		pack->len  = sizeof(header) + (sizeof(procInfo) * count) + 1;
		pack->data = (unsigned char *)malloc(pack->len);
		pack->data[pack->len - 1] = 0;
	}
	return (pack);
}

header *insert_header(header *head, char type)
{
	sprintf(head->type_n_date, "%c %s", type, get_curr_time());
	if (type == 'c')
		head->count = 1;
	else if (type == 'm')
		head->count = 1;
	return (head);
}

int get_count(char type, char *cmd)
{
	int res = 0;

	if (type == 'n')
	{
		FILE *fs = NULL;
		char buf[BUFF_SIZE];

		fs = open_fs(fs, cmd);
		while (fgets(buf, BUFF_SIZE, fs) != 0)
			res++;
		res -= 2;
		fclose(fs);
	}
	else if (type == 'p')
	{
		DIR *dir = NULL;
		struct dirent *buf = NULL;

		dir = open_dir(dir, "/proc");
		while ((buf = readdir(dir)) != NULL)
		{
			if (atoi(buf->d_name) > 0)
				res++;
		}
		closedir(dir);
	}
	return (res);
}
