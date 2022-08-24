#include "object.h"

void err_by(char *reason)
{
	perror(reason);
	exit(-1);
}

FILE *open_fs(FILE *fs, char *root)
{
	fs = fopen(root, "r");
	if (fs == NULL)
		err_by("fopen failed");

	return (fs);
}

DIR *open_dir(DIR *dir, char *root)
{
	dir = opendir(root);
	if (dir == NULL)
		err_by("diropen failed");

	return (dir);
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
