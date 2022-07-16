#include "mpi/mpi_head.h"
#include "mpi/server.h"
#include <functional>

//g++ mpi_head_test.cpp -std=c++17 -lpthread -o mpi_head_test


int main(int argc, char** argv){
	mpi_head *mpi_controller = new mpi_head("192.168.1.101");
	server* my_serv = new server("192.168.1.101", 900);
	mpi_controller->set_handleInput(std::bind(&server::sendMsg, my_serv, std::placeholders::_1));
	my_serv->setInputFunc(std::bind(&mpi_head::sendS, mpi_controller, std::placeholders::_1, "odroid-binance"));
	my_serv->run();
	delete mpi_controller;
	delete my_serv;
}
