#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

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
}

struct node *searchCard(struct node *head,struct card req){
	struct node* current = head;  // Initialize current
    while (current != NULL){
        if (current->c.cardVal == req.cardVal && current->c.cardSuite == req.cardSuite)
            return current;
        current = current->next;
    }
    return NULL;
}

struct node *addCard(struct node *head,struct card req){
	struct node *new;
	new = (struct node *)malloc(sizeof(struct node));
	new->c.cardVal = req.cardVal;
	new->c.cardSuite = req.cardSuite;
	new->next = NULL;
	
	if(head==NULL)
		head = new;
	else{
		struct node* current = head;
		while(current->next!=NULL)
			current = current->next;
		current->next = new;
	}
	
	return head;
}



int main(){
  int clientSocket,action;
  char buffer[1024],playerName[32],c;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  struct card reqCard;

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  addr_size = sizeof serverAddr;
  
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  recv(clientSocket, buffer, 1024, 0);
  printf("%s\n",buffer); 			//ask name,team
  memset(buffer, '\0', sizeof(buffer));
  
  scanf("%s",buffer);
  send(clientSocket, buffer, sizeof(buffer), 0);
  memset(buffer, '\0', sizeof(buffer));

  recv(clientSocket, buffer, 1024, 0);
  printf("%s\n",buffer);			//wait msg
  memset(buffer, '\0', sizeof(buffer));
  
  recv(clientSocket, buffer, 1024, 0);
  printf("%s\n",buffer);			//game start msg
  memset(buffer, '\0', sizeof(buffer));
  
  while(1){
  	recv(clientSocket, &action, 1024, 0);
  	switch (action){
  		case 1:
  		printf("Your Turn\n");
  		recv(clientSocket, buffer, 1024, 0);
  		printf("%s\n",buffer);			//cards in players hand
  		memset(buffer, '\0', sizeof(buffer));
  		
  		printf("claim LITT?\nRespond with 'y' or 'n'.");
  		scanf("%c",c);
  		send(clientSocket, &c, sizeof(c), 0);
  		if(c!='Y'&&c!='y'){
  			printf("Name the player followed by card\n");
  			scanf("%s",playerName);
  			send(clientSocket, playerName, sizeof(playerName), 0);
  			scanf("%d,%d",&reqCard.cardVal,&reqCard.cardSuite);
  			send(clientSocket, reqCard, sizeof(reqCard), 0);
  		}
  		else{
  			while(c=='Y'||c=='y'){
  				printf("Name the player followed by card\n");
  				scanf("%s",playerName);
  				send(clientSocket, playerName, sizeof(playerName), 0);
  				scanf("%d,%d",&reqCard.cardVal,&reqCard.cardSuite);
  				send(clientSocket, reqCard, sizeof(reqCard), 0);
  				recv(clientSocket, &c, sizeof(c),0);
  			}
  		}
  		break;
  		
  		case 2:
  		recv(clientSocket, buffer, 1024, 0);
  		printf("%s\n",buffer);			//broadCast msg
  		memset(buffer, '\0', sizeof(buffer));
  		break;
  	}
  }
  
  return 0;
}
