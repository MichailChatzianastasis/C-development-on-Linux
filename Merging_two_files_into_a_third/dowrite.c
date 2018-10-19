#include <unistd.h>
#include <string.h>
#include <stdio.h>

void doWrite(int fd, const char *buff , int len) {
        ssize_t wcnt;
        size_t idx=0;


        do {
                wcnt = write(fd,buff + idx, len - idx);
                if (wcnt == -1){ /* error */
                        perror("write");
                        return ;
                }
                idx += wcnt;
        } while (idx < len);

}
                   
