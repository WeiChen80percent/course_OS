#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <arpa/inet.h>
#include <vector>
#include <chrono>
#include <iomanip>
#include <mutex>
using namespace std;

// Maximum buffer size for message transmission
#define MAX_BUFFER_SIZE 1024

// Global whiteboard to store communication history
string WhiteBoard;

// Mutex for thread-safe access to shared resources
mutex g_mutex;

// Class to store client information
class clientInfo{
public:
    // Socket descriptor for the client
    int socket;
    // Client's IP address
    string ip;
    // Client's port number
    int port;
    // Client's username
    string name;

    // Constructor to initialize client information
    clientInfo(int s,string ipAddr,int p,string n){
        socket=s;
        ip=ipAddr;
        port=p;
        name=n;
    }
};

// Vector to store all connected clients
vector<clientInfo> clients;
vector<clientInfo> offlineClients;

// Function to get current timestamp as a string
string getCurrentTimeMethod(){
    // Get current system time
    auto now=std::chrono::system_clock::now();
    // Convert to time_t for formatting
    auto now_time_t=std::chrono::system_clock::to_time_t(now);
    
    // Create a string stream for formatting
    std::stringstream ss;
    // Format time as YYYY-MM-DD HH:MM:SS
    ss<<std::put_time(std::localtime(&now_time_t),"%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Broadcast a message to all clients except the sender
void broadcast(int senderSocket,string &message){
    // Iterate through all clients
    for(int i=0;i<clients.size();i++){
        // Send to all clients except the sender
        if(senderSocket!=clients[i].socket){
            send(clients[i].socket,message.c_str(),message.length(),0);
        }
    }
}

// Send a message to a specific receiver and update whiteboard
void sendMessage(int clientSocket,int receiverSocket,string &sendLine){
    // Send the message to the receiver
    send(receiverSocket,sendLine.c_str(),sendLine.length(),0);
    
    // Parse the send line to extract sender, receiver, and message
    stringstream ss(sendLine);
    string senderName,receiverName,msg;
    ss>>senderName;
    // Remove the ':' from sender name
    senderName=senderName.substr(0,senderName.length()-1);
    // Get the rest of the message
    getline(ss,msg);
    // Remove leading space
    msg=msg.substr(1,msg.length()-1);
    
    // Get current timestamp
    string time=getCurrentTimeMethod();
    
    // Find the receiver's name
    for(int i=0;i<clients.size();i++){
        if(receiverSocket==clients[i].socket){
            receiverName=clients[i].name;
            break;
        }
    }

    // Thread-safe update of the whiteboard
    g_mutex.lock();
    WhiteBoard+=("[" + time + "] "+senderName+" is using the whiteboard.\n<To "+receiverName+"> "+msg+"\n");
    g_mutex.unlock();
    
    // Clear screen and display whiteboard
    system("clear");
    cout<<WhiteBoard;
}

// Thread function to handle individual client communication
void* handleClient(void* arg){
    // Extract client socket from argument
    int clientSocket=*(int*)arg;
    delete (int*)arg;
    
    // Continuous message receiving loop
    while(1){
        // Buffer to receive messages
        char buffer[1024]={};
        // Receive message from client
        int byteRead=recv(clientSocket,buffer,sizeof(buffer)-1,0);
        string message(buffer);

        // Handle connection request
        if(message.substr(0,7)=="connect"){
            // Parse connection details
            stringstream ss(message);
            string command,server_ip,server_port,clientName;
            ss>>command>>server_ip>>server_port>>clientName;
            
            // Check if username already exists
            int flag=0;
            for(int i=0;i<clients.size();i++){
                if(clientName==clients[i].name){
                    flag=1;
                    break;
                }
            }
            
            // Reject duplicate username
            if(flag){
                string userNameExist="Username already exist. Input again!";
                send(clientSocket,userNameExist.c_str(),userNameExist.length(),0);
                break;
            }

            for(int i=0;i<offlineClients.size();i++){
                if(clientName==offlineClients[i].name){
                    offlineClients.erase(offlineClients.begin()+i);
                    break;
                }
            }
            
            // Get client's IP and port
            sockaddr_in address;
            socklen_t addr_size=sizeof(address);
            getpeername(clientSocket,(sockaddr*)&address,&addr_size);
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET,&address.sin_addr,clientIP,INET_ADDRSTRLEN);
            int clientPort=ntohs(address.sin_port);
            
            // Add new client to the list
            clients.push_back(clientInfo(clientSocket,clientIP,clientPort,clientName));
            
            // Create login notification message
            string loginMessage="<User "+clientName+" is on-line, socket address: "+clientIP+"/"+to_string(clientPort)+".>";
            
            // Thread-safe update of whiteboard
            g_mutex.lock();
            WhiteBoard+=loginMessage + "\n";
            g_mutex.unlock();

            // Clear screen and display whiteboard
            system("clear");
            cout<<WhiteBoard;

            // Broadcast login message to all other clients
            broadcast(clientSocket,loginMessage);
        }
        // Handle chat message
        else if(message.substr(0,4)=="chat"){
            // Find sender's name
            string command,receiverName,msg,senderName;
            for(int i=0;i<clients.size();i++){
                if(clientSocket==clients[i].socket){
                    senderName=clients[i].name;
                }
            }
            
            // Parse chat message
            stringstream ss(message);
            ss>>command>>receiverName;
            getline(ss,msg);
            
            // Find receiver
            int flag=0,receiverSocket;
            // Remove quotes from message
            msg=msg.substr(2,msg.length()-3);
            // Prepare send line
            string sendLine=senderName+": "+msg;
            
            // Find receiver's socket
            for(int i=0;i<clients.size();i++){
                if(receiverName==clients[i].name){
                    flag=1;
                    receiverSocket=clients[i].socket;
                    sendMessage(clientSocket,receiverSocket,sendLine);
                    break;
                }
            }
            
            // Handle non-existent user
            if(!flag){
                int flag2=0;
                for(int i=0;i<offlineClients.size();i++){
                    if(receiverName==offlineClients[i].name){
                        flag2=1;
                        string sendLine="<User "+receiverName+" is offline.>";
                        cout<<sendLine;
                        send(clientSocket,sendLine.c_str(),sendLine.length(),0);
                        break;
                    } 
                }
                if(!flag2){
                    string sendLine="<User "+receiverName+" does not exist.>";
                    cout<<sendLine;
                    send(clientSocket,sendLine.c_str(),sendLine.length(),0);
                }
            }
        }
        // Handle logout or disconnection
        else if(message.substr(0,3)=="bye"||byteRead==0){
            // Find and remove the client
            for(int i=0;i<clients.size();i++){
                if(clientSocket==clients[i].socket){
                    string clientName=clients[i].name;
                    
                    offlineClients.push_back(clients[i]);
                    clients.erase(clients.begin()+i);
                    
                    // Create logout notification
                    string sendLine="<User "+clientName+" is off-line.> ";
                    
                    // Broadcast logout to other clients
                    broadcast(clientSocket,sendLine);
                    
                    // Thread-safe update of whiteboard
                    g_mutex.lock();
                    WhiteBoard+=sendLine+"\n";
                    g_mutex.unlock();
                    
                    // Clear screen and display whiteboard
                    system("clear");
                    cout<<WhiteBoard;
                }
            }
            // Close client socket and exit thread
            close(clientSocket);
            break;
        }
    }
}

int main(){
    // Variables for server and client sockets
    int serverSocket,clientSocket;
    
    // Create server socket
    serverSocket=socket(AF_INET,SOCK_STREAM,0);
    if(serverSocket<0) {
        perror("Socket creation failed");
        return 1;
    }
    
    // Set socket option to reuse address
    int opt=1;
    if(setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0){
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    // Configure server address
    sockaddr_in serverAddress;
    serverAddress.sin_family=AF_INET;
    serverAddress.sin_port=htons(8080);
    
    // Set server IP to localhost
    if(inet_pton(AF_INET,"127.0.0.1",&serverAddress.sin_addr)<=0){
        perror("Invalid address");
    }
    
    // Bind socket to address
    if(::bind(serverSocket,(sockaddr*)&serverAddress,sizeof(serverAddress))<0){
        perror("Bind failed");
        return 1;
    }

    // Thread-safe update of whiteboard with server details
    g_mutex.lock();
    WhiteBoard+=("IP:"+string(inet_ntoa(serverAddress.sin_addr))+" port:"+to_string(ntohs(serverAddress.sin_port))+"\n");
    g_mutex.unlock();

    // Clear screen and display initial whiteboard
    system("clear");
    cout<<WhiteBoard;
    
    // Start listening for connections
    if(listen(serverSocket,5)<0){
        perror("Listen failed");
        return 1;
    }
    
    // Main server loop to accept client connections
    while(1){
        // Accept incoming client connection
        clientSocket=accept(serverSocket,nullptr,nullptr);
        if(clientSocket<0) {
            perror("Accept failed");
            continue;
        }
        
        // Create a pointer to client socket for thread argument
        int* clientSocketPtr=new int(clientSocket);
        
        // Create a thread to handle the client
        pthread_t handleClientThread;
        if(pthread_create(&handleClientThread,nullptr,handleClient,clientSocketPtr)!=0) {
            perror("Thread creation failed");
            close(clientSocket);
            continue;
        }
        
        // Detach the thread to allow it to run independently
        pthread_detach(handleClientThread);
    }
    
    // Close server socket (this line is technically unreachable)
    close(serverSocket);
}