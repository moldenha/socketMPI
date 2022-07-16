#include "mpi/mpi_head.h"
#include "mpi/server.h"
#include <functional>

//g++ mpi_head_test.cpp -std=c++17 -lpthread -o mpi_head_test


int main(int argc, char** argv){
	mpi_head *mpi_controller = new mpi_head("this machines IP");
	server* my_serv = new server("this machines IP", 900);
	mpi_controller->set_handleInput(std::bind(&server::sendMsg, my_serv, std::placeholders::_1));
	my_serv->setInputFunc(std::bind(&mpi_head::send_all, mpi_controller, std::placeholders::_1));
	my_serv->run();
	delete mpi_controller;
	delete my_serv;
}
