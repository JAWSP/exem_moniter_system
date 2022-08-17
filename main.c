#include "object.h"

int main(void)
{
        char  buff[BUFF_SIZE];
        FILE *fp;
 
        fp = popen("cat /proc/stat", "r");
        if (NULL == fp)
        {
               perror("popen failed");
               return (-1);
        }
 
        while (fgets(buff, BUFF_SIZE, fp))
               printf("%s", buff);
 
        pclose(fp);
        return (0);
}
