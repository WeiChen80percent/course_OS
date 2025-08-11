CC = g++
flag = -Wall -std=c++11 -pthread
all:
	$(CC) $(flag) -o server server.cpp 
	$(CC) $(flag) -o client client.cpp
	clear
server:
	$(CC) $(flag) -o server server.cpp 
client:
	$(CC) $(flag) -o client client.cpp 
clean:
	rm -f server client server.o client.o
