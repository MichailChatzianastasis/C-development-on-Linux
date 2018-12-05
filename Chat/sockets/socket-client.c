/*
 * socket-client.c
 * Simple TCP/IP communication using sockets
 *
 * Vangelis Koukis <vkoukis@cslab.ece.ntua.gr>
 */

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "socket-common.h"


#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2
static int getLine (char *prmpt, char *buff, size_t sz) {
    int ch, extra;

    // Get line with buffer overrun protection.
    if (prmpt != NULL) {
        printf ("%s", prmpt);
        fflush (stdout);
    }
    if (fgets (buff, sz, stdin) == NULL)
        return NO_INPUT;

    // If it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buff[strlen(buff)-1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        return (extra == 1) ? TOO_LONG : OK;
    }

    // Otherwise remove newline and give string back to caller.
    buff[strlen(buff)-1] = '\0';
    return OK;
}





/* Insist until all of the data has been written */
ssize_t insist_write(int fd, const void *buf, size_t cnt)
{
	ssize_t ret;
	size_t orig_cnt = cnt;

	while (cnt > 0) {
	        ret = write(fd, buf, cnt);
	        if (ret < 0)
	                return ret;
	        buf += ret;
	        cnt -= ret;
	}

	return orig_cnt;
}

int main(int argc, char *argv[])
{
	fd_set rfds;
	FD_ZERO(&rfds);
	int sd, port;
	ssize_t n;
	char buf[100];
	char *hostname;
	struct hostent *hp;
	struct sockaddr_in sa;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
		exit(1);
	}
	hostname = argv[1];
	port = atoi(argv[2]); /* Needs better error checking */

	/* Create TCP/IP socket, used as main chat channel */
	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	fprintf(stderr, "Created TCP socket\n");

	/* Look up remote hostname on DNS */
	if ( !(hp = gethostbyname(hostname))) {
		printf("DNS lookup failed for host %s\n", hostname);
		exit(1);
	}

	/* Connect to remote TCP port */
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	memcpy(&sa.sin_addr.s_addr, hp->h_addr, sizeof(struct in_addr));
	fprintf(stderr, "Connecting to remote host... "); fflush(stderr);
	if (connect(sd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		perror("connect");
		exit(1);
	}
	fprintf(stderr, "Connected.\n");

	/* Be careful with buffer overruns, ensure NUL-termination */
	int sel_ret;
	while(1) {
		FD_SET(0,&rfds);
		FD_SET(sd,&rfds);
		fflush(stdin);
		sel_ret = select(sd + 1,&rfds,NULL,NULL,NULL);
		if(sel_ret == -1 ) perror("select error");
		if(FD_ISSET(0,&rfds) ) {


			getLine(0,buf,sizeof(buf));
			printf("\033[A\33[2K\r");
		 	//strncpy(buf, HELLO_THERE, sizeof(buf));
			buf[sizeof(buf) - 1] = '\0';

			/* Say something... */
			if (insist_write(sd, buf, strlen(buf)) != strlen(buf)) {
				perror("write");
				exit(1);
			}
			fprintf(stdout, "I said:\n%s\nRemote says:\n", buf);
			fflush(stdout);
		}

			/*
			 * Let the remote know we're not going to write anything else.
			 * Try removing the shutdown() call and see what happens.
			 */

			/* Read answer and write it to standard output */
			if(FD_ISSET(sd,&rfds) ) {

				n = read(sd, buf, sizeof(buf));

				if (n < 0) {
					perror("read");
					exit(1);
				}

				if (n <= 0)
					break;

				if (insist_write(0, buf, n) != n) {
					perror("write");
					exit(1);
				}
		}
printf("\n");
}
if (shutdown(sd, SHUT_WR) < 0) {
	perror("shutdown");
	exit(1);
}

	fprintf(stderr, "\nDone.\n");
	return 0;
}
