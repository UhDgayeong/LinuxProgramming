#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	char str[128];
	char *cd1, *cd2;
	char tmp[128];
	int amper, status;
	pid_t pid;
	int fd;
	int filedes[2];
	
	while (1) {
		printf("[prompt] ");
		fgets(str, sizeof(str), stdin);
		str[strlen(str)-1] = '\0';

		// exit myShell
		if (strcmp(str, "exit") == 0) {
			break;
		}
		if (strcmp(str, "logout") == 0) {
			break;
		}

		// background command
		if (str[strlen(str)-1] == '&') {
			amper = 1;
		}
		// foreground command
		else {
			amper = 0;
		}
		
		pid = fork();
		if (pid == -1) {
			printf("fail to call fork()\n");
			exit(1);
		}

		if (pid == 0) { // child
			if (strchr(str, '>') != NULL) {
				// change output direction to a file
				// ex) ls>a.txt
				cd1 = strtok(str, ">");
				cd2 = strtok(NULL, ">");
				fd = open(cd2, O_CREAT|O_TRUNC|O_WRONLY, 0600);
				close(STDOUT_FILENO);
				dup(fd);
				close(fd);
			}
			if (strchr(str, '<') != NULL) {
				// ex) cat<a.txt
				cd1 = strtok(str, "<");
				cd2 = strtok(NULL, "<");
				fd = open(cd2, O_CREAT|O_RDONLY, 0600);
				close(STDIN_FILENO);
				dup(fd);
				close(fd);
			}
			
			if (strchr(str, '|') != NULL) {
				//make pipe
				// ex) who|sort
				cd1 = strtok(str, "|");
				cd2 = strtok(NULL, "|");

				pid_t pid2;
				if (pipe(filedes) == -1) {
					printf("fail to call pipe()\n");
					exit(1);
				}
				if ((pid2 = fork()) == -1) {
					printf("fail to call fork()\n");
					exit(1);
				}
				if(pid2 == 0) { //grandchild
					close(STDOUT_FILENO);
					dup(filedes[1]);
					close(filedes[1]);
					close(filedes[0]);
					execlp(cd1, cd1, NULL);
				}
				else { //child
					close(STDIN_FILENO);
					dup(filedes[0]);
					close(filedes[0]);
					close(filedes[1]);
					execlp(cd2, cd2, NULL);
				}	
			}
			else {
				// exec command.
				if (strchr(str, '&') != NULL) {
					cd1 = strtok(str, "&");
				}
				else {
					cd1 = str;
				}
				execlp(cd1, cd1, NULL);
				exit(1);
			}
		} // end child

		else { // parent
			if (amper == 0) {
				//printf("waiting for child to end...\n");
				wait(&status); // wait child to end
			}
		}
		
	} // end while
}
