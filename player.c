#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>

enum value{
ace = 1, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king
};

char value[13][10] = {"ace","two","three","four","five","six","seven","eight","nine","ten","jack","queen","king"};

char suit[4][15] = {"club","spade","diamonds","hearts"};

enum suite{
clubs = 1, spades, diamonds, hearts
};

struct card{
	enum value cardVal;
	enum suite cardSuite;
};

struct node{
	struct card c;
	struct node *next;
};

void displayHand(char msg[]){
    int i = 1;
    while(i < strlen(msg)){
        if(msg[i] >= '0' && msg[i] <= '9'){
            if(msg[i+1] == '-') {
                printf("%s - %s , ",value[msg[i] - '0' - 1],suit[msg[i+2] - '0' - 1]);
                i = i + 2;
            }
        }
        i++;
    }
    printf("\n");
}

int main(int argc,char **argv){
  int clientSocket,action,recvlen;
  char buffer[1024],playerName[32],c;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  struct card reqCard;

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(atoi(argv[2]));
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
  
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  addr_size = sizeof(serverAddr);
  
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  buffer[recvlen] = '\0';
  printf("%s",buffer); 			//ask name,team
  memset(buffer, '\0', sizeof(buffer));
  fflush(stdout);
  
  while(1)
  {
          scanf("%s",buffer);
          if(buffer[strlen(buffer)-2]==',' && ( buffer[strlen(buffer)-1]=='A' || buffer[strlen(buffer)-1]=='B') )
          {
              break;
          }
          else
            printf("Please enter in the format: username,A or username,B only(without any spaces)\n");
              
  }

 char msg[1024];
  send(clientSocket, buffer, strlen(buffer), 0);
  memset(buffer, '\0', sizeof(buffer));

  recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  buffer[recvlen] = '\0';
  printf("%s",buffer);			//wait msg
  memset(buffer, '\0', sizeof(buffer));
  fflush(stdout);
  
  recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  buffer[recvlen] = '\0';
  printf("%s",buffer);			//game start msg + initial hand
    strncpy(msg,buffer+2,strlen(buffer)-2);
    displayHand(msg);
  memset(buffer, '\0', sizeof(buffer));
  fflush(stdout);
  
  /*recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  buffer[recvlen] = '\0';
  printf("cards dealt -> %s,%d\n",buffer,recvlen);			//INITIAL hand
  memset(buffer, '\0', sizeof(buffer));
  fflush(stdout);*/
  
 
  while(1){
  	//printf("While loop entered\n");
  	memset(msg,'\0',sizeof(msg));
  	//printf("before recv\n");
  	if((recvlen = recv(clientSocket, &buffer, sizeof(buffer), 0))<0){
  	perror("Failed to bind");
        return -1;
  	}
  	//printf("after recv,recvlen=%d\n",recvlen);
  	buffer[recvlen] = '\0';
  	action = buffer[0] - '0';
  	strncpy(msg,buffer+2,strlen(buffer)-2);
  	printf("received = %s\naction = %d\n",buffer,action);
      short suit_flag = 0;
      for (int i = 0; i < strlen(msg); i++) {
          if(msg[i] >= '0' && msg[i] <= '9'){
              if(suit_flag == 0){
                  printf("Suit is : %s  & ",suit[msg[i] - '0' - 1]);
                  suit_flag = 1;
              }
              else{
                  printf("Card is : %s \n",value[msg[i] - '0' - 1]);
                  suit_flag = 0;
              }
          }
      }
    displayHand(msg);
  	memset(buffer, '\0', sizeof(buffer));
  	
  	switch (action){
  		case 1:
  		//printf("case 1 entered\n");
  		printf("Your Turn\n");
  		//recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  		//buffer[recvlen] = '\0';
            displayHand(msg);
  		//printf("%s\n",msg);			//cards in players hand
  		
  		
  		printf("claim LITT?\nRespond with 'y' or 'n'.\n");
  		while(1)
          {
              scanf(" %c",&c);
              if(c!='Y'&&c!='y'&&c!='n'&&c!='N')
              {
                  printf("Respond with 'y' or 'n' only\n");
              }
              else
                  break;
          }
  		send(clientSocket, &c, sizeof(c), 0);
  		if(c!='Y'&&c!='y'){											//litt not claimed
  			printf("Name the player followed by card as playerName(space)Card Value, Card Suite\n");
  			scanf("%s %u,%u",playerName,&reqCard.cardVal,&reqCard.cardSuite);
  			sprintf(buffer,"%s,%d,%d",playerName,reqCard.cardVal,reqCard.cardSuite);
  			send(clientSocket, buffer, strlen(buffer), 0);
  			memset(buffer, '\0', sizeof(buffer));
  		}
  		else{
  			int count=0;
  			while(c=='Y'||c=='y'){									//litt claimed
  				count++;
  				printf("Name the player followed by card as playerName(space)Card Value, Card Suite\n");
  				scanf("%s %u,%u",playerName,&reqCard.cardVal,&reqCard.cardSuite);
  				sprintf(buffer,"%s,%u,%u",playerName,reqCard.cardVal,reqCard.cardSuite);
  				send(clientSocket, buffer, strlen(buffer), 0);
  				memset(buffer, '\0', sizeof(buffer));
  				//memset(playerName, '\0', sizeof(playerName));
  				recv(clientSocket, &c, sizeof(c),0);
  				printf("  c=%c\n",c);
  				if(count == 6)
  					break;
  			}
  		}
  		break;
  		
  		case 2:
  		//recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  		//buffer[recvlen] = '\0';
  		printf("%s\n",msg);			//cards in players hand+bc_msg/last move msg
  		memset(buffer, '\0', sizeof(buffer));
  		break;

      case 3:
          printf("you have been busted. Low score. No entry to next level");
          fflush(stdout);
          memset(buffer, '\0', sizeof(buffer));
          close(clientSocket);
          exit(0);
      case 4:
            printf("%s\n",msg);			//cards in players hand+bc_msg/last move msg
            printf("Clearing History of moves\n");
            if(fork() == 0){
                execlp("clear","clear");
            }
            memset(buffer, '\0', sizeof(buffer));
            break;

            
  	}
  }
  
  return 0;
}
