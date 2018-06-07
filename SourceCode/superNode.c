
/*

Once a user process is authenticated, then super node process is supposed to receive requests only when a user process wants to find another user process that is not already known to it. 
If the process is already known, the super node is not contacted at all. 
Each user process will register its IP address and port number with the super node responsible for it.Once it finds it, it returns it to the user process.

*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

#define MAX 20

void *registerUser(void *);
struct node *find(char *);
void insert(char * , char *);



struct node {
   char *ip;
   char *port;
   struct node *next;
};

struct node *head = NULL;
struct node *current = NULL;

int main(int argc, char * argv[])
{
    struct sockaddr_in address;
    int sd,i,cd,val;
    int serverPort;
    int status = 0;
    int size = sizeof(struct sockaddr_in);
     int opt = 1;
   

    pthread_t clients[MAX];

//checking for correct arguments

    if (argc != 2){
        
        printf("Wrong Input.\n");
        printf("Kindly Enter Correct Values.\n");
        exit(0);
        
    }

    serverPort = atoi(argv[1]);
    if(serverPort <= 0 || serverPort > 65535){
        printf("\nPlease enter correct port\n" );
        exit(1);
    }
    
    sd = socket(AF_INET,SOCK_STREAM,0);

    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
          sizeof(opt)) < 0)
        {
          perror("setsockopt()");
          exit(1);
        }


//listening for user processes

    memset(&address, 0, sizeof(address));
    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=serverPort;
    address.sin_family=AF_INET;

    printf("\nSocket Created\n");

    if(bind(sd,(struct sockaddr*)&address,sizeof(address))>=0)
    {
        printf("\nBind Succesful\n");
        
        
        if(listen(sd,0)<0){
          perror("\nError in listening on port");
            exit(3);
        }
  
        printf("\nSuper Node is listening on port %d\n",serverPort);


        while(1)       
        {
            for(i=0;i<MAX;i++)      
            {
                if((cd=accept(sd, (struct sockaddr *)&address, &size))>=0){
                  printf("\nConnection Request from %s!\n", inet_ntoa(address.sin_addr));
                  printf("\nConnected to process on socket on %d\n",cd);


              // Creating new threads to handle user processes 

                  val = pthread_create(&clients[i],NULL,registerUser,(void *) cd);

                  if(val !=0 ){

                    printf("\nError in thread creation");
                    exit(1);
                    }

                  
                }
               
            }


        }

        for(i=0;i<MAX;i++){
          pthread_join(clients[i],NULL);
        }

        close(cd);
    } 

    else {
        printf("\nsocket creation failed");
    }
    return 0;
}


// method for registering users


void *registerUser(void *clientDescriptor){
  int cd=(int)clientDescriptor;
   char ip[1024];
   char port[1024];
   char buffer[1024];
   int bytes;

    printf("\nWaiting for data from User : \n");
    recv(cd,ip,sizeof(ip),0);
    printf("\nIP Received from user %d is : %s",cd,ip);
    recv(cd,port,sizeof(port),0);
    printf("\nPort Received from user %d is : %s",cd,port);
    printf("\nStoring ip and port to a linked list\n");
    insert(ip,port);
    printf("\nRegistered and Stored Data Succesfully\n"); 

    bytes=recv(cd,buffer,sizeof(buffer),0);
    recv(cd,port,sizeof(port),0);
    printf("\nSearch data received from user: %s",port);
    buffer[bytes]='\0';

    if (atoi(buffer)==1)
    {
     struct node* data = find(port);
                if(data != NULL) {
                  printf("\nDetails found: \n");
                  printf("ip = %s and port = %s \n",data->ip,data->port);
                  printf("\nSending Details...\n");
                  memset(ip,NULL,sizeof(ip));
                  memset(port,NULL,sizeof(port));
                  strcpy(ip,data->ip);
                  strcpy(port,data->port);
                  send(cd,"1",1,0);
                  send(cd,ip,strlen(ip),0);
                  send(cd,port,strlen(port),0);
                 } 
                 else {
                    printf("\nDetails not found with super node\n");
                    send(cd,"0",1,0);
                 }
    }

}

 

 //method for adding users to list

void insert(char *ip, char *port) {
   
   struct node *link = (struct node*) malloc(sizeof(struct node));
   link->ip = malloc(strlen(ip)+1);
    link->port = malloc(strlen(port)+1);
   link->ip = strcpy(link->ip,ip);
   link->port = strcpy(link->port,port);
   link->next = head;
   head = link;

   return;
}

//method for finding users in list

struct node *find(char *port) {

   struct node* current = head;

   if(head == NULL) {
      return NULL;
   }

   while(strcmp(current->port,port) != 0) {

      if(current->port == NULL) {
         return NULL;
      } else {
         current = current->next;
      }
   }      

   return current;
}