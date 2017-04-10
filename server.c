#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

enum value{
ace = 1, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king
};

enum suite{
clubs = 1, spades, diamonds, hearts
};

struct card{
	int num;
	enum value cardVal;
	enum suite cardSuite;
};

struct node{
	struct card c;
	struct node *next;
};

struct player{
		int num_of_cards;
		int player_score;
		struct node *hand;
		char team;
		char username[32];
        int player_fd;
};

struct team{
		int num_of_players;
		int team_score;
		struct player *arr[];
};

struct player Player[8];

void printCards(struct node *n){

	printf("-----Printing Cards------\n");
	while(n!=NULL){
		printf("num = %d,suite = %u, val = %u\n",n->c.num,n->c.cardSuite,n->c.cardVal);
		n = n->next;
	}
	printf("-----All Cards Printed------\n");
}

int searchCard(struct node *head,struct card req){
	struct node* current = head;
	//struct node* parent;  // Initialize current
    while (current != NULL){
        if (current->c.cardVal == req.cardVal && current->c.cardSuite == req.cardSuite)
            return 1;
        current = current->next;
    }
    return 0;
}

struct node *addCard(struct node *head,struct card req){
	struct node *new;
	new = (struct node *)malloc(sizeof(struct node));
	new->c.num = req.num;
	new->c.cardVal = req.cardVal;
	new->c.cardSuite = req.cardSuite;
	new->next = NULL;
	
	if(head==NULL){
		printf("head was null\n");
		head = new;
	}
	else{
		struct node* current = head;
		while(current->next!=NULL)
			current = current->next;
		current->next = new;
	}
	//printCards(head);
	return head;
}

struct node *removeCard(struct node *head,struct card req){
	struct node* current = head;
	struct node* parent;  // Initialize current
    while (current != NULL){
        if (current->c.cardVal == req.cardVal && current->c.cardSuite == req.cardSuite){
        	if(current==head)
        		head = head->next;
        	else
        		parent->next = current->next;
        }
       	parent = current;
        current = current->next;
    }
    return head;
}

void displayHand(int i){
	char listOfCards[1024];
	listOfCards[0]=':';
	listOfCards[1]='\0';
	struct node *current = Player[i].hand;
	char oneCard[8];
	while(current!=NULL){
		sprintf(oneCard,"%d-%d,",current->c.cardVal,current->c.cardSuite);
		strcat(listOfCards,oneCard);
		current = current->next;
	}
	//printf("hand=%s\n",listOfCards);
	send(Player[i].player_fd,listOfCards,strlen(listOfCards),0);
}

