TARGETS = tamamo
CXX	= g++
CFLAGS = -g -pg -O3 -fopenmp -Wall 
USE_LIB = -lPocoNet  # -lapx7140
CXXFLAGS = $(CFLAGS) -std=c++11 -D_FILE_OFFSET_BITS=64 -I ~/usr/include/ -L ~/usr/lib/ $(USE_LIB)

all:$(TARGETS)

tamamo:
	$(CXX) $(CXXFLAGS) -o tmm_daemon tamamo_daemon.cpp
	$(CXX) $(CXXFLAGS) -o main master_multinode_swhj.cpp
#	$(CXX) $(CXXFLAGS) -o main main_userprg.cpp

clean:
	rm tmm_daemon main
