#include "object.h"

int main(void)
{
        FILE *fp = NULL;
 
        fp = read_cmd(fp, "cat /proc/stat");
        fp = read_cmd(fp, "free");
        fp = read_cmd(fp, "cat /proc/net/dev");
        pclose(fp);

        return (0);
}
