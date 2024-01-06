// Group 31
// Members: Dipak Chaudhary, Ramakrishna Valupadasu
// Current Implementation level: Socket Project Milestone

// Header file for tracker and user

#include <stdio.h>      // for printf() and fprintf()
#include <sys/socket.h> // for socket() and bind()
#include <arpa/inet.h>  // for sockaddr_in and inet_ntoa()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include <ctype.h>
#include <stdbool.h>

// Structure of handle to store information of the user
struct handle
{
    char handle_name[15];
    int port1;
    int port2;
    int port3;
    char ipAddr[15];
    struct follower* head;
    struct handle *next;
};
//Save follower name
struct follower 
{
  char follower_name[15];
  struct follower* next;
};

//Server side Handler functions for different commands
int queryhandle(char *echoBuffer);
int registerserver(char *echoBuffer, unsigned int n);
int followHandler(char *echobuffer);
int dropHandler(char *echobuffer);
int exitHandler(char *echobuffer);
void appendHandle(struct handle* handle, char *echoBuffer);
int tweetHandler(char *echoBuffer);

//User commands to transport tweets and than print tweets
void tweetFunc(char *str);
void printTweet();
