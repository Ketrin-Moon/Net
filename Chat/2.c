#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

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

void queue_name(int p_name)
{
    int count = 0;
    struct msgname name;
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
//	sleep(2);
	name.active = 1;
	msgsnd(p_name,&name,sizeof(struct msgname),0);
//	name.active = 0;
	if(name.active == 1)
	    exit(0);	    
    }
}

void queue_message(int p_message)
{
    int received, error;
    struct msgmessage mes;
    sleep(2);
    while(1){    
	printf("Message:\n");
	scanf("%s", &mes.message);
	mes.type = 2;
	//strcpy(mes.name, name.name);
	msgsnd(p_message,&mes,sizeof(struct msgmessage),0);
	received=msgrcv(p_message,&mes, sizeof(struct msgmessage),getpid(),IPC_NOWAIT);
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
    }	
}

int main()
{
	key_t ipckey; 
	pthread_t t_message;
	pthread_t t_name;
//	struct msgname name;
//	struct msgmessage mes;
	int p_message, p_name; 
	int error;
	int status1, status2; 
	int received; 

	ipckey=ftok("server.c", 'A');
	p_message=msgget(ipckey, 0);
	ipckey=ftok("server.c", 'B');
	p_name=msgget(ipckey, 0);

	status1 = pthread_create(&t_name, NULL, queue_name, p_name);
	status2 = pthread_create(&t_message, NULL, queue_message, p_message);
	pthread_join(t_name, status1);
	pthread_join(t_message, status2);

/*	while(1){
		if(count == 0){
			printf("Your name:\n");
			scanf("%s", &name.name);
			name.type=1;
			name.pid=getpid();
			name.active = 0;
			count = 1;
			msgsnd(p_name,&name,sizeof(struct msgname),0);
		
		}
		printf("Message:\n");
		scanf("%s", &mes.message);
		mes.type = 2;
		strcpy(mes.name, name.name);
		msgsnd(p_message,&mes,sizeof(struct msgmessage),0);
		received=msgrcv(p_message,&mes, sizeof(struct msgmessage),getpid(),IPC_NOWAIT);
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
	}	
*/
}
