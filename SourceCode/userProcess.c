/*

Upon starting, it registers a new username and password with the authentication server, 
or just logins by sending username and password to the authentication server. 
Once authenticated, it registers with a super node that is already running by connecting 
to its IP address and port number and sending it your own IP address and port number. 
After that, it may chose to either communicate with an already known user or
 a new user you’re trying to communicate with (become friends with them). 
Upon establishing a communication, two users can now interact by 
sending messages to each others a task that must be demonstrated.

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
#include<fcntl.h>


#define MAX 20
#define MAXSIZE 1024


int authenticateClient(int);
int registerWithSuperNode(void);
struct node *find(char *);
void insert(char * , char *);
void interactOtherUser(char *,char *);

struct node {
   char *ip;
   char *port;
   struct node *next;
};

struct node *head = NULL;
struct node *current = NULL;



int main(int argc, char *argv[] )
{
 struct sockaddr_in serverAddress,clientAddress,otherUserAddress;
 int cd,newSocket,i,otherUserSocket,createdSocket;
 int clientPort,status,serverPort;
 pthread_t users[MAX],otherUser;
 char value;
 int found,num;
char ip[1024],port[1024];
char searchValue[5];
char buffer[1024];
 char ipAddress[50],portNumber[50];
 int opt = 1;
 int size = sizeof(struct sockaddr_in);
 int child;


 int readBytes,writeBytes, statusChild;

//checking for correct arguments


 if (argc != 3){
        
        printf("Wrong Input.\n");
        printf("Kindly Enter Correct Values.\n");
        exit(0);
        
    }

    serverPort = atoi(argv[1]);
    clientPort = atoi(argv[2]);

    if(clientPort <= 0 || clientPort > 65535){
        printf("Please enter correct port\n" );
        exit(1);
    }
    

cd=socket(AF_INET,SOCK_STREAM,0);


 serverAddress.sin_addr.s_addr=inet_addr("127.0.0.1");
 serverAddress.sin_port=serverPort;
 serverAddress.sin_family=AF_INET;

 
//connecting to authentication server

 
 if(connect(cd,(struct sockaddr*)&serverAddress,sizeof(serverAddress))>=0)
 {
	  	printf("Connected to server on socket %d\n",cd);
	  	status = authenticateClient(cd);

	  if(status==0){

	  	printf("Client Not authenticated\n");
	  	printf("Please register\n");  	
	  	close(cd);
	  	exit(1);
	  }

	  else if(status==1){
	  	printf("Client authenticated....Proceed Ahead\n");
	  	shutdown(cd, SHUT_WR);

	  	

	  }

	  else{
	  	printf("Client Registered Successfully....\n");
	  	shutdown(cd, SHUT_WR);
	  	
	  }


	  	if(status == 1 || status == 2){

	  		//register with the authentication server

			status = registerWithSuperNode();
			if(status){
						printf("\nDo want to send request or chat with someone?? Enter y/n ?");
		  				scanf(" %c",&value);
		  				if((value =='y') || (value == 'Y')){

		  					// creating a new process to send request

		  					if(fork()==0){

		  						// searching for user

		  					printf("\n Search for user process");
		  					printf("\n Enter the port number of user to be searched: ");
		  					scanf("%s",port);
					  		struct node* data = find(port);
					  		
                   			memset(buffer,'\0',sizeof(buffer));
                  			

					  		if(data != NULL) {
						      printf("\nDetails found: ");
						      printf("ip = %s and port = %s) ",data->ip,data->port);
						      printf("\n"); 
						      
                  			  strcpy(ipAddress,data->ip);
                  			  strcpy(portNumber,data->port);

							   } 
							   else {
							      printf("\nDetails not found with User");
							      printf("\nContacting super node for advance search");
							   }

							   if(data == NULL){
							   		strcpy(searchValue,"1");
							   		send(status,searchValue,strlen(searchValue),0);
							   		send(status,port,strlen(port),0);

							   		found = recv(status,buffer,sizeof(buffer),0);
							   		buffer[found] = '\0';

							   		memset(ipAddress,'\0',sizeof(ipAddress));
         		         			memset(portNumber,'\0',sizeof(portNumber));
							   		if(atoi(buffer)!=0){
							   		//recv(status,ip,sizeof(ip),0);
							   		//printf("\nFound ip: %s",ip);
							   		//recv(status,ip,sizeof(port),0);
							   		//printf("\nFound port: %s",port);

		                  			//strcpy(ipAddress,ip);
		                  			//strcpy(portNumber,port);

		                  			close(status);

							   		}
							   		else{
							   		printf("\nUser doesn't exist..");
							   		close(status);
							   		}

							   		

							   }
							   	

							   if(strlen(port)>0){

							   	interactOtherUser(ipAddress,port);
							   	exit(0);

							   }

		  					
		  					}

		  					
		  				}

			// listening for others users to contact
	
		    newSocket = socket(AF_INET,SOCK_STREAM,0);


		    if(setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		          sizeof(opt)) < 0)
		        {
		          perror("setsockopt()");
		          exit(1);
		        }


		    memset(&clientAddress, 0, sizeof(clientAddress));
		    clientAddress.sin_addr.s_addr=inet_addr("127.0.0.1");
		    clientAddress.sin_port=clientPort;
		    clientAddress.sin_family=AF_INET;

			    if(bind(newSocket,(struct clientAddress*)&clientAddress,sizeof(clientAddress))>=0)
			    {
			      
			        if(listen(newSocket,1)<0){
			          perror("Error in listening on port");
			            exit(3);
			        }
			  
		        	printf("\nUser process is listening on port %d",clientPort);

		           		if((createdSocket=accept(newSocket, (struct sockaddr *)&clientAddress, &size))>=0){

		           		 printf("\nConnection Request from %s!", inet_ntoa(clientAddress.sin_addr));
		                 printf("\nConnected to process on socket on %d\n",createdSocket);
		                 printf("\nPort number of incoming request %d\n",ntohs(clientAddress.sin_port));


		                 sprintf(ipAddress, "%d", value);
		                 sprintf(portNumber, "%d", value);

		                 //Adding new users to list

		                 insert(ipAddress,portNumber);

		                 	//starting conversation

		                 printf("\nAdding the request to my list");

			                 while(1) {

					                if ((num = recv(createdSocket, buffer, 1024,0))== -1) {
					                        perror("recv");
					                        exit(1);
					                }
					                else if (num == 0) {
					                        printf("Connection closed\n");
					                        break;
					                }
					                buffer[num] = '\0';
					                printf("\nMessage Received From user: %s\n",buffer);

					                printf("\nUser: Enter Data: ");
						        	fgets(buffer,MAXSIZE-1,stdin);
					                if ((send(createdSocket,buffer, strlen(buffer),0))== -1) 
					                {
					                     fprintf(stderr, "Failure Sending Message\n");
					                     close(createdSocket);
					                     break;
					                }

					               		printf("\nMessage sent to user: %s\n",buffer);

							        } 

					        close(createdSocket);
					        wait(&child);

		           		}

			  		}
			  	}

	  		else{
	  			printf("\nUser not registered with super node.");
	  		}
 		
 	}
}
	
 else{

  	printf("socket creation failed");

 	}

 return 0;
	


}

//method for authentication

int authenticateClient(int clientDescriptor){

 char username[50],password[50], response[5];
 char choice[5];

 

 printf("\nWant to Register or Login?\n");
 printf("\n1. Register\n");
 printf("\n2. Login\n");
 printf("\nPick your option\n");
 scanf("%s",choice);

  printf("\nEnter username: ");
  scanf("%s",username);
  write(clientDescriptor,username,50);
  printf("\nEnter password: ");
  scanf("%s",password);
  write(clientDescriptor,password,50);
  write(clientDescriptor,choice,5);
  read(clientDescriptor,response,5);

  return atoi(response);

}

//method for registration

int registerWithSuperNode(){

	 int opt = 1;
	 struct sockaddr_in otherUserAddress;
	 int i,otherUserSocket;
	 int readBytes,writeBytes;
	 char ipAddress[50],portNumber[50],buffer[50];
	  int superNodePort; 
	  char choice[5];

	printf("\nRegister with Super Node\n");
	  		printf("\nEnter super node port number: \n");
	  		scanf("%d",&superNodePort);


	  		memset(&otherUserAddress, 0, sizeof(otherUserAddress));
	  		otherUserAddress.sin_addr.s_addr=inet_addr("127.0.0.1");
			 otherUserAddress.sin_port=superNodePort;
			 otherUserAddress.sin_family=AF_INET;
	  		

	  		otherUserSocket=socket(AF_INET,SOCK_STREAM,0);
	  		


			  if(setsockopt(otherUserSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
			    sizeof(opt)) < 0)
			  {
			    perror("setsockopt()");
			    exit(1);
			  }

	  		printf("socket number is %d\n", otherUserSocket);

	  		if(connect(otherUserSocket,(struct sockaddr*)&otherUserAddress,sizeof(otherUserAddress))>=0){

					  printf("\nEnter IP Adress to register on super node: ");
					  scanf("%s",ipAddress);
					  send(otherUserSocket,ipAddress,strlen(ipAddress),0);
					  printf("\nEnter port to register on super node: ");
					  scanf("%s",portNumber);
					 send(otherUserSocket,portNumber,strlen(portNumber),0);			  
					  return otherUserSocket;
				}

				else{
	  			printf("\n User process not registered with super node");
	  			return 0;
	  			
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

//method for interacting other users


void interactOtherUser(char *ip, char *port){

	int opt = 1;
	struct sockaddr_in someUserAddress;
	 int i,someUserSocket,someUserPort;
	 int num;
	 char ipAddress[50],portNumber[50],buffer[MAXSIZE];


	 		memset(ipAddress,'\0', sizeof(ipAddress));
            memset(portNumber,'\0', sizeof(portNumber));

	 		strcpy(ipAddress,ip);
	 		strcpy(portNumber,port);


	  		printf("\nStarting Conversation from user at port: %s\n",portNumber);

	  		memset(&someUserAddress, 0, sizeof(someUserAddress));
	  		someUserAddress.sin_addr.s_addr=inet_addr("127.0.0.1");
			 someUserAddress.sin_port=atoi(portNumber);
			 someUserAddress.sin_family=AF_INET;
	  		

	  		someUserSocket=socket(AF_INET,SOCK_STREAM,0);
	  		


			  if(setsockopt(someUserSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
			    sizeof(opt)) < 0)
			  {
			    perror("setsockopt()");
			    exit(1);
			  }

	  		printf("\nsocket number is %d\n", someUserSocket);

	  		if(connect(someUserSocket,(struct sockaddr*)&someUserAddress,sizeof(someUserAddress))>=0){



					  	while(1) {
				        printf("\nUser: Enter Data: ");
				        fgets(buffer,MAXSIZE-1,stdin);
				        if ((send(someUserSocket,buffer, strlen(buffer),0))== -1) {
				                fprintf(stderr, "Failure Sending Message\n");
				                close(someUserSocket);
				                exit(1);
				        }
				        else {
				                printf("Message sent to user: %s\n",buffer);
				                num = recv(someUserSocket, buffer, sizeof(buffer),0);
				                if ( num <= 0 )
				                {
				                        printf("Either Connection Closed or Error\n");
				                        //Break from the While
				                        break;
				                }

				                buffer[num] = '\0';
				                printf("Message Received From user: %s\n",buffer);
				           }
				    }
				    close(someUserSocket);


					
				}

				else{
	  			printf("\nCommunication with other process user failed\n");
	  			return ;
	  			
	  		}


}



