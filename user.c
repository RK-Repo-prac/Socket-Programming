// Group 31
// Members: Dipak Chaudhary, Ramakrishna Valupadasu
// Current Implementation level: Socket Project Milestone

// Implements the client side of the TWEETER program
// Gets comands from users and exits on exit command
#include "defns.h"

#define ECHOMAX 255    // Longest string to echo
#define ITERATIONS 100 // Number of iterations the client executes

void DieWithError(const char *errorMessage) // External error handling function
{
    perror(errorMessage);
    exit(1);
}
size_t nread;
    int sock;                        // Socket descriptor
    struct sockaddr_in echoServAddr; // Echo server address
    struct sockaddr_in fromAddr;     // Source address of echo
    unsigned short echoServPort;     // tracker server port
    unsigned int fromSize;           // In-out of address size for recvfrom()
    char *servIP;                    // IP address of server
    char *echoString = NULL;         // String to send to echo server
    size_t echoStringLen = ECHOMAX;  // Length of string to echo
    int respStringLen;               // Length of received response

    char *userIP;                                      // User Ip address
    int trackerPort, sendPort, rcvPort;                // Ports for communicating with the tracker and other handles
    int sock1, sock2, sock3;                           // Sockets for P2P and client-server communication
    struct sockaddr_in trackerAddr, sendAddr, rcvAddr; // Addresses to bind port-IP with sockets

        bool exitflag = false;
    bool rcv = false;
