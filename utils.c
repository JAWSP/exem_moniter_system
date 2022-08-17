#include "object.h"

FILE *read_cmd(FILE *fp, char *cmd)
{
		char  buf[BUFF_SIZE];

	    fp = popen(cmd, "r");
        if (fp == NULL)
        {
            perror("popen failed");
			exit(-1);
        }

        while (fgets(buf, BUFF_SIZE, fp))
               printf("%s", buf);
		return (fp);
}
