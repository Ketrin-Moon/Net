#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
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

typedef struct client{
	char name[255];
	long pid;
 	struct Client *next;
}Client;

Client *head = NULL;

void push(Client **head, char* my_name, int pid)
{
	Client *tmp = (Client*)malloc(sizeof(Client));
	printf("%s(%d) зашел в чат\n", my_name, pid);
	strcpy(tmp->name, my_name);
//	strcpy(tmp->pid, pid);
	tmp->pid = pid;
	tmp->next = (*head);
	(*head) = tmp;
}

Client* getNth(Client* head, int pid)
{
	Client *hh;
	hh = head->next;
        while(hh){
	    if(hh->pid == 0)
		break;
	    else
		if(hh->pid == pid){
		    head = hh;
		    return head;
		}
	}
//	return head;
}

Client* getLast(Client *head)
{
	if(head == NULL)
	    return NULL;
	while(head->next){
	    head = head->next;
	}
	return head;
}

void pushBack(Client **head, char* my_name, int pid)
{
        Client *last = getLast(head);
	Client *tmp = (Client*)malloc(sizeof(Client));
	printf("%s(%d) зашел в чат\n", my_name, pid);
	strcpy(tmp->name, my_name);
	tmp->pid = pid;
//	strcpy(tmp->pid, pid);
	tmp->next = NULL;
	last->next = tmp;
}

int pop(Client **head)
{
    Client* prev = NULL;
    int pid;
    if(head == NULL)
	exit(-1);
    prev = (*head);
    pid = prev->pid;
    (*head) = (*head)->next;
    free(prev);
    return pid;
}
/*
Client* getLastButOne(Client* head)
{
    if(head == NULL)
	exit(-2);
    if(head->next == NULL)
	return NULL;
    while(head->next->next)
	head = head->next;
    return head;
}
*/
int popBack(Client **head)
{
	Client *pF = NULL;	//текущий узел
	Client *pB = NULL;	//предыдущий узел

	if(!(*head))
	    exit(-1);
	pF = *head;
	while(pF->next){
	    pB = pF;
	    pF = pF->next;
	}
	if(pB == NULL){
	    free(*head);
	    *head = NULL;
	}
	else{
	    free(pF->next);
	    pB->next = NULL;
	}

}


void printList(const Client *head)
{
	while(head){
	    printf("List:\n%s(%d)\n", head->name, head->pid);
	    head = head->next;
	}
	printf("\n");
}
/*
int length(Client *head)
{
    Client* current = head;
    int count = 0;
    while(current != NULL){
	count++;
	current = current->next;
    }
    return count;
}
*/
int deleteNth(Client **head, int pid) {
/*    int n;
    int data = (*head)->pid;
    n = length(head);
    if (n == 0) {
        return pop(head);
    } else {
	int i;
	Client *current = *head;
	for(i = 0; i<n; i++){
	    current = current->next;
	}
	pop(&(current->next));
    }
    return data;
*/
        Client *prev = getNth(*head, pid);
/*	if(prev == head){
	    return pop(head);
	}
*/      Client *elm  = prev->next;
	printf("ELM: %s\n", elm);
        int val = elm->pid;
 
        prev->next = elm->next;
        free(elm);
        return val;
//    }

}

void deleteList(Client **head) {
    Client* prev = NULL;
    while ((*head)->next) {
        prev = (*head);
        (*head) = (*head)->next;
        free(prev);
    }
    free(*head);
}


void queue_name(int p_name)
{
	int received, error;
	struct msgname name;
	Client my_client;
//	Client* head = NULL;
	
	while(1){
	    received = msgrcv(p_name,&name,sizeof(struct msgname),1,0);
    	    if(received == -1){
		error = errno;
		if(error != ENOMSG){
			perror(msgrcv);
			return 1;
		}
	    }
	    my_client.pid = name.pid;
	    strcpy(my_client.name, name.name);

	    if(name.active == 1){
		printf("%s вышел из чата\n", my_client.name);
		deleteNth(&head, my_client.pid);
		printList(head);
	    }

    	    if(head == NULL){
		push(&head, my_client.name, name.pid);
	    }
	    else
		pushBack(head, my_client.name, name.pid);
//	    printList(head);
	    //}
	
	}
}

void queue_message(int p_message)
{
	struct msgmessage message;
	int received, error;
	sleep(2);
	while(1){
	    received = msgrcv(p_message,&message,sizeof(struct msgmessage),2,0);
    	    printf("%s: %s\n", message.name, message.message);
	    if(received == -1){
	    	error = errno;
        	if(error == ENOMSG){
                    continue;
                }
            	    perror(msgrcv);
                    return 1;
	    }
	}
}


int main(){
	key_t ipckey;
	pthread_t t_name;
	pthread_t t_message;
//	struct msgname name;
//	struct msgmessage message;
	int p_message, p_name;
	int status1, status2;
//	int received, error;
//	Client* head = NULL;
//	Client my_client;


	ipckey=ftok("server.c", 'A');
	p_message=msgget(ipckey, IPC_CREAT|0666);
	ipckey=ftok("server.c", 'B');
	p_name=msgget(ipckey, IPC_CREAT|0666);

    	status1 = pthread_create(&t_name, NULL, queue_name, p_name);
	status2 = pthread_create(&t_message, NULL, queue_message, p_message);
	pthread_join(t_name, status1);
	pthread_join(t_message, status2);

/*	pid = fork();
	if(pid == 0){
	while(1){
	    received = msgrcv(p_name,&name,sizeof(struct msgname),1,0);
    	    if(received == -1){
		error = errno;
		if(error != ENOMSG){
			perror(msgrcv);
			return 1;
		}
	    }
	    my_client.pid = name.pid;

	    strcpy(my_client.name, name.name);
	    if(name.active == 1){
		printf("%s вышел из чата\n", my_client.name);
		deleteNth(&head, my_client.pid);
		printList(head);
	    }

    	    if(head == NULL){
		push(&head, my_client.name, name.pid);
	    }
	    else
		pushBack(head, my_client.name, name.pid);
//	    printList(head);
	    }
	
	}

	while(1){
	    received = msgrcv(p_message,&message,sizeof(struct msgmessage),2,0);
    	    printf("%s: %s\n", message.name, message.message);
	    if(received == -1){
	    	error = errno;
        	if(error == ENOMSG){
                    continue;
                }
            	    perror(msgrcv);
                    return 1;
	    }
	}
*/
	msgctl(p_message, IPC_RMID, 0);
	msgctl(p_name, IPC_RMID, 0);
	deleteList(&head);

}
