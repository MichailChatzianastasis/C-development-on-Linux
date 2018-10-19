#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

#include <sys/wait.h>
#include <sys/types.h>

#include "proc-common.h"
#include "request.h"

/* Compile-time parameters. */
#define SCHED_TQ_SEC 5                /* time quantum */
#define TASK_NAME_SZ 60               /* maximum size for a task's name */
#define SHELL_EXECUTABLE_NAME "shell" /* executable for shell */

int dead_children=0;
int nproc,hashigh=0;

struct proc_control_bl {
	int id;
	pid_t pid;
	char *onoma;
	char prio[5];
};

struct node_list {
	struct proc_control_bl dierg;
	struct node_list* next;
	struct node_list* prev;
} *head;

typedef struct node_list node_list;
node_list* curr;
node_list* temp;
node_list* start;
node_list* temp2;

/* Print a list of all tasks currently being scheduled.  */
static void
sched_print_tasks(void)
{
	node_list* temp2;
	temp2=curr;
	do {
 		printf("Proccess with id: %d, PID:%d, name:%s, priority:%s\n",
		(temp2->dierg).id,(temp2->dierg).pid,(temp2->dierg).onoma,(temp2->dierg).prio);
		temp2=temp2->next;
	} while(temp2!=curr);

 }

/* Send SIGKILL to a task determined by the value of its
 * scheduler-specific id.
 */
static int
sched_kill_task_by_id(int id)
{
 	temp2=curr;
 		do {
			if((temp2->dierg).id==id) {
				kill((temp2->dierg).pid,SIGKILL);
				break;
			}
			temp2=temp2->next;
	} while(temp2!=curr);

	return 0;
}

/* 	High-prioritize to a task determined by the value
	of its scehduler-specific id */
static int
sched_high_task(int id) {
	temp2=curr;
		do {
		if((temp2->dierg).id==id) {
			if(strcmp((temp2->dierg).prio,"HIGH")==0)
				printf("The proccess has already HIGH priority\n");
			else {
				strcpy((temp2->dierg).prio,"HIGH");
				hashigh++; }
			break;
		}
		temp2=temp2->next;
} while(temp2!=curr);
	return 0;
}

/* 	Low-prioritize to a task determined by the value
	of its scehduler-specific id */
static int
sched_low_task(int id) {
	temp2=curr;
		do {
		if((temp2->dierg).id==id) {
			if(strcmp((temp2->dierg).prio,"LOW")==0)
				printf("The proccess has already LOW priority\n");
			else {
				strcpy((temp2->dierg).prio,"LOW");
				hashigh--; }
			break;
		}
		temp2=temp2->next;
	} while(temp2!=curr);
	return 0;
	}


/* Create a new task.  */
static void
sched_create_task(char *executable)
{ pid_t p;
	p=fork();
	if(p<0) { printf("error in fork\n");}
	else if(p==0) {
		char *newargv[] = { executable, NULL, NULL, NULL };
		char *newenviron[] = { NULL };
		printf("Shell about to create a new task\n");
		raise(SIGSTOP);
		printf("About to replace myself with the executable %s...\n",
			executable);
		execve(executable, newargv, newenviron);

		/* execve() only returns on error */
		perror("execve");
		exit(1);

	}
	struct proc_control_bl cbl;
 	temp2=(node_list*) malloc(sizeof(node_list));
	cbl.id=nproc+1;
	cbl.pid=p;
	cbl.onoma=malloc(strlen(executable) +1);
	strcpy(cbl.prio,"LOW");
	strcpy(cbl.onoma,executable);
	temp2->dierg=cbl;
	temp2->next=curr->next;
	temp2->prev=curr;
	curr->next=temp2;
	temp2->next->prev=temp2;
	nproc++;
}

/* Process requests by the shell.  */
static int
process_request(struct request_struct *rq)
{
	switch (rq->request_no) {
		case REQ_PRINT_TASKS:
			sched_print_tasks();
			return 0;

		case REQ_KILL_TASK:
			return sched_kill_task_by_id(rq->task_arg);

		case REQ_EXEC_TASK:
			sched_create_task(rq->exec_task_arg);
			return 0;

		case REQ_HIGH_TASK:
			return sched_high_task(rq->task_arg);

		case REQ_LOW_TASK:
			return sched_low_task(rq->task_arg);

		default:
			return -ENOSYS;
	}
}

/*
 * SIGALRM handler
 */
static void
sigalrm_handler(int signum)
{
	if (signum != SIGALRM) {
 		fprintf(stderr, "Internal error: Called for signum %d, not SIGALRM\n",
 			signum);
 		exit(1);
 	}

	printf("ALARM! %d seconds have passed.\n", SCHED_TQ_SEC);
	//SIGSTOP CURRENT PROCCESS
 	kill((curr->dierg).pid,SIGSTOP);
			/* Setup the alarm again */

	}


