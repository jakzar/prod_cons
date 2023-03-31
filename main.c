#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define SIZE 1024


//komunikaty
struct mesg_buffer{
	long mesg_type;
	char mesg_text[SIZE];
	int mesg_size;
}message;


key_t key;
int msgid;
	

//bufor danych
char buffer[SIZE];
//char count[SIZE];
	

int main(void){
	
	int pipefd[2];
	if(pipe(pipefd)==-1){
		printf("blad");
	}
	key = ftok(".", 0);
	msgid=msgget(key, IPC_CREAT | 0660);
	

	if(fork()==0){
		
		while(1){
		
			int size=read(STDIN_FILENO, buffer, SIZE);
			
			if(size<1){
				break;
			}
			
			
			write(pipefd[1], buffer, size);
			
		}
		
		close(pipefd[0]);
	}
	
	
	else if(fork()==0){
		
		
		while(1){
			int size=read(pipefd[0], buffer, SIZE);
			
			fprintf(stderr, "%d\n", size);
			
			if(size<1){
				break;
			}
			
			//write(STDOUT_FILENO, buffer, size);
			message.mesg_type=1;
			memcpy(message.mesg_text, buffer, size);
			message.mesg_size=size;
			msgsnd(msgid, &message, sizeof(message), 0);
			
		}
		
		message.mesg_size=-1;
		msgsnd(msgid, &message, sizeof(message), 0);
		
	}
	
	
	else if(fork()==0){
		
		while(1){
			msgrcv(msgid, &message, sizeof(message), 1, 0);
			
			if(message.mesg_size==-1){
				break;
			}
			
			memcpy(buffer, message.mesg_text, message.mesg_size);
			write(STDOUT_FILENO, buffer, message.mesg_size);
			fflush(stdout);
		}
		
		fprintf(stderr, "Koniec dzialania");
		msgctl(msgid, IPC_RMID, 0);
		kill(getppid(), 2);
		
	}
	
	pause();
	return 0;
}

