#include <unistd.h>
#include <stdio.h>
#include "dowrite.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/types.h>
#include <string.h>

void write_file(int fd, const char *infile) {
        char  buff[1024];
        size_t len;
        ssize_t rcnt=1;
        ssize_t fd1=open(infile, O_RDONLY);
        for (;;){

                rcnt = read(fd1,buff,sizeof(buff)-1);
                if (rcnt==0) return ;
                if (rcnt==-1)           {
                        perror("read");
                        return ;  }
                buff[rcnt]='\0';
                len = strlen(buff);
                doWrite(fd,buff,len);

        }

        close(fd1);
}


