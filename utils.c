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

FILE *open_fs(FILE *fs, char *root)
{
	//	char  buf[BUFF_SIZE];

	    fs = fopen(root, "r");
        if (fs == NULL)
            err_by("fopen failed");

      //  while (fgets(buf, BUFF_SIZE, fp))
        //    printf("%s", buf);
		return (fs);
}

DIR *open_dir(DIR *dir, char *root)
{
	    dir = opendir(root);
        if (dir == NULL)
            err_by("diropen failed");

		return (dir);
}
