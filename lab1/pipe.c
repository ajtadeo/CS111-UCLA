#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	// exit if there's an incorrect number of args
	if (argc <= 1){
		printf("Error %d: Incorrect number of variables\n", EINVAL);
		exit(EINVAL);
	}

	// set up pipe organization for all processes
	int *pipes[8]; // holds all pipes in the program
	
	// set up pipe and fork for the first and second processes
	int pipefd[2]; // pipefd_n[0] = read end, pipefd_n[1] = write end
	int pipe(pipefd);
	pipes[0] = &pipefd;

	// setup for first process
	int pid = fork();
	dup2(pipefd[1], STDOUT_FILENO);
	for (int i=1; i<argc; i++){
		switch(pid){
			case 0: 
				// child process
				printf("Child process: %d", pid);
				// organize pipe
				if (dup2(pipefd[1], STDIN_FILENO) == -1){
					printf("Error %d: dup2() failed.", errno);
					exit(errno);
				}
				close(pipefd[1]);
				close(pipefd[0]); // unused write
				// execute the command
				if (execlp(argv[1], argv[1], NULL) == -1){
					printf("Error %d: Command execution failed", errno);
					exit(errno);
				}
				exit(0);
				break;
			case -1:
				// error occurred
				printf("Error %d: Child creation unsuccessful", ECHILD);
				exit(ECHILD);
				break;
			default:
				// parent process
				printf("Parent process\n");
				// organize pipe
				if (dup2(pipefd[0], STDOUT_FILENO) == -1){
					printf("Error %d: dup2() failed.", errno);
					exit(errno);
				}
				close(pipefd[0]);
				close(pipefd[1]); // unused read
				// wait for child execution
				int status = 0;
				if (waitpid(pid, &status, 0) < 0){
					printf("Error %d: waitpid() failed.", errno);
					exit(errno);
				}
				if (!WIFEXITED(status) || WEXITSTATUS(status) != 0){
					exit(errno);
				}
				break;
		}
	}

	return 0;
