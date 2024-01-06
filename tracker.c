// Group 31
// Members: Dipak Chaudhary, Ramakrishna Valupadasu
// Current Implementation level: Socket Project Milestone

//  Implements the server side of the TWEETER program
//  Runs infinitely, listens to specified port and changes user data based on commands recieved
#include "defns.h"

#define ECHOMAX 255 // Longest string to echo

// strings to compare input commands with
char register_Handle[30] = "register";
char Query_Handle[30] = "query";
char follow_handle[30] = "follow";
char drop[30] = "drop";
char exitHandle[30] = "exit";
char tweetHandle[30] = "tweet";
struct handle *head = NULL; //global linked list header

int size = 0;         // size of the trackewr array
int handle_count = 0; // Number of handles in follower array

  int sock;                        // Socket
void DieWithError(const char *errorMessage) // External error handling function
{
  perror(errorMessage);
  exit(1);
}

int main(int argc, char *argv[])
{
  struct sockaddr_in echoServAddr; // Local address of server
  struct sockaddr_in echoClntAddr; // Client address
  unsigned int cliAddrLen;         // Length of incoming message
  char echoBuffer[ECHOMAX];        // Buffer for echo string
  unsigned short echoServPort;     // Server port
  int recvMsgSize;                 // Size of received message

  if (argc != 2) // Test for correct number of parameters
  {
    fprintf(stderr, "Usage:  %s <UDP SERVER PORT>\n", argv[0]);
    exit(1);
  }

  echoServPort = atoi(argv[1]); // First arg: local port

  // Create socket for sending/receiving datagrams
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    DieWithError("server: socket() failed");

  // Construct local address structure */
  memset(&echoServAddr, 0, sizeof(echoServAddr));   // Zero out structure
  echoServAddr.sin_family = AF_INET;                // Internet address family
  echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
  echoServAddr.sin_port = htons(echoServPort);      // Local port

  // Bind to the local address
  if (bind(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0)
    DieWithError("server: bind() failed");

  printf("server: Port server is listening to is: %d\n", echoServPort);

  for (;;) // Run forever
  {
    cliAddrLen = sizeof(echoClntAddr);

    // Block until receive message from a client
    if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *)&echoClntAddr, &cliAddrLen)) < 0)
      DieWithError("server: recvfrom() failed");

    echoBuffer[recvMsgSize] = '\0';

    // Printing recieved command
    printf("server: received string %s\n", echoBuffer);

    // Getting the length of the echoBuffer input
    int l = 0;
    while (l <= 255 && echoBuffer[l] != '\0')
    {
      if (!isspace(echoBuffer[l]))
        l++;
      else
        break;
    }
    // Case of receiving register command
    if (strncmp(register_Handle, echoBuffer, l) == 0)
    {
      unsigned int n = sizeof(echoBuffer) / sizeof(echoBuffer[0]); // length of echoBuffer
      int check1 = registerserver(echoBuffer, n);                  // calling register handler function
      if (check1 == 1)
      { // printing success message
        strcpy(echoBuffer, "Successfully registered handle");
      }
      else
      { // printing failure message
        strcpy(echoBuffer, "User Already Registered");
      }
    }
    // Case of query command
    else if (strncmp(Query_Handle, echoBuffer, l) == 0)
    {
      int flag = queryhandle(echoBuffer); // calling query handler
      if (flag == 0)
      { // failure if there are no users registered
        strcpy(echoBuffer, "There are no users currently registered with the tracker\n");
      }
    }

    // Case of follow command
    else if (strncmp(follow_handle, echoBuffer, l) == 0)
    {
      int flag = followHandler(echoBuffer); // follow handler
      if (flag == 1)
        strcpy(echoBuffer, "SUCCESS"); // sending success or failure message
      else
        strcpy(echoBuffer, "FAILURE");
    }

    // Case of drop command
    else if (strncmp(drop, echoBuffer, l) == 0)
    {
      int flag = dropHandler(echoBuffer); // drop command handler
      if (flag == 1)
        strcpy(echoBuffer, "SUCCESS");
      else
        strcpy(echoBuffer, "FAILURE");
    }

    // Case of exit command
    else if (strncmp(exitHandle, echoBuffer, l) == 0)
    {
      int flag = exitHandler(echoBuffer); // exit command handler
      if (flag == 1)
        strcpy(echoBuffer, "SUCCESS");
      else
        strcpy(echoBuffer, "FAILURE");
    }

    // Case of tweet command
    else if (strncmp(tweetHandle, echoBuffer, l) == 0)
    {
      int flag = tweetHandler(echoBuffer); // tweet command handler
      if (flag == 0)
        strcpy(echoBuffer, "FAILURE");
    }
    else if(strncmp("print", echoBuffer, l) == 0)
    {
      strcpy(echoBuffer, "");
    }
    else
      strcpy(echoBuffer, "FAILURE: INVALID COMMAND"); // Invalid Command failure

    // Send success or failure message back to the user
    if (sendto(sock, echoBuffer, strlen(echoBuffer), 0, (struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr)) != strlen(echoBuffer))
      DieWithError("server: sendto() sent a different number of bytes than expected");
  }
  // NOT REACHED */
}

