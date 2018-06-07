
/*

This is a multi-process server that accepts connections from user processes and either create a user and save it in a file, or lookup a pre-existing user. 
The idea is to store this information in a file so it persists across runs of this server. 
The only information that is stored is two text fields (username, and password) per user.

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
#define true 1
#define false 0


void *checkClient(void *);
int registerClient(char [],char []);
int loginClient(char [],char []);




int main(int argc, char * argv[])
{
    struct sockaddr_in address;
    int sd,i,cd;
    int serverPort;

    pthread_t clients[MAX];

//checking for correct arguments

    if (argc != 2){
        
        printf("Wrong Input.\n");
        printf("Kindly Enter Correct Values.\n");
        exit(0);
        
    }

    serverPort = atoi(argv[1]);
    if(serverPort <= 0 || serverPort > 65535){
        printf("Please enter correct port\n" );
        exit(1);
    }
    
    sd = socket(AF_INET,SOCK_STREAM,0);

    address.sin_addr.s_addr=inet_addr("127.0.0.1");
    address.sin_port=serverPort;
    address.sin_family=AF_INET;

    printf("\nSocket Created\n" );
    if(bind(sd,(struct sockaddr*)&address,sizeof(address))>=0)
    {
        printf("\nBind Successfull\n" );

        printf("\nServer is listening on port %d\n",serverPort);
        
        
        if(listen(sd,0)<0){
        	perror("Error in listening on port");
            exit(3);
        }

       //listening for user processes

        while(1)       
        {
            for(i=0;i<MAX;i++)      
            {
                cd=accept(sd,0,0);
                printf("\nConnected to client on socket on %d\n",cd);

                // Creating new threads to handle user processes

                pthread_create(&clients[i],NULL,checkClient,(void *)cd);
            }
        }
        close(cd);
    } else {
        printf("\nsocket creation failed");
    }
    return 0;
}


//method to register or login user processes

void *checkClient(void *clientDescriptor)
{
    int cd=(int)clientDescriptor;
    char username[50];
    char password[50];
    char choice[5];
    int option;
    int status;
    
    read(cd,username,50);
    printf("\nUsername from Client %d is : %s",cd,username);
    read(cd,password,50);
    printf("\nPassword from Client %d is : %s",cd,password);
    read(cd,choice,5);
    printf("\n Option selected  is : %s",choice);
    
    option = atoi(choice);

    switch (option){
    	case 1 : 
    	status = registerClient(username,password);
    	write(cd,"2",5);
    	
    	break;

    	case 2 : 
    	status = loginClient(username,password);
    	if(status == 1){
    		write(cd,"1",5);
    	} 
    	else{
    		write(cd,"0",5);
    	}   
    		
    	break;

    	default:
    	printf("Invalid Selection\n");


    }

    	
    
    

    close(cd);
}

// method to register userv procresses

int registerClient(char userName[],char passWord[]){

FILE *fp;
fp = fopen("loginDetails.txt", "a+");
if(fp == NULL){
        printf("Not able to open file");
        exit(1);
    }

    fprintf(fp,"%s%s\n",userName,passWord);
    fclose(fp);


    return 2;

}

//method to login user processes

int loginClient(char userName[],char passWord[]){
	FILE *fp;
	char input[100];
	char temp[100];
	fp = fopen("loginDetails.txt", "r");
	if(fp == NULL){
	        printf("Not able to open file");
	        exit(1);
	    }

	    strcpy(temp, userName);
    	strcat(temp, passWord);

    	printf("\nprinting value: %s", temp);

	    while(fgets(input,100,fp)!= NULL){
	    	if(strstr(input, temp) != NULL){

	    		return true;

			}
	    }

	   return false;
}










































