#ifndef MPI_NODE_H
#define MPI_NODE_H

#include <stdio.h>
#include <functional>
#include <thread>
#include <map>
#include <cstring>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "client.h"

class mpi_node{
	client *cli;
	void handle_msg(std::string);
	std::function<void(std::string)> handleInput;
	public:
		mpi_node(const char* addr, int port=901)
			:cli(new client(addr, port))
		{
			cli->setInputFunc(std::bind(&mpi_node::handle_msg, this, std::placeholders::_1));
			cli->run();
			this->login();

		}
		void login();
		void set_handleInput(std::function<void(std::string)> func){handleInput = func;}
		void send(std::string input){cli->sendMsg(input);}

};

void mpi_node::handle_msg(std::string input){
	handleInput(input);	
}


void mpi_node::login(){
	char name_buf[80];
	gethostname(name_buf, 80);
	std::string login = "login:"+std::string(name_buf);
	cli->sendMsg(login);
}

#endif