// Register Function
int registerserver(char *echoBuffer, unsigned int n)
{
  int i = 0;
  int flag = 0;
  char *p = strtok(echoBuffer, " "); // tokenize input on space
  char *array[10];
  while (p != NULL)
  {
    array[i++] = p; // store each word of the command in an array
    p = strtok(NULL, " ");
  }

  struct handle *ptr = head; // pointer to the head of global linked list
  bool found = false;

  // Search the list to see if username is already taken
  while (ptr != NULL)
  {
    if (strcmp(ptr->handle_name, array[1]) == 0)
    {
      found = true;
      return 0; // return 0 (failure) if username is taken
    }
    ptr = ptr->next;
  }

  // If username is not found than create a new user and add him to list
  if (!found)
  {
    // Create new object of struct handle and add appropriate values to its parameters
    struct handle *new_handle = (struct handle *)malloc(sizeof(struct handle));
    strcpy(new_handle->handle_name, array[1]);
    new_handle->port1 = atoi(array[2]);
    new_handle->port2 = atoi(array[3]);
    new_handle->port3 = atoi(array[4]);
    strcpy(new_handle->ipAddr, array[5]);
    new_handle->next = head;
    head = new_handle;
    size++;
  }

  return 1;
}

// Query Function
int queryhandle(char *echoBuffer)
{
  if (size != 0) // Checks that there is atleast one user registered
  {
    memset(echoBuffer, 0, sizeof(echoBuffer)); // resetting output
    struct handle *ptr = head;                 // pointer to the head of global linked list
    int n = 0;
    char num[2];
    // traverse the global list of handles
    while (ptr != NULL)
    {
      sprintf(num, "%d", n); // converting current number to string and appending it
      strcat(echoBuffer, num);
      strcat(echoBuffer, " ");
      // appending rest of the information of the handle to the output
      appendHandle(ptr, echoBuffer);
      n++;
      ptr = ptr->next;
    }

    return 1;
  }
  else
    return 0;
}

// Function to append handle's info to output
void appendHandle(struct handle *handle, char *echoBuffer)
{
  char str[60]; // string to store handle info
  // adding handle info to string
  sprintf(str, "%s %d %d %d %s\n\0", handle->handle_name, handle->port1, handle->port2, handle->port3, handle->ipAddr);
  strcat(echoBuffer, str); // appending string to output
}

// Follow Function
int followHandler(char *echobuffer)
{
  // Tokenize input and store it to a char* array
  char *p = strtok(echobuffer, " ");
  char *array[4];
  int i = 0;
  while (p != NULL)
  {
    array[i++] = p;
    p = strtok(NULL, " ");
  }

  struct handle *ptr = head; // pointer to the head of global linked list
  bool found = false;

  struct handle *current;
  // Iterate the list to find the user that will be followed
  while (ptr != NULL)
  {
    // if the user is found
    if (strcmp(ptr->handle_name, array[2]) == 0)
    {
      current = ptr;
      // Create a new follower object and add it to the list of followers
      struct follower *new = (struct follower *)malloc(sizeof(struct follower));
      strcpy(new->follower_name, array[1]);
      new->next = current->head;
      current->head = new;
      found = true;
      return 1;
      break;
    }
    ptr = ptr->next;
  }


  // if user is not found return failure
  if (!found)
    return 0;

  return 0;
}

