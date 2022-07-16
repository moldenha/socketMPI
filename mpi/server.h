#ifndef SERVER_H
#define SERVER_H

/*
class testSends:
    updateLog = bytes('system:updateLog:ex:FTX;cs:fuck,fu,ck;up:1.7,-3,5;data:ending amount:0.000000\nUSDT -> BUSD1.589752\nBUSD -> ADA0.992601\nADA -> USDT0.992799\rStart Amount: 100\rPrices:\nUSDT/BUSD:\t1.56', 'utf-8')
    updateAmts = bytes('system:updateAMTs:key:TNT;val:300', 'utf-8')
    logBinance = bytes('binance:Hello!!', 'utf-8')

*/

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <algorithm>
#include <iostream>
#include <thread>
#include <functional>
#include <vector>
#include <arpa/inet.h>

class server{
	int server_fd, new_socket, valread, addrlen;
	sockaddr_in address;
	int opt = 1;
	std::vector<std::string> to_send;
	std::function<void(std::string)> handleInput;
	public:
		server(const char*, int);
		~server(){
			for(int i = 0; i < connections.size(); i++){
				close(connections.at(i));	
			}
			connections.clear();
			shutdown(server_fd, SHUT_RDWR);
		}
		std::vector<int> connections;
		void run();
		void handleClient(int new_socket);
		void sendMsg(std::string input);
		void sendMsg_spec(std::string input, int sock);
		void setInputFunc(std::function<void(std::string)> func){handleInput = func;} 
};

server::server(const char* addr, int port){
	addrlen = sizeof(address);
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	//the below one is used more for maybe linux? with max os a different one will be used
	//if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
	//	perror("setsockopt");
	//	exit(EXIT_FAILURE);
	//}
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	//address.sin_addr.s_addr = addr;
	address.sin_port = htons(port);

	//setting address
	if (inet_pton(AF_INET, addr, &address.sin_addr.s_addr) <= 0) {
        	printf("\nInvalid address/ Address not supported \n");
		return;
    	}

	if (bind(this->server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	std::cout<<"Server bound and listening "<<addr<<":"<<port<<std::endl;
}

void server::sendMsg(std::string input){
	if(connections.size() == 0){
		to_send.push_back(input);
		return;
	}
	const char* next_msg = input.c_str();
	for(int i = 0; i < connections.size(); i++){
		write(connections.at(i), next_msg, strlen(next_msg));
	}
}

void server::sendMsg_spec(std::string input, int sock){
	write(sock, input.c_str(), input.size());
}

void server::handleClient(int new_socket){
	if(to_send.size() > 0){
		for(int i = 0; i < to_send.size(); i++){
			const char* next_msg = to_send.at(i).c_str();
			std::cout<<"sending "<<next_msg<<std::endl;
			write(new_socket, next_msg, strlen(next_msg));
			sleep(2);
		}
		to_send.clear();
	}
	while(true){
		char sock_buf[1024];
		int sock_readden = read(new_socket, sock_buf, sizeof(sock_buf));
		if (sock_readden == 0)
			break;
		if (sock_readden > 0){
			std::cout<<"got message: "<<sock_buf<<std::endl;
			handleInput(std::string(sock_buf));
			bzero(sock_buf, sizeof(sock_buf));
		}
	}
	std::cout<<"connection closed"<<std::endl;
	connections.erase(std::remove(connections.begin(), connections.end(), new_socket), connections.end());
}

void server::run(){
	while(true){
		int new_socket;
		if ((new_socket = accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen)) < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		std::cout<<"got new connection"<<std::endl;
		connections.push_back(new_socket);
		std::thread(std::bind(&server::handleClient, this, new_socket)).detach();
	}
}



#endif
