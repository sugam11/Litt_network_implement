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
		int player_score; //-10 if out
		struct node *hand;
		char team;
		char username[32];
        int player_fd;
        int active;
};

struct team{
		int num_of_players;
		int team_score;
		struct player *arr[];
};

struct player Player[8];
char str[1024];
char teams[2];
int team_score[2];

//returns the player id(i) of the minimum score player in the team.
int min_team(char team){
    int min=100000, min_player=-1;
    for(int i=0; i<8; i++){
        if(Player[i].team == team && Player[i].player_score != -10)
            if(Player[i].player_score < min){
                min = Player[i].player_score;
                min_player = i;
            }
    }
    return min_player;
}

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
	printf("searching card\n");
    while (current != NULL){
        if (current->c.cardVal == req.cardVal && current->c.cardSuite == req.cardSuite)
        //if(current->c.cardVal == 0 && current->c.cardSuite == 0)
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
		//printf("head was null\n");
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
    parent->next = NULL;
    while (current != NULL){
        if (current->c.cardVal == req.cardVal && current->c.cardSuite == req.cardSuite){
        //if (current->c.cardVal == 0 && current->c.cardSuite == 0){
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
	
	memset(str,'\0',sizeof(str));
	int len,total=0;
	//char str[1024];
	str[0]=':';
	str[1]='\0';
	struct node *current = Player[i].hand;
	char oneCard[8];
	while(current!=NULL){
		len = sprintf(oneCard,"%d-%d,",current->c.cardVal,current->c.cardSuite);
		total+=len;
		strcat(str,oneCard);
		current = current->next;
	}
	
	str[total] = '\n';
	str[total+1] = '\0';
	//printf("total=%d\n,displayhand=%s\n",total,str);
	//send(Player[i].player_fd,str,strlen(str),0);
}

    //function to remove two players of each team ( LEVEL IMPLEMENTATION )
    void remove_two_players(){
        //1. GET two min players.
        int min_0=100000, min_1=100000, min_player_0, min_player_1;
        for(int i =0; i<8 ; i++){
            if(Player[i].team == teams[0]){
                if(Player[i].player_score < min_0){
                    min_0 = Player[i].player_score;
                    min_player_0 = i;
                }
            }
            else if(Player[i].team == teams[1]){
                if(Player[i].player_score < min_1){
                    min_1 = Player[i].player_score;
                    min_player_1 = i;
                }
            }
        }
        Player[min_player_0].player_score = 100001;
        Player[min_player_1].player_score = 100001;

        //redistributing team 0's min player's cards to his/her team members.
        struct node * current = Player[min_player_0].hand;

        while(current){
            int player = (rand()) % 8;
            if(Player[player].team != teams[0] && Player[player].active && player != min_player_0)
                continue;
            Player[player].hand = addCard(Player[player].hand, current->c);
            struct node * temp = current->next;
            current->next = Player[player].hand;
            Player[player].hand = current;
            current = temp;
        }

        //redistributing team 1's min player's cards to his/her team members.
        current = Player[min_player_1].hand;
        while(current){
            int player = (rand()) % 8;
            if(Player[player].team != teams[1] && Player[player].active && player != min_player_1)
                continue;
            Player[player].hand = addCard(Player[player].hand, current->c);
            struct node * temp = current->next;
            current->next = Player[player].hand;
            Player[player].hand = current;
            current = temp;
        }

        Player[min_player_0].hand = NULL;
        Player[min_player_1].hand = NULL;
        
        Player[min_player_0].active = 0;
        Player[min_player_1].active = 0;
        char buffer[1024];
        int buflen;
        int action = 4;

        //broad cast this to all
        for(int i=0;i<8;i++){
                            if(Player[i].active == 0)
                                continue;
                     
                            sprintf(buffer,"%d,Players - %s of team %c and %s of team %c have been removed. Level UP !\n", action, Player[min_player_0].username, 
                                 teams[0], Player[min_player_1].username, teams[1]);
                            //printf("buffer=%s\n",buffer);
                            send(Player[i].player_fd,buffer,strlen(buffer),0);
                            memset(buffer, '\0', sizeof(buffer));
        }

        //broadcast quit message to removed players
        char * kick_msg = "3,kick";
        send(Player[min_player_0].player_fd,kick_msg,strlen(kick_msg),0);
        send(Player[min_player_1].player_fd,kick_msg,strlen(kick_msg),0);
        close(Player[min_player_0].player_fd);
        close(Player[min_player_1].player_fd);
    }

int main(){
	team_score[0] = team_score[1] = 0;
	srand(time(0));
   //Fixed Strings for communication
    char client_connected_message[64] = "Welcome to LITT!!\nEnter UserName,team A or B\n";
    char game_start_message[64] = "Game is Starting.\nDealing cards to each player...\n";
    char wait_msg[64] = "Waiting for other players to connect...\n";
    
    int count_litts_claimed = 0;
//Server Variables
    
    int numOfClientConnected = 0;
//Connection Variables
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
	
	int socket_fd;
	int portnum = 12690;
	
    if( (socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket failed");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
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

    teams[0] = '9';
    teams[1] = '9';
    int team_count = 0;
    char prev_team = '0';
    
    while(numOfClientConnected < 8){
        //Client structure Initialisation
        unsigned int clilen = sizeof(struct sockaddr_in);
        struct   sockaddr_in client;
        
        Player[numOfClientConnected].player_fd = accept(socket_fd,(struct sockaddr*)&client,&clilen);
        
        send(Player[numOfClientConnected].player_fd,client_connected_message,strlen(client_connected_message),0);					//asking name,team
        printf("Client connected with IP : %s\n",inet_ntoa(client.sin_addr));
        
        buflen = recv(Player[numOfClientConnected].player_fd,buffer,sizeof(buffer),0);	//recv username and team and store info
        buffer[buflen] = '\0';
        
       // printf("%s\n",buffer);
        
        strncpy(Player[numOfClientConnected].username,buffer,buflen-2);
        Player[numOfClientConnected].username[buflen-2]='\0';
        
        printf("username is : %s\n",Player[numOfClientConnected].username);
        Player[numOfClientConnected].team = buffer[buflen-1];
        Player[numOfClientConnected].active = 1;

        //validating teams
        if(team_count < 3){
            int flag = 0;
            for(int i = 0; i<2; i++)
                if(buffer[buflen-1] == teams[i])
                    flag = 1;
            if(flag == 0)
                if(team_count < 2)
                    teams[team_count++] = buffer[buflen-1];
                else 
                    printf("----------------------ERROR : more than 2 teams----------------------");
        }

        
        memset(buffer,'\0',sizeof(buffer));
        
        Player[numOfClientConnected].player_score = 0;					//initialize
        Player[numOfClientConnected].num_of_cards = 0;
        Player[numOfClientConnected].hand = NULL;

        send(Player[numOfClientConnected].player_fd,wait_msg,strlen(wait_msg),0);			//sending wait msg
        
        numOfClientConnected++;
    }

    int action_here = 2;
    char buffer_new[100];
    sprintf(buffer,"%d,All the players are - \n",action_here);
    for(int j=0; j<8; j++) {                      
        sprintf(buffer_new,"%s , team - %c\n",Player[j].username, Player[j].team);
        strcat(buffer, buffer_new);
    }
    for(int i=0;i<8;i++){
                        
                            
                            
                            //printf("buffer=%s\n",buffer);
                            send(Player[i].player_fd,buffer,strlen(buffer),0);
                            //memset(buffer, '\0', sizeof(buffer));
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
    	//printf("num = %d,suite = %u, val = %u, Player = %d,card num = %d\n",c.num,c.cardSuite,c.cardVal,j,Player[j].num_of_cards+1);
    	
    	Player[j].hand = addCard(Player[j].hand,c);
    	Player[j].num_of_cards++;
    }
    
    //char *str;
    sleep(2);
    
    for(int i = 0;i < 8;i++){
    	if(Player[i].active == 0)
            continue;
    	displayHand(i);
    	
    	/*if(Player[i].team == 'A')
    		A.arr[A.num_of_players++] = &Player[i];
        else if(Player[i].team == 'B')
        	B.arr[B.num_of_players++] = &Player[i];*/
        char msgbuf[1024];
        strcpy(msgbuf,game_start_message);
        strcat(msgbuf,str);	
        send(Player[i].player_fd,msgbuf,strlen(msgbuf),0);		//game start msg + initial hand
        memset(msgbuf,'\0',sizeof(msgbuf));
        //free(str);
    }
    	
    int turnOf = (rand())%8;
    while(!Player[turnOf].active)
        turnOf = (rand())%8;
    int action;
    char claiming;
    char playerAsked[32],bc_msg[32];
    char lastmove[32] = "The last move was a ";
    char hit[8] = "HIT\n";
    char miss[8] = "MISS\n";
    struct card askedCard;
    
    
    
    while(1){
    	sleep(2);
    	memset(playerAsked, '\0', sizeof(playerAsked));
    	memset(bc_msg, '\0', sizeof(bc_msg));
        
        for(int i=0;i<8;i++){
                            if(Player[i].active == 0)
                                continue;
                            int action_now = 2;
                                                        
                            sprintf(buffer,"%d,Your score - %d\n", Player[i].player_score);
                            //printf("buffer=%s\n",buffer);
                            send(Player[i].player_fd,buffer,strlen(buffer),0);
                            memset(buffer, '\0', sizeof(buffer));
        }

    	printf("turnOf=%d\n",turnOf);
    	action = 1;
    	displayHand(turnOf);
    	sprintf(buffer,"%d,%s",action,str);
    	send(Player[turnOf].player_fd,buffer,sizeof(buffer),0);
    	memset(buffer, '\0', sizeof(buffer));
    	
    	recv(Player[turnOf].player_fd,&claiming,sizeof(claiming),0);
    	printf("claiming=%c\n",claiming);
    	
    	if(claiming == 'y'||claiming=='Y'){						//write litt() fxn here
    		int count=0;
            int flag = 0;
    		struct card setCards[6];
    		int playerSlot[6];
    		
    		printf("Player claiming litt\n");
    		while(1){
    			buflen = recv(Player[turnOf].player_fd,&buffer,sizeof(buffer),0);
    			buffer[buflen] = '\0';
    			//printf("-----buffer=%s,buflen=%d\n",buffer,buflen);
    			int l;
    			for(l=0;l<32;l++){
    				if(buffer[l]==',')
    					break;
    				playerAsked[l] = buffer[l];
    				//printf("playerAsked[l] = %c\n",playerAsked[l]);	
    			}
    			char somebuf[10];
    			strncpy(somebuf,buffer+l+1,6);
    			//printf("somebuf = %s\n",somebuf);
    			sscanf(somebuf,"%u,%u",&askedCard.cardVal,&askedCard.cardSuite);
    			memset(buffer, '\0', sizeof(buffer));
    			
    			int i;
    			for(i=0;i<8;i++){
    				if(strcmp(playerAsked,Player[i].username)==0)
    					break;
    			}

                // if(Player[turnOf].team == Player[i].team){
                //     printf("you are asking the opposite team player : ERROR\n");
                //     continue;
                // }		

                // i gives position of 'playerasked' in array
    			//printf("PLAYERaSKED = %s,i=%d\n",playerAsked,i);


    			
    			if(searchCard(Player[i].hand,askedCard)==0){	
    				printf("litt claim rejected\n");
    				claiming = 'n';
    				send(Player[turnOf].player_fd,&claiming,sizeof(claiming),0);
                    flag = 0;
                    Player[turnOf].player_score -= 3;
    				//break;
    			}
    			else{
    				printf("card found\n");
    				playerSlot[count] = i;
    				claiming = 'y';
    				setCards[count].cardVal = askedCard.cardVal;
    				setCards[count].cardSuite = askedCard.cardSuite;
    				send(Player[turnOf].player_fd,&claiming,sizeof(claiming),0);
    				count++;
                    flag = 1;

    				if(count==6){
    					printf("litt claim accepted\n");
                        Player[turnOf].player_score += 3;
                        if(Player[turnOf].team == 'A')
                            team_score[0] += 5;
                        else
                            team_score[1] += 5;
    					for(int a=0;a<6;a++)
    						removeCard(Player[playerSlot[a]].hand,setCards[a]);
    					//break;
                        flag = 2;
    				}

                        
    					
    			}

                //setting result_turn to be used in next broadcasting loop.
                char result_turn[50];
                switch(flag) {
                    case 0: strcpy(result_turn, "not found"); break;
                    case 1: strcpy(result_turn, "found"); break;
                    case 2: strcpy(result_turn, "found and litt claim successful"); break;
                }


                //BROADCASTING litt claim turns of each playerasked.
                for(int i=0;i<8;i++){
                            if(i==turnOf || Player[i].active == 0)
                                continue;
                            action = 2;
                                                        
                            sprintf(buffer,"%d,Litt Claimed : Player - %s and card - %d %d - %s",action,playerAsked, askedCard.cardSuite, 
                                askedCard.cardVal, result_turn);
                            //printf("buffer=%s\n",buffer);
                            send(Player[i].player_fd,buffer,strlen(buffer),0);
                            memset(buffer, '\0', sizeof(buffer));
                }

                //break if litt rejected or accepted.
                if(flag == 0 || flag ==2){
                    break;
                    count_litts_claimed++;
                }
                
    			memset(playerAsked,'\0',sizeof(playerAsked));
    		}

            //if(count_litts_claimed % 1 == 0){
                //int times = count_litts_claimed / 3;
                //if(times == 1 || times == 2){
                    remove_two_players();
                    //update_cards();
                //}

            //}
            turnOf = (rand())%8;
            while(!Player[turnOf].active)
    		  turnOf = (rand())%8;
    		continue;
                //}
    	} 					 
    	
    	else{												//litt not claimed
    		buflen = recv(Player[turnOf].player_fd,&buffer,sizeof(buffer),0);
    		buffer[buflen] = '\0';
    		int l;
    		for(l=0;l<32;l++){
    			if(buffer[l]==',')
    				break;
    			playerAsked[l] = buffer[l];	
    		}
    		char somebuf[10];
    		strncpy(somebuf,buffer+l+1,6);
    		sscanf(somebuf,"%u,%u",&askedCard.cardVal,&askedCard.cardSuite);
    		memset(buffer, '\0', sizeof(buffer));
    	}
    	
    	sprintf(bc_msg,"%s has asked %s for card %d-%d.",Player[turnOf].username,playerAsked,askedCard.cardVal,askedCard.cardSuite);
    	bc_msg[strlen(bc_msg)]='\0';
    	action = 2;
    	
    	//printf("Before sending broadcast\n");
    	
    	for(int i=0;i<8;i++){
    		if(i==turnOf || Player[i].active == 0)
    			continue;
    			
    		displayHand(i);
    		sprintf(buffer,"%d,%s,%s",action,str,bc_msg);
    		//printf("buffer=%s\n",buffer);
    		send(Player[i].player_fd,buffer,strlen(buffer),0);
    		memset(buffer, '\0', sizeof(buffer));
    	}
    	
    	//printf("Before identifing playerAsked\n");
    	
    	int i;
    	for(i=0;i<8;i++){
    		if(strcmp(playerAsked,Player[i].username)==0 && Player[i].active == 1 )
    			break;
    	}												// i gives position of 'playerasked' in array
    	
    	//printf("Before hit and miss\n");
    	//printf("askedCard = %u,%u\n",askedCard.cardVal,askedCard.cardSuite);
    	
    	if(searchCard(Player[i].hand,askedCard)==0){	//miss
    		//printf("inside miss loop\n");
    		turnOf = i;
    		for(int i=0;i<8;i++){
                if( Player[i].active == 1){
        			sprintf(buffer,"%d,%s%s",action,lastmove,miss);
        			send(Player[i].player_fd,buffer,strlen(buffer),0);
    				memset(buffer, '\0', sizeof(buffer));
                }
    		}
    	}
    	else{ 											//hit
    		//printf("inside hit loop\n");
    		Player[i].hand = removeCard(Player[i].hand,askedCard);
    		Player[turnOf].hand = addCard(Player[turnOf].hand,askedCard);
    		Player[turnOf].player_score++;

    		
    		for(int i=0;i<8;i++){
    			//printf("inside hit for loop\n");
                if( Player[i].active == 1){
        			sprintf(buffer,"%d,%s%s",action,lastmove,hit);
        			send(Player[i].player_fd,buffer,strlen(buffer),0);
    				memset(buffer, '\0', sizeof(buffer));
                }
    		}
    	}
    }
    
    
    return 0;
}

