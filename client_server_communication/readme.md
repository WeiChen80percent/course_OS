# Bill, Alice and Wei simple communication app

A TCP-based client-server chat application that enables users to connect to a server and exchange messages in real-time.

## Coding Enviroment

Ubuntu VScode

## Features

- Multi-client support with unique usernames (No overlap)
- Real-time messaging between users (At most 5)
- Server-side whiteboard that logs all communications (Implement using "string", including online/offline/sending messages )
- Thread-safe implementation for concurrent client handling
- Connection status monitoring
- User presence notifications (online/offline broadcasting)

## System Requirements

- C++ compiler with C++11 support
- POSIX-compliant operating system (Linux, macOS)
- pthread library

## Building the Application

Compile the server and client separately (In the makefile):

```bash
# Compile the server
g++ -o server server.cpp 

# Compile the client
g++ -o client client.cpp 
```

## Usage

### Starting the Server

The server automatically listens on IP 127.0.0.1 (localhost) and port 8080:

```bash
./server
```

Once started, the server will display its IP address and port number on the whiteboard.

### Starting the Client

```bash
./client
```

### Client Commands

The client supports the following commands:

1. **Connect to the server**:
   ```
   connect <server_ip> <server_port> <username>
   ```
   Example: `connect 127.0.0.1 8080 Alice`

2. **Send a message to another user**:
   ```
   chat <recipient_username> "<message>"
   ```
   Example: `chat Bob "Hello, how are you?"`

3. **Disconnect from the server**:
   ```
   bye
   ```

## Server Functionality

- The server maintains a whiteboard that logs all connections and messages
- Displays real-time updates of user activities
- Automatically handles user connections and disconnections
- Prevents duplicate usernames
- Forwards messages between users

## Technical Details

- The application uses TCP sockets for reliable communication
- Server supports multiple simultaneous client connections through multi-threading
- Messages are transmitted as text strings with a maximum size of 1024 bytes
- Mutex locks ensure thread-safe access to shared resources (writing WhiteBoard)
- Timestamps are added to all messages for logging purposes

## Error Handling

The application handles various error cases:
- Connection failures
- Duplicate usernames
- Non-existent recipients
- Unexpected disconnections

## Limitations

- The server IP and port are hardcoded (127.0.0.1:8080)
- Maximum message size is limited to 1024 bytes
 
