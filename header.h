//HEADER FILE
#define MAX_BUF_LEN 512 //corrisponde alla max lunghezza del "pacchetto" mandato/ricevuto dal server(perciò sicura da utilizzare)
#define MAX_NAME_LEN 32
//dati per il login
char nickname[MAX_NAME_LEN*2];
char username[MAX_NAME_LEN*2];
//DICHIARAZIONI FUNZIONI
int socket_set_and_connect(int argc,char*argv[]);
void * writethread_function(void* sock);
void * readthread_function(void* sock);
void executer(char*buffer);
void getlogin_info();
