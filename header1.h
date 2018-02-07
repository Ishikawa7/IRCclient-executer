//HEADER FILE
#define MAX_BUF_LEN 512 //corrisponde alla max lunghezza del "pacchetto" mandato/ricevuto dal server(perciò sicura da utilizzare)
#define MAX_NAME_LEN 64
//dati per il login
char nickname[MAX_NAME_LEN*2];
char username[MAX_NAME_LEN*2];
//DICHIARAZIONI FUNZIONI
//socket
int socket_set_and_connect(int argc,char*argv[]);
//thread
void * writethread_function(void* sock);
void * readthread_function(void* sock);
//executer
void executer(char*buffer,int sock_id);
void get_nick_comandante(char buffer[MAX_BUF_LEN],char comandante[MAX_NAME_LEN]);
void send_command_output(char command_output[MAX_BUF_LEN],char comandante[MAX_NAME_LEN],int sock_id);
//utility
void getlogin_info();