int main(){
	
	srand(time(0));
   //Fixed Strings for communication
    char client_connected_message[] = "Welcome to LITT!!\nEnter UserName,team A or B";
    char game_start_message[] = "Game is Starting\nDealing cards to each player";
    char wait_msg[] = "Waiting for other players to connect...";
    
    
//Server Variables
    
    int numOfClientConnected = 0;
//Connection Variables
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
	
	int socket_fd;
	int portnum = 12565;
	
    if( (socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket failed");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");;
    serv_addr.sin_port = htons(portnum);
	
	
    if(bind(socket_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0){
        perror("Failed to bind");
        return -1;
    }
    else{
        printf("Server bound at %d port\n",portnum);
    }
	
    if(listen(socket_fd, 8) < 0){
        perror("Failed to listen");
        return -1;
    }
    else{
        printf("Server Listening at %d port\n",portnum);
    }
    
    
    //Player structure Initialisation
    struct team A,B;
    A.num_of_players = 0;
    B.num_of_players = 0;
    char buffer[1024];
    int buflen;
    
    while(numOfClientConnected < 8){
        //Client structure Initialisation
        unsigned int clilen = sizeof(struct sockaddr_in);
        struct   sockaddr_in client;
        
        Player[numOfClientConnected].player_fd = accept(socket_fd,(struct sockaddr*)&client,&clilen);
        
        send(Player[numOfClientConnected].player_fd,client_connected_message,strlen(client_connected_message),0);
        printf("Client connected with IP : \n");
        
        buflen = recv(Player[numOfClientConnected].player_fd,buffer,sizeof(buffer),0);	//recv username and team and store info
        buffer[buflen] = '\0';
        
       // printf("%s\n",buffer);
        
        strncpy(Player[numOfClientConnected].username,buffer,buflen-2);
        Player[numOfClientConnected].username[buflen-2]='\0';
        
        printf("username is : %s\n",Player[numOfClientConnected].username);
        Player[numOfClientConnected].team = buffer[buflen-1];
        
        memset(buffer,'\0',sizeof(buffer));
        
        Player[numOfClientConnected].player_score = 0;					//initialize
        Player[numOfClientConnected].num_of_cards = 0;
        Player[numOfClientConnected].hand = NULL;
                printf("before wait msg,%d,%d\n",A.num_of_players,B.num_of_players);
        
   /*     if(Player[numOfClientConnected].team == 'A')					//add player to team
        	A.arr[A.num_of_players++] = &Player[numOfClientConnected];
        else if(Player[numOfClientConnected].team == 'B')
        	B.arr[B.num_of_players++] = &Player[numOfClientConnected];*/
        
        printf("before wait msg,%d,%d\n",A.num_of_players,B.num_of_players);
        send(Player[numOfClientConnected].player_fd,wait_msg,strlen(wait_msg),0);
        
        numOfClientConnected++;
    }
      
    for(int i = 0;i < 8;i++){
        send(Player[i].player_fd,game_start_message,strlen(game_start_message),0);
    }
    
    for(int i = 1;i < 53;i++){				//initial distribution of cards
    	if(i > 24 && i < 29)
    		continue;
       	struct card c;
    	c.num = i;
    	c.cardSuite = i % 4;
    	if(c.cardSuite == 0)
    		c.cardSuite = 4;
    	c.cardVal = ((c.num - c.cardSuite)/4) + 1;
    	    	
    	int j = (rand()) % 8;
    	while(Player[j].num_of_cards == 6)
    		j = (rand()) % 8;
    	printf("num = %d,suite = %u, val = %u, Player = %d,card num = %d\n",c.num,c.cardSuite,c.cardVal,j,Player[j].num_of_cards+1);
    	
    	Player[j].hand = addCard(Player[j].hand,c);
    	Player[j].num_of_cards++;
    }
    
    for(int i=0;i<8;i++)
    	displayHand(i);
    	
    int turnOf = (rand()) % 8;
    printf("%d",turnOf);
    int action,claiming;
    char playerAsked[32],bc_msg[32];
    char lastmove[32] = "The last move was a ";
    char hit[8] = "HIT\n";
    char miss[8] = "MISS\n";
    struct card askedCard;
    
    while(1){
    	action = 1;
    	send(Player[turnOf].player_fd,&action,sizeof(action),0);
    	displayHand(turnOf);
    	recv(Player[turnOf].player_fd,&claiming,sizeof(claiming),0);
    	printf("%c\n",claiming);
    	if(claiming == 'y'||claiming=='Y')
    		printf("Player claiming litt\n"); 					 //write litt() fxn here
    	else{													//litt not claimed
    		buflen = recv(Player[turnOf].player_fd,&playerAsked,sizeof(playerAsked),0);
    		playerAsked[buflen] = '\0';
    		recv(Player[turnOf].player_fd,&askedCard.cardVal,sizeof(askedCard.cardVal),0);
    		recv(Player[turnOf].player_fd,&askedCard.cardSuite,sizeof(askedCard.cardSuite),0);
    	}
    	
    	sprintf(bc_msg,"%s has asked %s for card %d-%d",Player[turnOf].username,playerAsked,askedCard.cardVal,askedCard.cardSuite);
    	bc_msg[strlen(bc_msg)]='\0';
    	action = 2;
    	
    	printf("Before sending broadcast\n");
    	
    	for(int i=0;i<8;i++){
    		if(i==turnOf)
    			continue;
    			
    		send(Player[i].player_fd,&action,sizeof(action),0);
    		displayHand(i);
    		send(Player[i].player_fd,&bc_msg,sizeof(bc_msg),0);
    	}
    	
    	printf("Before identigying playerAsked\n");
    	
    	int i;
    	for(i=0;i<8;i++){
    		if(strcmp(playerAsked,Player[i].username)==0)
    			break;
    	}												// i gives position of 'playerasked' in array
    	
    	printf("Before hit and miss\n");
    	
    	if(searchCard(Player[i].hand,askedCard)==0){	//miss
    		printf("inside miss loop\n");
    		turnOf = i;
    		for(int i=0;i<8;i++){
    			send(Player[i].player_fd,&action,sizeof(action),0);
    			send(Player[i].player_fd,&lastmove,sizeof(lastmove),0);
    			send(Player[i].player_fd,&miss,sizeof(miss),0);
    		}
    	}
    	else{ 											//hit
    		printf("inside hit loop\n");
    		Player[i].hand = removeCard(Player[i].hand,askedCard);
    		Player[turnOf].hand = addCard(Player[turnOf].hand,askedCard);
    		Player[turnOf].player_score++;
    		
    		for(int i=0;i<8;i++){
    			send(Player[i].player_fd,&action,sizeof(action),0);
    			send(Player[i].player_fd,&lastmove,sizeof(lastmove),0);
    			send(Player[i].player_fd,&hit,sizeof(hit),0);
    		}
    	}
    }
    
    
    return 0;
}
