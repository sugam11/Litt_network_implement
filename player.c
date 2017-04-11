#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

enum value{
ace = 1, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king
};

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

int main(){
  int clientSocket,action,recvlen;
  char buffer[1024],playerName[32],c;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  struct card reqCard;

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(12621);
  serverAddr.sin_addr.s_addr = inet_addr("172.17.46.52");
  
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  addr_size = sizeof serverAddr;
  
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  buffer[recvlen] = '\0';
  printf("ask name,team=%s\n",buffer); 			//ask name,team
  memset(buffer, '\0', sizeof(buffer));
  fflush(stdout);
  
  scanf("%s",buffer);
  send(clientSocket, buffer, strlen(buffer), 0);
  memset(buffer, '\0', sizeof(buffer));

  recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  buffer[recvlen] = '\0';
  printf("wait msg=%s\n",buffer);			//wait msg
  memset(buffer, '\0', sizeof(buffer));
  fflush(stdout);
  
  recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  buffer[recvlen] = '\0';
  printf("game start msg=%s\n",buffer);			//game start msg + initial hand
  memset(buffer, '\0', sizeof(buffer));
  fflush(stdout);
  
  /*recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  buffer[recvlen] = '\0';
  printf("cards dealt -> %s,%d\n",buffer,recvlen);			//INITIAL hand
  memset(buffer, '\0', sizeof(buffer));
  fflush(stdout);*/
  char msg[1024];
  while(1){
  	printf("While loop entered\n");fflush(stdout);
  	recvlen = recv(clientSocket, &buffer, sizeof(buffer), 0);
  	buffer[recvlen] = '\0';
  	sscanf(buffer,"%d,%s",&action,msg);
  	memset(buffer, '\0', sizeof(buffer));
  	
  	switch (action){
  		case 1:
  		printf("case 1 entered\n");
  		printf("Your Turn\n");
  		//recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  		//buffer[recvlen] = '\0';
  		printf("cards in players hand=%s\n",msg);fflush(stdout);			//cards in players hand
  		fflush(stdout);
  		
  		printf("claim LITT?\nRespond with 'y' or 'n'.\n");
  		scanf(" %c",&c);
  		send(clientSocket, &c, sizeof(c), 0);
  		if(c!='Y'&&c!='y'){											//litt not claimed
  			printf("Name the player followed by card as playerName, Card Value, Card Suite\n");fflush(stdout);
  			scanf("%s,%u,%u",playerName,&reqCard.cardVal,&reqCard.cardSuite);
  			sprintf(buffer,"%s,%d,%d",playerName,reqCard.cardVal,reqCard.cardSuite);
  			send(clientSocket, buffer, strlen(buffer), 0);
  			memset(buffer, '\0', sizeof(buffer));
  		}
  		else{
  			while(c=='Y'||c=='y'){									//litt claimed
  				printf("Name the player followed by card as playerName, Card Value, Card Suite\n");fflush(stdout);
  				scanf("%s,%u,%u",playerName,&reqCard.cardVal,&reqCard.cardSuite);
  				sprintf(buffer,"%s,%d,%d",playerName,reqCard.cardVal,reqCard.cardSuite);
  				send(clientSocket, buffer, strlen(buffer), 0);
  				memset(buffer, '\0', sizeof(buffer));
  				recv(clientSocket, &c, sizeof(c),0);
  			}
  		}
  		break;
  		
  		case 2:
  		//recvlen=recv(clientSocket, buffer, sizeof(buffer), 0);
  		//buffer[recvlen] = '\0';
  		printf("cards in players hand/last move msg=%s",msg);fflush(stdout);			//cards in players hand+bc_msg/last move msg
  		memset(buffer, '\0', sizeof(buffer));
  		break;
  	}
  }
  
  return 0;
}
