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

int indx_go_next(char *line, int i)
{
	while (line[i] != ' ' && line[i])
		i++;
	i = indx_space(&line[i], i);

	return (i);
}
