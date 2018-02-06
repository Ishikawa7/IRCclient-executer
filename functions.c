//FUNZIONI CLIENT
//////////////////
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
//////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//////////////////
#include <pthread.h>
//////////////////
#include "header.h"
///////////////////
const char* PAROLACHIAVE ="KEYWORD_COMANDO:";
//CREAZIONE ED IMPOSTAZIONE DEL SOCKET//////////////////
int socket_set_and_connect(int argc,char*argv[])
	{
	int server_sock,return_value;
	struct sockaddr_in server_addr;
	struct hostent * host;
	//creazione unnamed sock
	printf("[CLIENT] creazione unnamed sock... \n");
	server_sock=socket(AF_INET,SOCK_STREAM,0);
	if(server_sock==-1)
		{
		perror("[ERROR]Socket creation error\n");
		exit(EXIT_FAILURE);
		}
	printf("[CLIENT] done\n");
	printf("[CLIENT] gethostbyname...\n");
	//gethostbyname
	host=gethostbyname(argv[1]);
	if(host==NULL)
		{
		perror("[ERROR]Error host\n");
		exit(EXIT_FAILURE);
		}
	printf("[CLIENT] done\n");
	printf("[CLIENT] impostazioni server...\n");
	server_addr.sin_family=AF_INET;
	//passo l'address da host a server_addr
	server_addr.sin_addr=*(struct in_addr *)*host->h_addr_list;/*il puntatore al primo elemento della lista h_addr_list viene castato al tipo struct in_addr e poi deferenziato*/
	server_addr.sin_port=htons(atoi(argv[2]));//converte prima la stringa in intero e poi il numero di porta in formato network
	printf("[CLIENT] done \n");
		printf("[CLIENT] connect...\n");
	//Connect
	if(connect(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
		{
		perror("[ERROR]Connect error\n");
		exit(EXIT_FAILURE);
		}
	printf("[CLIENT] done\n");
	//return
	return server_sock;
	}
///////////////////////////////////////////////////////
//EXECUTER
void executer(char*buffer)
	{
	char command[MAX_BUF_LEN];
	char *token;
	FILE * write_fp;
	//controllo sul buffer per cercare la parola chiave
	if((token=strstr(buffer,PAROLACHIAVE))!=NULL) //strstr restituisce dalla prima occorezzanza della parola chiave in poi
		{
		printf("°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°EXECUTER°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°\n");
		printf("[EXECUTER]parola chiave trovata!\n");
		printf("[EXECUTER]ritorno da strstr:%s",token);
		token=strtok(token,PAROLACHIAVE); //separo la parola chiave dal comando sfruttando la funzione strtok che divide la stringa data in token il cui separatore è la key word, restituisce il primo token ottenuto cioè il primo comando dopo la prima parola chiave
		printf("[EXECUTER]token dopo la parola chiave:%s",token);
		memset(command, '\0', MAX_BUF_LEN); //pulizia buffer command
		strcpy(command,token);
		printf("[EXECUTER] comando estratto :%s[EXECUTER] Di lunghezza:%d\n",command,(int)strlen(command));
		printf("[EXECUTER] valore caratteri 'misteriosi' :%d-%d\n",command[strlen(command)-2],command[strlen(command)-1]);
		command[strlen(command)-2]='\0'; //tolgo gli ultimi 2 caratteri in più del server {carriage return(13) line feed(10)}
		printf("[EXECUTER] comando estratto :%s ///// Di lunghezza:%d\n\n",command,(int)strlen(command));
		/*if((write_fp=popen(command,"w"))==NULL)
			{
			printf("Errore esecuzione comando\n");   //alternativa
			}*/
		if((system(command))==-1) 
			{
			printf("[ERROR]errore esecuzione comando\n");
			}
		printf("°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°\n");	
		}
	}
///////////////////////////////////////////////////////
//THREAD DI COMUNICAZIONE VERSO SERVER
void * writethread_function(void* sock)
	{
	printf("[CLIENT] thread di scrittura avviato\n");
	int sock_id,return_value,running=1;
	char write_buffer[MAX_BUF_LEN];
	sock_id=*(int*)sock;
	printf("[CLIENT] procedura di accesso automatica...\n");
	//procedura automatica di ingresso con NICKNAME USERNAME REALNAME
	printf("[DI CONTROLLO] comando nickname %s",nickname);
	return_value=send(sock_id,nickname,strlen(nickname),0); 
	if(return_value==-1)
		{
		perror("[ERROR]Send error\n");
		exit(EXIT_FAILURE);
		}
	printf("[DI CONTROLLO] comando username %s",username);
	return_value=send(sock_id,username,strlen(username),0); 
	if(return_value==-1)
		{
		perror("[ERROR]Send error\n");
		exit(EXIT_FAILURE);
		}
	printf("[CLIENT] done \n");
	fflush(stdin);
	////////////////////////////// Core Write Thread //////////////////////////
	//è sempre in attesa dell'input dall'utente che viene memorizzato nel write_buffer e poi inviato
	while(running) //termina con "QUIT"
		{
		memset(write_buffer, '\0', MAX_BUF_LEN); //pulizia buffer
		while(fgets(write_buffer,MAX_BUF_LEN-1,stdin)==NULL);
		if(strstr(write_buffer,"QUIT")!=NULL)
			{
			printf("[CLIENT] comando QUIT inserito, arresto client\n");
			running=0;
			return_value=send(sock_id,"QUIT",4,0);//manda QUIT al server
			if(return_value==-1)
				{
				perror("[ERROR]Send error\n");
				exit(EXIT_FAILURE);
				}
			continue;
			}
		return_value=send(sock_id,write_buffer,(int)strlen(write_buffer),0);
		fflush(stdin);
		if(return_value==-1)
			{
			perror("[ERROR]Send error\n");
			exit(EXIT_FAILURE);
			}
		printf(":-----------------------------------------------------------------------:\n"); 
		}
	pthread_exit(NULL);
	}
//THREAD COMUNICAZIONE DA SERVER ///////////////////////////////////////////////////////
void * readthread_function(void* sock)
	{
	printf("[CLIENT] thread di lettura avviato \n");
	int sock_id,return_value,running=1;
	char read_buffer[MAX_BUF_LEN];
	char * msg_pong;
	sock_id=*(int*)sock;
	//è sempre in attesa di dati dal server che vengono memorizzati nel read_buffer e poi stampati
	while(running)
		{
		memset(read_buffer, '\0', MAX_BUF_LEN); //pulizia buffer 
		return_value=recv(sock_id,read_buffer,MAX_BUF_LEN,0); 
		if(return_value==-1)
			{
			perror("[ERROR]Recv error\n");
			exit(EXIT_FAILURE);
			}
		if(return_value==0)
			{
			printf("[CLIENT] Connessione chiusa dal server 'QUIT' per chiudere il client\n");
			running=0;
			break;
			}
		if(strstr(read_buffer,"PING :")!=NULL) //nel caso in cui viene ricevuto un PING si risponde automaticamente con PONG
			{
			msg_pong="PONG : connected";
			return_value=send(sock_id,msg_pong,strlen(msg_pong),0);
			if(return_value==-1)
				{
				perror("[ERROR]Write error\n");
				exit(EXIT_FAILURE);
				}
			}
		else
			{
			printf("##############################[DAL SERVER]################################\n");
			printf("%s\n",read_buffer);
			executer(read_buffer); //qui c'è l'executer che controlla se arriva un comando con PAROLACHIAVE
			}
		}
	}
///////////////////////////////////////////////////////
//OTTENIMENTO INFORMAZIONI PER L'ACCESSO AL SERVER E COSTRUZIONE COMANDO NICK E COMANDO USER
void getlogin_info()
	{
	printf("[CLIENT] getlogin_info...\n");
	char nick[MAX_NAME_LEN];
	char user[MAX_NAME_LEN];
	char real[MAX_NAME_LEN];
	strcpy(nickname, "NICK ");
	strcpy(username, "USER ");
	printf("[CLIENT]Inserisci NICKNAME: \n");
	while(fgets(nick,MAX_NAME_LEN-1,stdin)==NULL);
	strcat(nickname, nick);
	printf("[CLIENT]Inserisci USERNAME: \n");
	while(fgets(user,MAX_NAME_LEN-1,stdin)==NULL);
	strcat(username, user);
	username[strlen(username)-1]='\0';
	strcat(username, " 0 * : ");
	printf("[CLIENT]Inserisci REALNAME: \n");
	while(fgets(real,MAX_NAME_LEN-1,stdin)==NULL);
	strcat(username, real);
	fflush(stdin);
	}
