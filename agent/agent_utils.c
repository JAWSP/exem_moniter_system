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

void pack_new(packUsage **new)
{
	*new = (packUsage*)malloc(sizeof(packUsage));
	if (!(*new))
		err_by("new pack malloc_error");
	(*new)->inter[0] = '\0';
	(*new)->next = NULL;
}

void pack_free(packUsage **head)
{
    packUsage *del = NULL;
    packUsage *tmp = *head;
    while (tmp)
    {
        del = tmp;
        tmp = tmp->next;
		free(del->inter);
        free(del);
		del = NULL;
    }
	*head = NULL;
}

void proc_new(procInfo **new)
{
	*new = (procInfo*)malloc(sizeof(procInfo));
	if (!(*new))
		err_by("new pack malloc_error");
	(*new)->name[0] = '\0';
	(*new)->next = NULL;
}

void proc_free(procInfo **head)
{
    procInfo *del = NULL;
    procInfo *tmp = *head;
    while (tmp)
    {
        del = tmp;
        tmp = tmp->next;
		//인자들 free
		free(del->name);
		free(del->user_name);
		free(del->cmd_line);
        free(del);
		del = NULL;
    }
	*head = NULL;
}
