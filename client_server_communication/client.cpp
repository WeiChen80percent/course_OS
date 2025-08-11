#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <arpa/inet.h>
using namespace std;

#define MAX_BUFFER_SIZE 1024 // Maximum buffer size

int clientSocket; // Stores the client socket
string username; // Stores the username
bool connected=false; // Connection status

// Receive messages from the server
void* receiveMessages(void* arg){
    while(1){
        char buffer[MAX_BUFFER_SIZE]={};
        int byteRead=recv(clientSocket,buffer,sizeof(buffer)-1,0);
        if(byteRead<=0) break; // Exit loop if reading fails or connection is closed
        string message(buffer);
        cout<<message<<endl;
        
        // If the server responds that the username already exists, disconnect
        if(message=="Username already exist. Input again!"){
            connected=false;
            close(clientSocket);
            break;
        }
    }
}

// Send a message to the server
void sendMessage(string &input){
    send(clientSocket,input.c_str(),input.length(),0);
}

// Parse and execute commands
void parseCommand(string &input){
    stringstream ss(input);
    string cmd;
    ss>>cmd;
    
    // If the command is 'connect', establish a connection to the server
    if(cmd=="connect"){
        if(connected){
            cout<<"Already connected"<<endl;
            return;
        }
        
        string server_ip;
        int server_port;
        string user;
        ss>>server_ip>>server_port>>user;
        
        // Ensure valid input
        if(server_ip.empty()||server_port==0||user.empty()){
            cout<<"Usage: connect <server_ip> <server_port> <username>"<<endl;
            return;
        }
        
        username=user;
        
        sockaddr_in server_address;
        server_address.sin_family=AF_INET;
        server_address.sin_port=htons(server_port);

        if(inet_pton(AF_INET,server_ip.c_str(),&server_address.sin_addr)<=0){
            perror("Invalid address");
            return;
        }

        if(connect(clientSocket,(struct sockaddr*)&server_address,sizeof(server_address))<0){
            perror("Connection failed");
            return;
        }
        
        // Send the connection command to the server
        sendMessage(input);
        connected=true;
        
        // Create a thread to receive messages
        pthread_t receiveMessagesThread;
        if(pthread_create(&receiveMessagesThread,NULL,receiveMessages,NULL)!=0){
            perror("Thread creation failed");
            return;
        }

        pthread_detach(receiveMessagesThread); // Detach thread
        cout<<"Connected to server as "<<username<<endl;
    }
    // If the command is 'chat', send a chat message
    else if(cmd=="chat"&&connected){
        sendMessage(input);
    }
    // If the command is 'bye', disconnect
    else if(cmd=="bye"&&connected){
        string sendLine="bye";
        sendMessage(sendLine);
        connected=false;
        close(clientSocket);
        clientSocket=-1;
    }
    // Invalid command
    else{
        cout<<"Wrong input"<<endl;
    }
}

int main(){
    string input;
    while(1){
        // Create a new socket
        clientSocket=socket(AF_INET,SOCK_STREAM,0);
        if(clientSocket<0){
            perror("Socket creation failed");
            return 1;
        }
        
        while(1){
            getline(cin,input); // Read user input
            parseCommand(input); // Parse and execute the command
            usleep(100000); // Prevent rapid execution
            
            if(input=="bye"||connected==false) break; // If disconnected, create a new socket
        }
    }
}
