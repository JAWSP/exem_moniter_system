#include "object.h"

int		indx_space(char *line, int i)
{
	while (line[i] == ' ')
	{
		i++;
		if (line[i] == 0)
			break ;
	}
	return (i);
}

int indx_getnum(char *line, int i)
{
	int num = atoi(line);
	return (i);
}
