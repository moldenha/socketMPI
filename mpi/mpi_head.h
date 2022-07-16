#ifndef MPI_HEAD_H
#define MPI_HEAD_H

#include "server.h"
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

class mpi_head{
	std::map<std::string, int> connections;
	server* serv;
	std::function<void(std::string)> handleInput;
	void handle_login(std::string);
	bool startswith(std::string input, std::string starting){
		return input.rfind(starting, 0) == 0;
	}
	public:
		mpi_head(const char* addr, int port=901)
			:serv(new server(addr, port)) {
				serv->setInputFunc(std::bind(&mpi_head::handle_input, this, std::placeholders::_1));
				std::thread(std::bind(&server::run, serv)).detach();
			}
		~mpi_head(){delete serv;}
		void handle_input(std::string input);
		void sendS(std::string message, std::string name){
			try{this->sendI(message, connections.at(name));}
			catch(...){std::cout<<"had to send to all"<<std::endl;send_all(message);}
		}
		void sendI(std::string message, int connection){serv->sendMsg_spec(message, connection);}
		void send_all(std::string message){serv->sendMsg(message);}
		void set_handleInput(std::function<void(std::string)> func){handleInput = func;}
};


void mpi_head::handle_input(std::string input){	
	if(startswith(input, "login:")){
		handle_login(input);
		return;
	}
	handleInput(input);
}

void mpi_head::handle_login(std::string input){
	int connection = serv->connections.back();
	//input.substr(0,6) == "login:"
	std::string name(input.substr(6));
	connections.insert({name, connection});
	std::cout<<name<<" logged in at connection number "<<connection<<std::endl;
}


#endif
