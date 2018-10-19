#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "write_file.h"
#include "dowrite.h"


int main(int argc, char **argv){
        int oflags=O_CREAT |O_RDWR | O_TRUNC | O_APPEND ;
        int mode= S_IRUSR | S_IWUSR;
        if(argc<3) {
                printf("You should give 2 input files");
                return 0;}
        if(argc==3) argv[3]="fconc.out";
        ssize_t fd3=open(argv[3],oflags,mode);
        write_file(fd3,argv[1]);
        write_file(fd3,argv[2]);
        close(fd3);
        return 0;
        }
