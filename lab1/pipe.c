#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define ONLY_CMD 0
#define FIRST_CMD 1
#define MIDDLE_CMD 2
#define LAST_CMD 3

void safeDup2(int dest, int src){
	if (dup2(dest, src) == -1){
		printf("Error %d: dup2() failed.\n", errno);
		exit(errno);
	}
}

void setFirstCommandParent(int fd[2]){
	printf("first command parent");
	close(fd[1]);
}

void setFirstCommandChild(int fd[2]){
	printf("first command child");
	safeDup2(fd[1], STDOUT_FILENO);
	close(fd[1]);
	close(fd[0]);
}

void setLastCommandParent(int fd[2]){
	printf("last command parent");
	close(fd[0]);
}

void setLastCommandChild(int fd[2]){
	printf("last command child");
	safeDup2(fd[0], STDIN_FILENO);
	close(fd[0]);
}

void setMiddleCommandChild(int fdprev[2], int fdnext[2]){
	printf("middle command child");
	safeDup2(fdprev[0], STDIN_FILENO);
	close(fdprev[0]);
	safeDup2(fdnext[1], STDOUT_FILENO);
	close(fdnext[1]);
	close(fdnext[0]);
}

void setMiddleCommandParent(int fdprev[2], int fdnext[2]){
	printf("middle command parent");
	close(fdprev[0]);
	close(fdnext[1]);
}

int main(int argc, char *argv[])
{
	// exit if there's an incorrect number of args
	if (argc <= 1){
		printf("Error %d: Incorrect number of variables\n", EINVAL);
		exit(EINVAL);
	}

	// set up pipe organization for all processes
	int *pipes[8]; // holds all pipes in the program
	
	int mode = -1;
	for (int i=1; i<argc; i++){
		printf("Command %d\n", i);
		// initial pipe setup
		int fd[2];
		if (argc == 2){
			mode = ONLY_CMD;
		} else if (i == 1){
			mode = FIRST_CMD;
			pipes[i] = &fd;
			pipe(fd);
		} else if (i == (argc-1)){
			mode = LAST_CMD;
		} else {
			mode = MIDDLE_CMD;
			pipes[i] = &fd;
			pipe(fd);
		}

		int pid = fork(); // copy file descriptors in parent to child
		
		switch(pid){
			case 0: 
				// child process
				printf("Child process: %d\n", pid);
				// initalize child fd
				switch(mode){
					case ONLY_CMD:
						// do nothing
						break;
					case FIRST_CMD:
						setFirstCommandChild(fd);
						break;
					case MIDDLE_CMD:
						// setMiddleCommandChild(fd);
						break;
					case LAST_CMD:
						setLastCommandChild(fd);
						break;
					default:
						// should never get here by design
						break;
				}
				// execute the command
				if (execlp(argv[i], argv[i], NULL) == -1){
					printf("Error %d: Command execution failed", errno);
					exit(errno);
				}
				exit(0);
				break;
			case -1:
				// error occurred
				printf("Error %d: Child creation unsuccessful\n", ECHILD);
				exit(ECHILD);
				break;
			default:
				// parent process
				printf("Parent process\n");
				// initalize parent fd
				switch(mode){
					case ONLY_CMD:
						// do nothing
						break;
					case FIRST_CMD:
						setFirstCommandParent(fd);
						break;
					case MIDDLE_CMD:
						// setMiddleCommandParent(fd);
						break;
					case LAST_CMD:
						setLastCommandParent(fd);
						break;
					default:
						// should never get here by design
						break;
				}
				// wait for child execution
				int status = 0;
				if (waitpid(pid, &status, 0) < 0){
					printf("Error %d: waitpid() failed.\n", errno);
					exit(errno);
				}
				if (!WIFEXITED(status) || WEXITSTATUS(status) != 0){
					exit(errno);
				}
				break;
		}
		printf("-------\n");
	}
	return 0;
}
