#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <pthread.h>
#include "interface.h"

struct msgname{
	long type;
	long pid;
        char name[255];
	int active;
};

struct msgmessage{
	long type;
	char name[255];
	int pid;
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

//	wprintw(user_window, "%s\n", my_name);
//	wprintw(chat_window, "%s - connected\n", my_name);
//	wrefresh(user_window);
//	wrefresh(chat_window);
	strcpy(tmp->name, my_name);
	tmp->pid = pid;
	tmp->next = (*head);
	(*head) = tmp;
}

Client* getNth(Client* head, int pid)
{
	Client *hh = head;
	printf("GETNTH\n");

	if(head->pid == pid){
//		printf("Head: %s(%d)\n", head, head->pid);
		return head;
	}
//	else{
//	hh = head->next;
	while(hh!=NULL){
	    hh = hh->next;
/*	    if(hh->pid == 0){
		printf("PID == 0\n");
		break;
	   }
*/
	    if(head->next == NULL){
		if(head->pid == pid){
		    printf("Head: %s(%d)\n", head, head->pid);
		    return head;
		}
	    }
	    else
		if(hh->pid == pid){
		    head = hh;
		    printf("HEAD: %s %d %s\n", head, hh->pid, hh->next);
		    return head;
		}
//	}
	}
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
//	wprintw(user_window, "%s\n", my_name);
//	wprintw(chat_window, "%s - connected\n", my_name);
//	wrefresh(user_window);
//	wrefresh(chat_window);

	strcpy(tmp->name, my_name);
	tmp->pid = pid;
	tmp->next = NULL;
	last->next = tmp;
}

void pop(Client **head)
{
    Client* prev = NULL;
    if(head == NULL)
	exit(-1);
    prev = (*head);
    (*head) = (*head)->next;
    free(prev);
}

Client* getLastButOne(Client* head) 
{
    Client *hh;
//    printList(head);
    if (head == NULL) {
        exit(-1);
    }
/*    if (head->next == NULL) {
        return NULL;
    }
*///  hh = head->next;
    while (head->next) {
	hh = head->next;
	if(hh->next == NULL){
	    head = hh;
	    return head;
	}
        head = hh;
    }
    return head;
}


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


void popBack(Client **head) {
    Client *lastbn = NULL;
    //Получили NULL
    printf("PopBack\n");
//    printList(*head);
    if (!head) {
        exit(-1);
    }
    //Список пуст
    if (!(*head)) {
        exit(-1);
    }
    lastbn = getLastButOne(*head);
    //Если в списке один элемент

    if (lastbn == NULL) {
        free(*head);
        *head = NULL;
    } else {
        free(lastbn->next);
        lastbn->next = NULL;
    }
}

/*
void popBack(Client **head)
{
        Client *pF = NULL;
        Client *pB = NULL;

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
*/
void printList(const Client *head)
{
	while(head){
	    printf("LIST:\n%s(%d)\n", head->name, head->pid);
	    head = head->next;
	}
	printf("\n");
}

void deleteNth(Client **head, int pid)
{
//	Client *prev = getLastButOne(*head);
	Client *current = getNth(*head, pid);
	Client *prev = getLastButOne(*head);
	int n;

	if((current == *head) && (prev->next == NULL)){
		printf("Pop\n");
		pop(head);
	}

	if((current->next == NULL) && (prev->next == NULL)){
		printf("PopBack\n");
		popBack(*head);
	}

	if(current->next == NULL){
		printf("Prev: %s\n", prev);
		prev->next = NULL;
	}
	if(current->next != NULL){
		prev->next = current->next;
		printf("Prev->next: %s\n", prev->next);
		free(current);
	}

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
/*	    if(name.active == 1){
		printf("%s вышел из чата\n", my_client.name);
//		deleteNth(&head, my_client.pid);
		popBack(&head);
		printList(head);
	    }
*/
    	    if(head == NULL){
		push(&head, my_client.name, name.pid);
	    }
	    else
		pushBack(head, my_client.name, name.pid);
//	    printList(head);
	}

}

void queue_message(int p_message)
{
	struct msgmessage message;
	int received, error;
	char flag[3] = "bye";
//	sleep(2);
	while(1){
	    received = msgrcv(p_message,&message,sizeof(struct msgmessage),2,0);
	    if(strcmp(message.message, flag) == 0){
		printf("%s вышел из чата\n", message.name);
		deleteNth(&head, message.pid);
		//popBack(&head);
		printList(head);
	    }
	   else{
  	    printf("%s: %s\n", message.name, message.message);
//	    wprintw(chat_window, "%s:%s\n", message.name,message.message);
//	    wrefresh(chat_window);
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
}


int main()
{
	key_t ipckey;
	pthread_t t_name;
	pthread_t t_message;
	int p_message, p_name;
	int status1, status2;
	Client my_client;

/*
	ipckey=ftok("server.c", 'A');
	p_message=msgget(ipckey, IPC_CREAT|0666);
	ipckey=ftok("server.c", 'B');
	p_name=msgget(ipckey, IPC_CREAT|0666);
//	window();
    	status1 = pthread_create(&t_name, NULL, queue_name, p_name);
	status2 = pthread_create(&t_message, NULL, queue_message, p_message);
	pthread_join(t_name, status1);
	pthread_join(t_message, status2);

	msgctl(p_message, IPC_RMID, 0);
	msgctl(p_name, IPC_RMID, 0);
*/

	char str1[20] = "hello world";
	char str2[20] = "bobobobob";
	char str3[20] = "qqqqqqqqqqqq";
	int pid1 = 10202;
	int pid2 = 49494;
	int pid3 = 27272;
	printf("INSERT 1\n");
	strcpy(my_client.name, str1);
	my_client.pid = pid1;
	push(&head, my_client.name, my_client.pid);
	printList(head);
	printf("INSERT 2\n");
	strcpy(my_client.name, str2);
	my_client.pid = pid2;
	pushBack(head, my_client.name, my_client.pid);
	printList(head);
	printf("INSERT 3\n");
	strcpy(my_client.name, str3);
	my_client.pid = pid3;
	pushBack(head, my_client.name, my_client.pid);
	printList(head);
	printf("DELETE\n");
	deleteNth(&head, pid1);
//	printf("Вывод списка\n");
	printList(head);
//	deleteList(&head);

}