// Drop function
int dropHandler(char *echobuffer)
{
  // Tokenize input and store it to a char* array
  char *p = strtok(echobuffer, " ");
  char *array[4];
  int i = 0;
  while (p != NULL)
  {
    array[i++] = p;
    p = strtok(NULL, " ");
  }

  struct handle *ptr = head; // pointer to the head of global linked list
  bool found = false;

  // Finding the handle that will be unfollowed from the list
  while (ptr != NULL)
  {
    // If found leave the loop else continue
    if (strcmp(ptr->handle_name, array[2]) == 0)
    {
      found = true;
      break;
    }
    ptr = ptr->next;
  }

  // If handle not found than return failure
  if (!found)
    return 0;

  // struct follower pointer to iterate dfollowers list for a handle
  struct follower *t = ptr->head;
  struct follower *n;
  found = false;

  // If head of the list is the unfollowing user than remove it, return success
  if (strcmp(t->follower_name, array[1]) == 0)
  {
    ptr->head = ptr->head->next;
    free(t);
    return 1;
  }

  // Iterate followers list
  while (t != NULL)
  {
    // if unfollowing handle is found than drop it
    if (strcmp(t->follower_name, array[1]) == 0)
    {
      found = true;
      break;
    }
    n = t;
    t = t->next;
  }

  // if follower was not found than return failure
  if (!found)
    return 0;
  else
  {
    // else remove the handle from followers lists, return success
    n->next = t->next;
    free(t);
    return 1;
  }

  return 0;
}

// Exit Function
int exitHandler(char *echobuffer)
{
  char *p = strtok(echobuffer, " ");
  char *array[4];
  int i = 0;
  while (p != NULL)
  {
    array[i++] = p;
    p = strtok(NULL, " ");
  }

  // pointers to the head of the global list
  struct handle *current;
  struct handle *previous;
  current = head;
  bool found = false;

  // if head is the exiting handle, delete first node
  if (strcmp(head->handle_name, array[1]) == 0)
  {
    head = head->next;
    free(current);
    size--;
    return 1;
  }

  // iterate the list to find the handle
  while (current != NULL)
  {
    if (strcmp(current->handle_name, array[1]) == 0)
    {
      found = true;
      break;
    }
    previous = current;
    current = current->next;
  }

  // If handle is not in the list than return failure
  if (!found)
    return 0;
  else
  {
    // remove the handle from the list
    previous->next = current->next;
    free(current);
    size--;
    return 1; // return 1 if successful
  }

  return 0;
}

// Tweet command handler
int tweetHandler(char *echoBuffer)
{
  char *p = strtok(echoBuffer, " ");
  char *array[4];
  int i = 0;
  while (p != NULL)
  {
    array[i++] = p;
    p = strtok(NULL, " ");
  }

  if(strcmp(array[0], "tweet") != 0)
  {
    return 0;
  }
  // pointer to the head of the global list
  struct handle *ptr = head;
  bool found = false;
  int n = 0;
  // Find the handle in the list
  char str1[60];
  char output[ECHOMAX];
  memset(output, 0, sizeof(output));
  while (ptr != NULL)
  {
    if (strcmp(ptr->handle_name, array[1]) == 0)
    {
      found = true;
      sprintf(str1, "%d %s %d %s \n", n, ptr->handle_name, ptr->port3, ptr->ipAddr);
      strcat(output, str1);  
      break;
    }
    ptr = ptr->next;
  }
  struct handle *tweeting_handle = ptr;
  if (!found) // if handle is not in the list than return failure
  {
    return 0;
  }
  struct follower *itr = ptr->head;          // pointer to iterate follower list of tweeting handle
  n = 1;
  char num[2];

  struct sockaddr_in followerAddr;
  char *followerIP = ptr->ipAddr;
  int followerPort = ptr->port3;
  char propstr[20];
  strcpy(propstr, "Alert");

  while (itr != NULL)
  {
    ptr = head;
    // Iterate the global list to find the follower information
    while (ptr != NULL)
    {
      // If follower is found in the global list, output the handle information
      if (strcmp(ptr->handle_name, itr->follower_name) == 0)
      {
        //print follower recv port and ip address
        char str[60];
        sprintf(str, "%d %s %d %s \n", n, ptr->handle_name, ptr->port3, ptr->ipAddr);
        strcat(output, str);

        //send tweet Alert to followers
        memset(&followerAddr, 0, sizeof(followerAddr));
        followerAddr.sin_family = AF_INET;
        followerAddr.sin_addr.s_addr = inet_addr(ptr->ipAddr);
        followerAddr.sin_port = htons(ptr->port1);
        
        sendto(sock, propstr, strlen(propstr), 0, (struct sockaddr *)&followerAddr, sizeof(followerAddr));
      }
      ptr = ptr->next;
    }
    n++;
    itr = itr->next;
  }

  //print tweeting handle's info again
  sprintf(str1, "%d %s %d %s \n\0", n, tweeting_handle->handle_name, tweeting_handle->port3, tweeting_handle->ipAddr);
  strcat(output, str1);

  memset(echoBuffer, 0, sizeof(echoBuffer)); // reset output message
  strcpy(echoBuffer, output);
  return 1;
}
