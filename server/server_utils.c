#include "server.h"
#include "server_queue.h"
#include <sys/time.h>

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