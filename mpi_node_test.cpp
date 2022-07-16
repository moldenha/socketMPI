#include "mpi/mpi_node.h"
#include "mpi/server.h"

//g++ mpi_node_test.cpp -std=c++17 -lpthread -o mpi_node_test


int main(int argc, char** argv){
	mpi_node *mpi_controller = new mpi_node("The head computers' IP address");
	server* my_serv = new server("This machines IP address", 900);
	mpi_controller->set_handleInput(std::bind(&server::sendMsg, my_serv, std::placeholders::_1));
	my_serv->setInputFunc(std::bind(&mpi_node::send, mpi_controller, std::placeholders::_1));
	my_serv->run();
	delete mpi_controller;
	delete my_serv;
}
