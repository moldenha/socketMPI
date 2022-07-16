#ifndef CLIENT_H
#define CLIENT_H

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

class client{
	int sockfd, connfd;	
	sockaddr_in servaddr, cli;
	std::function<void(std::string)> handleInput;
	void wait_for_input();
	public:
		client(const char*, int);
		~client(){
			if(!connected)
				return;
			connected = false;
			close(sockfd);	
		}
		bool connected;
		void run();
		void sendMsg(std::string input);
		void setInputFunc(std::function<void(std::string)> func){handleInput = func;}
	        void waitForDisconnect(){while(connected){;}}	
};

client::client(const char* addr, int port)
	:connected(false)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);	
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(addr);
	servaddr.sin_port = htons(port);
	if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");
	connected = true;
}

void client::sendMsg(std::string input){
	const char* buf = input.c_str();
	int size = input.size();
	write(this->sockfd, buf, size);
}

void client::wait_for_input(){
	while(connected){
		char buf[1024];
		int output = read(this->sockfd, buf, sizeof(buf));
		if(output == 0){
			connected = false;
			std::cout<<"server disconnected"<<std::endl;
			break;
		}
		std::string input(buf);
		handleInput(input);
	}
}
void client::run(){
	assert(connected);
	std::thread(std::bind(&client::wait_for_input, this)).detach();
}

#endif
