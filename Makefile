# NOTE: If editing in nano, and you have tabs-to-spaces, hit shift+alt+q to use tabs

all:
	g++ -g -Wall --std=c++11 main.cpp -o 2fish1tank.app -lzmq -lboost_program_options -I include
