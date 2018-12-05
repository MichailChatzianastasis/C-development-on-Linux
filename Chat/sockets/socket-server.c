/*
 * socket-server.c
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

#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "socket-common.h"

#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2
int sd, newsd=-1;
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




/* Convert a buffer to upercase */
void toupper_buf(char *buf, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		buf[i] = toupper(buf[i]);
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

void intHandler(int signal_to_be_handled){
  printf("\nWelcome to signal handler,exited with SIGINT\n");
  if (newsd != -1){
    shutdown(newsd,SHUT_WR);
    close(newsd);
  }
  shutdown(sd,SHUT_WR);
  close(sd);
  exit(0);
}

int main(void)
{
  signal(SIGINT,intHandler);
	fd_set rfds;
	FD_ZERO(&rfds);
	char buf[100];
	char addrstr[INET_ADDRSTRLEN];
	int sel_ret;
	ssize_t n;
	socklen_t len;
	struct sockaddr_in sa;

	/* Make sure a broken connection doesn't kill us */
	signal(SIGPIPE, SIG_IGN);

	/* Create TCP/IP socket, used as main chat channel */
	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	fprintf(stderr, "Created TCP socket\n");

	/* Bind to a well-known port */
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(TCP_PORT);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("bind");
		exit(1);
	}
	fprintf(stderr, "Bound TCP socket to port %d\n", TCP_PORT);

	/* Listen for incoming connections */
	if (listen(sd, TCP_BACKLOG) < 0) {
		perror("listen");
		exit(1);
	}

	/* Loop forever, accept()ing connections */
	for (;;) {
		fprintf(stderr, "Waiting for an incoming connection...\n");

		/* Accept an incoming connection */
		len = sizeof(struct sockaddr_in);
		if ((newsd = accept(sd, (struct sockaddr *)&sa, &len)) < 0) {
			perror("accept");
			exit(1);
		}
		if (!inet_ntop(AF_INET, &sa.sin_addr, addrstr, sizeof(addrstr))) {
			perror("could not format IP address");
			exit(1);
		}
		fprintf(stderr, "Incoming connection from %s:%d\n",

			addrstr, ntohs(sa.sin_port));

		/* We break out of the loop when the remote peer goes away */
		for (;;) {
			FD_SET(0,&rfds);
			FD_SET(newsd,&rfds);
			fflush(stdin);
			sel_ret = select(newsd + 1,&rfds,NULL,NULL,NULL);
			if(sel_ret == -1 ) perror("select error");
			if(FD_ISSET(newsd,&rfds) ) {
  			n = read(newsd, buf, sizeof(buf));
  			if (n <= 0) {
  				if (n < 0)
  					perror("read from remote peer failed");
  				else{
  					fprintf(stderr, "Peer went away\n");
            /* Make sure we don't leak open files */
            if (close(newsd) < 0)
            perror("close");
  				  break;
        }
  			}
  			fprintf(stdout, "Remote says:\n");
  			write(1,buf,n);
  			printf("\n");
    }
			else if (FD_ISSET(0,&rfds)){

  			// ALLIOS TREXE GETLINE
  			getLine(0,buf,sizeof(buf));
  			printf("\033[A\33[2K\r");
  			//toupper_buf(buf, n);
  			buf[sizeof(buf) - 1] = '\0';

  			if (insist_write(newsd, buf, strlen(buf)) != strlen(buf)) {
  				perror("write to remote peer failed");
  				break;
  			}
  			fprintf(stdout, "I said:\n%s\n", buf);
  			fflush(stdout);
  			fflush(stdin);

		}
	}
	}

	/* This will never happen */
	return 1;
}