/*
 * SIGCHLD handler
 */
static void
sigchld_handler(int signum)
{
	pid_t p;
	int status;

	if (signum != SIGCHLD) {
		fprintf(stderr, "Internal error: Called for signum %d, not SIGCHLD\n",
			signum);
		exit(1);
	}
	if (alarm(SCHED_TQ_SEC) < 0) {
			perror("alarm");
			exit(1);
		}
	/*
	 * Something has happened to one of the children.
	 * We use waitpid() with the WUNTRACED flag, instead of wait(), because
	 * SIGCHLD may have been received for a stopped, not dead child.
	 *
	 * A single SIGCHLD may be received if many processes die at the same time.
	 * We use waitpid() with the WNOHANG flag in a loop, to make sure all
	 * children are taken care of before leaving the handler.
	 */

	for (;;) {
 		p = waitpid(-1, &status, WUNTRACED | WNOHANG);
		if (p < 0) {
			perror("waitpid");
			exit(1);
		}
		if (p == 0)
			break;

		 explain_wait_status(p, status);
		 node_list *temp4;
		 temp4=curr->next;
		if (WIFEXITED(status) || WIFSIGNALED(status)) {
			/* A child has died */
			// find the proccess that terminated
			node_list *temp3;
			temp3=curr;
			do {
				if((temp3->dierg).pid==p)
				break;
				temp3=temp3->next;
			} while(temp3!=curr); //we have in temp3 the proccess that terminated
			if(strcmp((temp3->dierg).prio,"HIGH")==0)
				hashigh--;
			temp3->prev->next=temp3->next;
			temp3->next->prev=temp3->prev;
		 	if(hashigh==0) {
				curr=curr->next;
				free(temp3);
				kill((curr->dierg).pid,SIGCONT);
			}
			else {
					do {
						if(strcmp((temp4->dierg).prio,"HIGH")==0)
							{
 								kill((temp4->dierg).pid,SIGCONT);
								curr=temp4;
								break;
							}
						temp4=temp4->next;
					} while(temp4!=curr->next);
					free(temp3);
			}
			// afairesh ths diergasias apo thn lista
			if(++dead_children==nproc+1) {
			printf("Parent: All children are dead...  Exiting\n");
				exit(0); }

		}
		if (WIFSTOPPED(status)) {
 			/* A child has stopped due to SIGSTOP/SIGTSTP, etc... */
			if(hashigh==0) {
				kill((curr->next->dierg).pid,SIGCONT);
				curr=curr->next;
			}
			else
			{
				do {
					if(strcmp((temp4->dierg).prio,"HIGH")==0)
						{
							kill((temp4->dierg).pid,SIGCONT);
							curr=temp4;
							break;
						}
						temp4=temp4->next;
					} while(temp4!=curr->next);
 		 }
		}
			printf("Parent: Child has been stopped. Moving right along...\n");
		}
	}


/* Disable delivery of SIGALRM and SIGCHLD. */
static void
signals_disable(void)
{
	sigset_t sigset;

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigaddset(&sigset, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &sigset, NULL) < 0) {
		perror("signals_disable: sigprocmask");
		exit(1);
	}
}

/* Enable delivery of SIGALRM and SIGCHLD.  */
static void
signals_enable(void)
{
	sigset_t sigset;

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	sigaddset(&sigset, SIGCHLD);
	if (sigprocmask(SIG_UNBLOCK, &sigset, NULL) < 0) {
		perror("signals_enable: sigprocmask");
		exit(1);
	}
}


/* Install two signal handlers.
 * One for SIGCHLD, one for SIGALRM.
 * Make sure both signals are masked when one of them is running.
 */
static void
install_signal_handlers(void)
{
	sigset_t sigset;
	struct sigaction sa;

	sa.sa_handler = sigchld_handler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGCHLD);
	sigaddset(&sigset, SIGALRM);
	sa.sa_mask = sigset;
	if (sigaction(SIGCHLD, &sa, NULL) < 0) {
		perror("sigaction: sigchld");
		exit(1);
	}

	sa.sa_handler = sigalrm_handler;
	if (sigaction(SIGALRM, &sa, NULL) < 0) {
		perror("sigaction: sigalrm");
		exit(1);
	}

	/*
	 * Ignore SIGPIPE, so that write()s to pipes
	 * with no reader do not result in us being killed,
	 * and write() returns EPIPE instead.
	 */
	if (signal(SIGPIPE, SIG_IGN) < 0) {
		perror("signal: sigpipe");
		exit(1);
	}
}

