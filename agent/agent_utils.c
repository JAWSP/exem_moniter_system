#include "object.h"
#include "packets.h"

void pack_free(packUsage **head)
{
    packUsage *del = NULL;
    packUsage *tmp = *head;
    while (tmp)
    {
        del = tmp;
        tmp = tmp->next;
		free(del->inter);
		del->inter = NULL;
        free(del);
		del = NULL;
    }
	*head = NULL;
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
		del->name = NULL;
		free(del->user_name);
		del->user_name = NULL;
		free(del->cmd_line);
		del->cmd_line = NULL;
        free(del);
		del = NULL;
    }
	*head = NULL;
}
