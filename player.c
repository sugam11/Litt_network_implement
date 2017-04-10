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
  serverAddr.sin_port = htons(12559);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  addr_size = sizeof serverAddr;
  
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  recvlen=recv(clientSocket, buffer, 1024, 0);
  buffer[recvlen] = '\0';
  printf("%s\n",buffer); 			//ask name,team
  memset(buffer, '\0', sizeof(buffer));
  
  scanf("%s",buffer);
  send(clientSocket, buffer, strlen(buffer), 0);
  memset(buffer, '\0', sizeof(buffer));

  recvlen=recv(clientSocket, buffer, 1024, 0);
  buffer[recvlen] = '\0';
  printf("%s\n",buffer);			//wait msg
  memset(buffer, '\0', sizeof(buffer));
  
  recvlen=recv(clientSocket, buffer, 1024, 0);
  buffer[recvlen] = '\0';
  printf("%s\n",buffer);			//game start msg
  memset(buffer, '\0', sizeof(buffer));
  
  recvlen=recv(clientSocket, buffer, 1024, 0);
  buffer[recvlen] = '\0';
  printf("%s\n",buffer);			//INITIAL hand
  memset(buffer, '\0', sizeof(buffer));
  
  while(1){
  	recv(clientSocket, &action, 1024, 0);
  	switch (action){
  		case 1:
  		printf("Your Turn\n");
  		recvlen=recv(clientSocket, buffer, 1024, 0);
  		buffer[recvlen] = '\0';
  		printf("%s\n",buffer);			//cards in players hand
  		memset(buffer, '\0', sizeof(buffer));
  		
  		printf("claim LITT?\nRespond with 'y' or 'n'.");
  		scanf("%c",&c);
  		send(clientSocket, &c, sizeof(c), 0);
  		if(c!='Y'&&c!='y'){											//litt not claimed
  			printf("Name the player followed by card\n");
  			scanf("%s",playerName);
  			send(clientSocket, playerName, strlen(playerName), 0);
  			scanf("%u,%u",&reqCard.cardVal,&reqCard.cardSuite);
  			send(clientSocket, &reqCard.cardVal, sizeof(reqCard.cardVal), 0);
  			send(clientSocket, &reqCard.cardSuite, sizeof(reqCard.cardSuite), 0);
  		}
  		else{
  			while(c=='Y'||c=='y'){									//litt claimed
  				printf("Name the player followed by card\n");
  				scanf("%s",playerName);
  				send(clientSocket, playerName, strlen(playerName), 0);
  				scanf("%u,%u",&reqCard.cardVal,&reqCard.cardSuite);
  				send(clientSocket, &reqCard.cardVal, sizeof(reqCard.cardVal), 0);
  				send(clientSocket, &reqCard.cardSuite, sizeof(reqCard.cardSuite), 0);
  				recv(clientSocket, &c, sizeof(c),0);
  			}
  		}
  		break;
  		
  		case 2:
  		recvlen=recv(clientSocket, buffer, 1024, 0);
  		buffer[recvlen] = '\0';
  		printf("%s\n",buffer);			//cards in players hand/last move msg
  		memset(buffer, '\0', sizeof(buffer));
  		
  		recvlen=recv(clientSocket, buffer, 1024, 0);
  		buffer[recvlen] = '\0';
  		printf("%s\n",buffer);			//broadCast msg/ hit miss msg
  		memset(buffer, '\0', sizeof(buffer));
  		break;
  	}
  }
  
  return 0;
}
