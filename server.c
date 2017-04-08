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


struct player{
		int num_of_cards;
		int player_score;
		struct cards array[];
		char team;
		char username[];
        int player_fd;
};

struct team{
		int num_of_players;
		int team_score;
		struct player arr[];
};


int main(){
	
   //Fixed Strings for communication
    char client_connected_message[] = "Welcome to LITT!!";
    char game_start_message[] = "Game is Starting";
    
    
//Server Variables
    
    int numOfClientConnected = 0;
//Connection Variables
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
	
	int socket_fd;
	int portnum = 12567;
	
    if( (socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket failed");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.0.1");;
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
    struct player Player[8];
    
    
    while(numOfClientConnected < 8){
        //Client structure Initialisation
        struct   sockaddr_in client;
        Player[numOfClientConnected].player_fd = accept(socket_fd,(struct sockaddr*)&client,&clilen);
        send(Player[numOfClientConnected].player_fd,client_connected_message,strlen(client_connected_message),0);
        numOfClientConnected++;
    }
    
    for(int i = 0;i < 8;i++){
        send(Player[i].player_fd,game_start_message,strlen(game_start_message),0);
    }
    return 0;

}
