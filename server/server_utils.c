#include "server.h"
#include "server_queue.h"
#include <sys/time.h>

extern g_serv *gs;

int certification(void)
{
	int res = 0;
	char buf[12];

	if ((res = recv(gs->agent_fd, buf, 8, 0)) < 0)
		err_by("certi recv failed");
		//return (0);
	if (res != 8)
		return (-1);
	buf[8] = 0;
	//키값 확인
	for (int i = 0; i < 9; i++)
	{
		if (strcmp(gs->keys[i], buf) == 0)
		{
			if ((res = send(gs->agent_fd, "OK", 2, 0)) < 0)
				err_by("certi OK send failed");
			//TODO 나중에 재접속했다고 로그를 남겨야함
			return (i);
		}
	}

	for (int i = 0; i < 9; i++)
	{
		if (strcmp(gs->ids[i], buf) == 0)
		{
			if ((res = send(gs->agent_fd, gs->keys[i], 8, 0)) < 0)
				err_by("certi hash(?) send failed");
			//여기서 키를 부여하고 send
			//그리고 다시 받고 티키타카 시작
			return (i);
		}
	}

	return (-2);
}

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

char *get_curr_day(void)
{
	time_t curr;
	struct tm *t;
	static char res[11];

	curr = time(NULL);
	t = localtime(&curr);

	sprintf(res, "%d-%d-%d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);

	return (res);
}
