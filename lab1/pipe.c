#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	// exit if there's an incorrect number of args
	if (argc <= 1){
		printf("Error: Incorrect number of variables\n");
		exit(EINVAL);
	}
	// first set up the pipe file descriptors for the parent process
	// pipefd[0] = read end
	// pipefd[1] = write end
	// int pipefd[2];
	// int pipe(pipefd);

	// fork to call the first pipe command
	int pid = fork();
	if (pid>0){
		printf("Parent process\n");
		int status = 0;
		waitpid(pid, &status, 0);
	} else if (pid==0){
		printf("Child process: %d", pid);
		execlp(argv[1], argv[1], NULL);
		exit(0);
	} else {
		exit(ECHILD);
	}

	return 0;
}
