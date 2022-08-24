#include "object.h"

void err_by(char *reason)
{
	perror(reason);
	exit(-1);
}

FILE *read_cmd(FILE *fp, char *cmd)
{
	fp = popen(cmd, "r");
	if (fp == NULL)
		err_by("popen failed");

	return (fp);
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
