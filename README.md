# Socket Programming Project

This project implements a simple Twitter-like application using socket programming in C.

## Overview

The application consists of a central tracker server and multiple client handles (users). Clients can register with the tracker, follow/unfollow other handles, tweet messages, and receive tweets from handles they follow. 

The communication happens using UDP sockets. Each client handle opens 3 sockets - one for communicating with the tracker, one for sending tweets to followers, and one for receiving tweets.

## Contents

The repository contains the following files:

- `tracker.c` - Implements the tracker server
- `user.c` - Implements the client handles
- `defns.h` - Contains common definitions, data structures  
- `README.md` - This file

## Working

### Tracker Server

- Listens on a UDP port for incoming messages
- Maintains a linked list of registered handles and their info  
- Handles registration of new handles
- Allows handles to follow/unfollow other handles
- Returns list of followers when a handle wants to tweet

### Client Handles  

- Register with the tracker on start
- Open 3 sockets for communicating with tracker and other handles  
- Can issue various commands to tracker:
  - `register` - To register themselves 
  - `follow`/`unfollow` - To follow/unfollow other handles
  - `tweet` - To tweet a message to their followers
- Send and receive tweets from handles they follow
  
## Message Format

Messages in this implementation are strings.
1. Register: Register command is issued in the following format.
* “register <handle_name>.
* Here handle name is the name of the handle. The information about 3 ports and user IP address is filled via
program and than sent to tracker. Server will send success or receive based on the results.
2. Query: Query command is issued in the following format:
* “query”
* This will return the list of the registered users with their port and IP address information.
3. Follow: Follow command is issued in the following format:
* “follow <handle1> <handle2>”
* Here handle1 wants to follow handle2. Either success or failure is the return message.
4. Drop: Follow command is issued in the following format:
* “drop <handle1> <handle2>”
* Here handle1 wants to stop following handle2. Either success or failure is the return message.
5. Tweet: Tweet command is issued in the following Format:
* “Tweet <handle>”
* Here the handle wants to circulate message to all of its followers. Tracker returns list of followers of the 
handle, which is used by handle to create/update logical ring. Handle than asks for the tweet and the input is 
circulated. Upon receiving the tweet back, handle gives 'end-tweet' command back to the tracker.
6. Exit: Exit command is issued in the following format:
* “exit <handle_name>”
* Here handle name is the name of the user that wants to exit the TWEETER application

## Data Structures  

- `struct handle` - Represents a handle
  - Contains name, IP, ports 
  - Linked list to store handles
- `struct follower` - Represents a follower of a handle 
  - Contains name
  - Linked list to store followers of a handle  

## Algorithms

- **Registration**: Handles are added to a linked list on registration 
- **Following**: A follower struct is added to the follower LL of target handle
- **Tweeting**:
  - Get followers from tracker 
  - Pass tweet in a ring fashion among followers
  - Uses 2 additional sockets for sending and receiving tweets among handles  

## Testing  

See the video [video](https://youtu.be/ywNy40caKew) for a demo of the app. 

It shows:  

- handles registering with the tracker  
- querying list of registered handles
- following/unfollowing other handles  
- tweeting and receiving tweets among followers


## CHOICE OF ALGORITHMS AND DESIGN CONSIDERATIONS
* The first step in socket programming is to register our clients with the Tracker/Server. In order to do this, we need
the user's name, IP address, and port number (which is used to communicate between the clients). We used a
globally defined Linked List, and one of the data values is a pointer to another linked list. The second linked list is
used to keep track of its followers, which can be used to create a logical ring required for tweeting later on.
* To sign up for the account, I send a string from the client side that contains all of the information required to sign
up for the tracker. The tokens are then separated from the string by their space value and placed in a globally
defined linked list. To accomplish this, we wrote a function that takes an unsigned integer and the received string
as parameters. Because we know that the string size is always a positive number, we're using unsigned int in this
case. To break the string into tokens, we used the built-in C function strtok. This function takes two arguments: a
string and a delimiter, which in this case is a space. The data is then added to the linked list after we loop through
the linear array. We keep track of the registered users by incrementing a flag variable as new users are added.
When a user handle is asked to register, we first loop through the linked list to see if the user is already
registered. If the user is already registered, we simply display that the user is already present.
* Following registration, the next step is to determine who has registered with the tracker overall. To do this, we
built a function called Query Handle. When the server receives a string with the value Handle, this method is
invoked. We make use of the flag variable used while registering the user with the server. If this flag variable has a
value of zero, then there are no users registered with the server, and the client is informed of this. If this flag has
a value greater than zero, then the client is provided with a list of all registered handle. We loop through the
linked list, appending all of the user handle information to a single string called echo string, which is returned to
the client.

## Client Side Peer 2 Peer design Considerations
*  When the tweet command is entered we circulate the tweet among the followers of the user which issued
the tweet command. On the client side, three sockets are maintained: sock1, sock2, and sock3. Sock 1 is
solely used to communicate between the tracker and the client. Socket 2 only receives messages from other
clients, while socket 3 sends messages to other clients .Socket 2 is assigned a port number and an IP address
so that it can continue to listen from that address. This also aids us in distinguishing between messages
received from the server and those received from the client. For this purpose, we're using a nonbinding
socket, which allows us to receive messages from both sockets at the same time, eliminating the need to
wait for messages from either socket. When a user sends a tweet, Sock 1 communicates with the tracker and
retrieves the logical ring of followers that user had. Once this is fetched with sock 3, we will send the
message to the first follower, along with the follower ring , but we remove the first follower user name and
related details from that ring. User receives the tweet and the follower ring from sock 2 and this process is
continued till the message reaches the user which issued the tweet command

## Scope for Improvements

Some ideas for enhancing the application:  

- Add authentication for handles
- Store tweets in a database  
- Improve message formats for better parsing 
- Add multithreading for high concurrency
- Scale tracker to multiple servers