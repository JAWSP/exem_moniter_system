#include "object.h"

void err_by(char *reason)
{
	perror(reason);
	exit(-1);
}

FILE *read_cmd(FILE *fp, char *cmd)
{
	//	char  buf[BUFF_SIZE];

	    fp = popen(cmd, "r");
        if (fp == NULL)
            err_by("popen failed");

      //  while (fgets(buf, BUFF_SIZE, fp))
        //    printf("%s", buf);
		return (fp);
}
