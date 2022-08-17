#include "object.h"

//빈칸을 넘기는 함수
int	indx_space(char *line, int i)
{
	while (line[i] == ' ' && line[i])
		i++;
	return (i);
}

//만약 int를 넘는 경우가 있을까?
//한 단어에 있는 숫자를 추출하는 함수
int	indx_get_num(char *line, int i)
{
	int res = 0;
	if (line[i] > '9' || line[i] < '0')
		return (-1);
	while (line[i] >= '0' && line[i] <= '9')
	{
		res = ((res * 10) + (line[i] - '0'));
		i++;
	}
	return (res);
}

//문자와 빈칸을 넘어 다음 문자로 넘기는 함수
int indx_go_next(char *line, int i)
{
	while (line[i] != ' ' && line[i])
		i++;
	i = indx_space(line, i);

	return (i);
}
