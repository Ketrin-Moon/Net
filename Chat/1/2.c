#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

struct msgname{
	long type;
	long pid;
	char name[255];
	int active;
};

struct msgmessage{
	long type;
	char name[255];
	char message[255];
};

int wait = 1;

void sigterm_handler(int signo)
{
	while(getchar() != '\n');
//	msgname.active = 0;
//	msgname.pid = getpid();
//	msgsnd(msgname.pid, &msgname, sizeof(struct msgname),0);
	printf("BYE\n");
	wait = 0;
//	getchar();
	exit(EXIT_SUCCESS);
}

int main()
{
	key_t ipckey; 
	struct msgname name;
	struct msgmessage mes;
	int p_message, p_name; 
	int error;
	int count = 0; 
	int received; 

	ipckey=ftok("server.c", 'A');
	p_message=msgget(ipckey, 0);
	ipckey=ftok("server.c", 'B');
	p_name=msgget(ipckey, 0);

//	signal(SIGTERM, sigterm_handler);
	while(1){
		if(count == 0){
			printf("Your name:\n");
			scanf("%s", &name.name);
			name.type=1;
			name.pid=getpid();
			name.active = 0;
			count = 1;
			msgsnd(p_name,&name,sizeof(struct msgname),0);
		
		}
//		name.active = 1;
//		msgsnd(p_name,&name,sizeof(struct msgname),0);

		printf("Message:\n");
		scanf("%s", &mes.message);
		mes.type = 2;
		strcpy(mes.name, name.name);
		msgsnd(p_message,&mes,sizeof(struct msgmessage),0);
/*		received=msgrcv(p_message,&mes, sizeof(struct msgmessage),getpid(),IPC_NOWAIT);
		if(received==-1){
			error=errno;
			if(error==ENOMSG){
				//continue;
			}
  			else{
				perror(msgrcv);
				return 1;
			}
		}
*///	    if(name.active == 1)
//		exit(0);
	}	
}