int main(int argc, char *argv[])
{
echoString = (char *)malloc(ECHOMAX);

        if (argc < 7) // Test for correct number of arguments
    {
        // Updates error message to show how many arguments are needed
        fprintf(stderr, "Usage: %s <Server IP address> <Server Port> <User Port1> <User Port2> <User Port3> <userIP> \n", argv[0]);
        exit(1);
    }

    servIP = argv[1];             // First arg: server IP address (dotted decimal)
    echoServPort = atoi(argv[2]); // Second arg: Use given port
    // Assigning ports and user IP from command line
    trackerPort = atoi(argv[3]);
    sendPort = atoi(argv[4]);
    rcvPort = atoi(argv[5]);
    userIP = argv[6];

    // printf("client: Arguments passed: server IP %s, port %d\n", servIP, echoServPort);

    // Create a datagram/UDP socket
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("client: socket() failed");

    // Creating and binding UDP socket to communicate with tracker
    if ((sock1 = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("client: socket() failed");
    // Creating local Address to bind socket with
    memset(&trackerAddr, 0, sizeof(trackerAddr));
    trackerAddr.sin_family = AF_INET;
    trackerAddr.sin_addr.s_addr = inet_addr(userIP);
    trackerAddr.sin_port = htons(trackerPort);
    // binding socket with appropriate address
    if (bind(sock1, (struct sockaddr *)&trackerAddr, sizeof(trackerAddr)) < 0)
        DieWithError("Server: bind() failed on tracker address");

    // Creating and binding UDP socket to send message to other peers
    if ((sock2 = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("client: socket() failed");
    // Creating local Address to bind socket with
    memset(&sendAddr, 0, sizeof(sendAddr));
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_addr.s_addr = inet_addr(userIP);
    sendAddr.sin_port = htons(sendPort);
    // binding socket with appropriate address
    if (bind(sock2, (struct sockaddr *)&sendAddr, sizeof(sendAddr)) < 0)
        DieWithError("Server: bind() failed on send address");

    // Creating and binding UDP socket to recieve message from other peers
    if ((sock3 = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("client: socket() failed");
    // Creating local Address to bind socket with
    memset(&rcvAddr, 0, sizeof(rcvAddr));
    rcvAddr.sin_family = AF_INET;
    rcvAddr.sin_addr.s_addr = inet_addr(userIP);
    rcvAddr.sin_port = htons(rcvPort);
    // binding socket with appropriate address
    if (bind(sock3, (struct sockaddr *)&rcvAddr, sizeof(rcvAddr)) < 0)
        DieWithError("Server: bind() failed on recieve address");

    // Construct the server address structure
    memset(&echoServAddr, 0, sizeof(echoServAddr));   // Zero out structure
    echoServAddr.sin_family = AF_INET;                // Use internet addr family
    echoServAddr.sin_addr.s_addr = inet_addr(servIP); // Set server's IP address
    echoServAddr.sin_port = htons(echoServPort);      // Set server's port

    // Pass string back and forth between server ITERATIONS times
    for (int i = 0; i < ITERATIONS; i++)
    {
      memset(echoString, 0, sizeof(echoString));
      //If recieve flag in true, print tweet
      if(rcv)
      {
        printTweet();
      }

        // Get user command as command line input
        printf("\nEnter command: \n");
        if ((nread = getline(&echoString, &echoStringLen, stdin)) != -1)
        {
            echoString[(int)strlen(echoString) - 1] = '\0'; // Overwrite newline
        }
        else
            DieWithError("client: error reading string to echo\n");

        // If incoming command is register, append port numbers and IP address to output string
        if (echoString[0] == 'r')
        {
            char str[50];
            sprintf(str, " %d %d %d %s", trackerPort, sendPort, rcvPort, inet_ntoa(trackerAddr.sin_addr));
            strcat(echoString, str);
        }
        // check if the input command is exit, set boolean to true if yes
        if (echoString[0] == 'e')
        {
            exitflag = true;
        }
        if (echoString[0] == 't')
        {
          tweetFunc(echoString);
        }
        else
        {
        // Send the string to the server
        if (sendto(sock1, echoString, strlen(echoString), 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != strlen(echoString))
            DieWithError("client: sendto() sent a different number of bytes than expected");

        // Receive a response
        fromSize = sizeof(fromAddr);

        if ((respStringLen = recvfrom(sock1, echoString, ECHOMAX, 0, (struct sockaddr *)&fromAddr, &fromSize)) > ECHOMAX)
            DieWithError("client: recvfrom() failed");

        echoString[respStringLen] = '\0';

        //If server has alerted user, recieve and print tweet
        if(echoString[0] == 'A')
        {
          rcv = true;
        }

        // print recieved string
        printf("%s\n", echoString);
        
        // if command was exit, exit the program
        if (exitflag)
        {
            close(sock);
            exit(0);
        }
      }
    }

    close(sock);
    exit(0);
}

void tweetFunc(char *str)
{
  //send tweet message to tracker
  if (sendto(sock1, str, strlen(echoString), 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != strlen(str))
    DieWithError("client: sendto() sent a different number of bytes than expected");

  // Receive a response
  fromSize = sizeof(fromAddr);
  
  //get list of followers
  if ((respStringLen = recvfrom(sock1, str, ECHOMAX, 0, (struct sockaddr *)&fromAddr, &fromSize)) > ECHOMAX)
    DieWithError("client: recvfrom() failed");

  char followerList[ECHOMAX];
  strcpy(followerList, str);
  followerList[respStringLen] = '\0';

  //print list of followers
  printf("%s\n", followerList);

  
  char *tweet; 
  tweet = (char *)malloc(140);
  size_t tweetLen = 140;
 // Get user tweet as command line input
  printf("\nEnter Tweet: \n");
  if ((nread = getline(&tweet, &tweetLen, stdin)) != -1)
  {
      tweet[(int)strlen(tweet) - 1] = '\0'; // Overwrite newline
  }
  else
      DieWithError("client: error reading string to echo\n");

  
  //Get handle name of the tweeting handle
  char tweetingHandleName[15];
  char propstr[250];
 

  struct sockaddr_in followerAddr;
  char *followerIP = "127.0.0.22";
  int followerPort = 6669;
  

  char *token = strtok(followerList, " ");
  
  token = strtok(NULL, " ");
  strcpy(tweetingHandleName, token);
  //string to store tweet
  sprintf(propstr, "User %s tweeted: \"%s\" \n\0", tweetingHandleName, tweet);
  printf("%s\n", propstr);

  //tokenize follower list to extract follower port and IP
  token = strtok(NULL, " ");
  token = strtok(NULL, " ");
  int count = 0;

  while(count < 3)
  {
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    followerPort = atoi(token);
    token = strtok(NULL, " ");
    followerIP = token;

    memset(&followerAddr, 0, sizeof(followerAddr));
    followerAddr.sin_family = AF_INET;
    followerAddr.sin_addr.s_addr = inet_addr(followerIP);
    followerAddr.sin_port = htons(followerPort);
  //Send tweet to next follower
  sendto(sock2, propstr, strlen(propstr), 0, (struct sockaddr *)&followerAddr, sizeof(followerAddr));  
  count++;
  }
  
  //send end-tweet command back to server after tweet is done
  sendto(sock1, "end-tweet", strlen("end-tweet"), 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr));
  return;
}


//Get message from recv Port and print it
void printTweet()
{
  char *str;
  size_t strLen = ECHOMAX;
  fromSize = sizeof(fromAddr);
  //Recieve message on socket3 (recieving socket)
  strLen = recvfrom(sock3, str, ECHOMAX, 0, (struct sockaddr *)&fromAddr, &fromSize);
  str[strLen] = '\0';
  printf("%s\n", str);
  rcv = false;
  //get a buffer message from the server to synchronize P2P and client-server messanging
  strLen = recvfrom(sock1, str, ECHOMAX, 0, (struct sockaddr *)&fromAddr, &fromSize);
}




