#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

void display(char *str) {

	char *tmp;

	for (tmp=str;*tmp;tmp++) 
	{
		write(1,tmp,1);
		usleep(100);
	}
}

int main() {

	struct sembuf up = {0, 1, 0};
	struct sembuf down = {0, -1, 0};

	int my_sem;

	int i;
	pid_t childpid;

	/* create semaphore */
	my_sem = semget( IPC_PRIVATE, 1, 0600);

	/* initialize semaphore to 1 */
	semctl(my_sem, 0, SETVAL, 1);
	
	childpid = fork();
	
	if( childpid == -1)
	{
		fprintf(stderr, "could not create process\n");
		return 0;
	}


	if ( childpid == 0 ) /* parent process */
	{
		for (i=0;i<10;i++) 
		{
			semop(my_sem, &down, 1);
			display("Hello world\n");
			semop(my_sem, &up, 1);
		}
		wait(NULL);

		/* destroy the semaphore*/
		semctl(my_sem, 0, IPC_RMID);
	}
	else /* child process */
	{
		for (i=0;i<10;i++) 
		{
			semop(my_sem, &down, 1);
			display("Bonjour monde\n");
			semop(my_sem, &up, 1);
		}
	}

	return 0;
}