static void
do_shell(char *executable, int wfd, int rfd)
{
	char arg1[10], arg2[10];
	char *newargv[] = { executable, NULL, NULL, NULL };
	char *newenviron[] = { NULL };

	sprintf(arg1, "%05d", wfd);
	sprintf(arg2, "%05d", rfd);
	newargv[1] = arg1;
	newargv[2] = arg2;

	raise(SIGSTOP);
	execve(executable, newargv, newenviron);

	/* execve() only returns on error */
	perror("scheduler: child: execve");
	exit(1);
}

/* Create a new shell task.
 *
 * The shell gets special treatment:
 * two pipes are created for communication and passed
 * as command-line arguments to the executable.
 */
static void
sched_create_shell(char *executable, int *request_fd, int *return_fd)
{
	pid_t p;
	int pfds_rq[2], pfds_ret[2];

	if (pipe(pfds_rq) < 0 || pipe(pfds_ret) < 0) {
		perror("pipe");
		exit(1);
	}

	p = fork();
	if (p < 0) {
		perror("scheduler: fork");
		exit(1);
	}

	if (p == 0) {
		/* Child */
		close(pfds_rq[0]);
		close(pfds_ret[1]);
		do_shell(executable, pfds_rq[1], pfds_ret[0]);
		assert(0);
	}
 	struct proc_control_bl cblo;
	head=(node_list*) malloc(sizeof(node_list));
	cblo.id=0;
	cblo.pid=p;
	cblo.onoma=malloc(strlen("executable")+1);
	strcpy(cblo.onoma,executable);
	strcpy(cblo.prio,"LOW");
	head->dierg=cblo;
	/* Parent */
	close(pfds_rq[1]);
	close(pfds_ret[0]);
	*request_fd = pfds_rq[0];
	*return_fd = pfds_ret[1];
}

static void
shell_request_loop(int request_fd, int return_fd)
{
	int ret;
	struct request_struct rq;

	/*
	 * Keep receiving requests from the shell.
	 */
	for (;;) {
		if (read(request_fd, &rq, sizeof(rq)) != sizeof(rq)) {
			perror("scheduler: read from shell");
			fprintf(stderr, "Scheduler: giving up on shell request processing.\n");
			break;
		}

		signals_disable();
		ret = process_request(&rq);
		signals_enable();

		if (write(return_fd, &ret, sizeof(ret)) != sizeof(ret)) {
			perror("scheduler: write to shell");
			fprintf(stderr, "Scheduler: giving up on shell request processing.\n");
			break;
		}
	}
}

int main(int argc, char *argv[])
{

	/* Two file descriptors for communication with the shell */
	static int request_fd, return_fd;

	/* Create the shell. */
	sched_create_shell(SHELL_EXECUTABLE_NAME, &request_fd, &return_fd);
	/* TODO: add the shell to the scheduler's tasks */

	/*
	 * For each of argv[1] to argv[argc - 1],
	 * create a new child process, add it to the process list.
	 */

 	char *executable=NULL;
	nproc = argc-1;
	pid_t pids[nproc];

	int i; /* number of proccesses goes here */
	for(i=0;i<nproc;i++) {
			pids[i]=fork();
			if(pids[i]<0) { printf("error "); exit(1); }
			if(pids[i]==0) {
					//child
					executable = argv[i+1];
					char *newargv[] = { executable, NULL, NULL, NULL };
					char *newenviron[] = { NULL };
					printf("I am %s, PID = %ld\n",
						argv[0], (long)getpid());
					raise(SIGSTOP);
					printf("About to replace myself with the executable %s...\n",
						executable);
					execve(executable, newargv, newenviron);

					/* execve() only returns on error */
					perror("execve");
					exit(1);
				}
	}
	wait_for_ready_children(nproc+1);

	/* Wait for all children to raise SIGSTOP before exec()ing. */
	struct proc_control_bl cbl;
	curr=head;
	start=head;
	node_list* tail;
	for(i=0;i<nproc;i++)
	{
		tail=(node_list*) malloc(sizeof(node_list));
		head->next=tail;
		tail->prev=head;
		head=tail;
		cbl.id=i+1;
		cbl.pid=pids[i];
		cbl.onoma=malloc(strlen(argv[i+1])+1);
		strcpy(cbl.onoma,argv[i+1]);
		strcpy(cbl.prio,"LOW");
		head->dierg=cbl;
	}
	// cycled list
	head->next=curr;
	curr->prev=head;

	/* Install SIGALRM and SIGCHLD handlers. */
	install_signal_handlers();
 	kill((curr->dierg).pid,SIGCONT);
	if (nproc == 0) {
		fprintf(stderr, "Scheduler: No tasks. Exiting...\n");
		exit(1);
	}

	 shell_request_loop(request_fd, return_fd);

	/* Now that the shell is gone, just loop forever
	 * until we exit from inside a signal handler.
	 */
	while (pause())
		;

	/* Unreachable */
	fprintf(stderr, "Internal error: Reached unreachable point\n");
	return 1;
}
