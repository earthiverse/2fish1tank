normal: main.cpp
	g++ -g -s -Wall --std=c++11 main.cpp -o 2fish1tank.app -lzmq -lboost_program_options -I include

debug: main.cpp
	g++ -g -Wall --std=c++11 main.cpp -o 2fish1tank.app -lzmq -lboost_program_options -I include -DNDEBUG

performant: main.cpp
	g++ -s --std=c++11 main.cpp -o 2fish1tank.app -lzmq -lboost_program_options -I include -O3
