//progetto client IRC sistemi operativi 
//CLIENT+EXECUTER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "header.h"
//utilizzo due thread per migliorare le prestazioni uno di write e uno di read
int main(int argc,char *argv[]) //viene passato come argomento l'hostname e la porta
	{
	if(argc<3)
		{
		printf("[CLIENT]Insufficent server information\n");
		exit(EXIT_FAILURE);
		}
	pthread_t write_thread, read_thread;
	int server_sock,return_value;
	//OTTENIMENTO INFORMAZIONI PER L'ACCESSO AL SERVER
	printf("[CLIENT] ottenimento credenziali accesso...\n");
	getlogin_info();
	printf("[CLIENT] done\n");
	//CREAZIONE IMPOSTAZIONE E CONNECT DEL SOCKET
	server_sock=socket_set_and_connect(argc,argv);
	//THREADS CREATION
	printf("[CLIENT] thread creation...\n");
	return_value=pthread_create(&write_thread,NULL,writethread_function,(void*)&server_sock);
	if(return_value==-1)
		{
		perror("[ERROR]Creating writethread error\n");
		exit(EXIT_FAILURE);
		}
	return_value=pthread_create(&read_thread,NULL,readthread_function,(void*)&server_sock);
	if(return_value==-1)
		{
		perror("[ERROR]Creating readthread error\n");
		exit(EXIT_FAILURE);
		}
	printf("[CLIENT] done\n");
	//il main thread aspetta la terminazione del thread di write che avverà con "QUIT" dopo di che cancella il thread di read, attende la sua terminazione, chiude il socket e termina
	return_value=pthread_join(write_thread,NULL);
	if(return_value!=0)
		{
		perror("[ERROR]Join fail\n");
		exit(EXIT_FAILURE);
		}
	pthread_cancel(read_thread);
	return_value=pthread_join(read_thread,NULL);
	if(return_value!=0)
		{
		perror("[ERROR]Join fail\n");
		exit(EXIT_FAILURE);
		}
	//chiusura socket e uscita
	close(server_sock);
	exit(EXIT_SUCCESS);
	}
