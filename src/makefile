CC=g++11
CPPFLAGS=-I
LDFLAGS=-g
BOOST_ROOT=PATH_TO_BOOST_ROOT
DEPS = bigraph.h indConst.h utility.h 
OBJ = bigraph.o main.o indConst.o utility.o 

%.o: %.cpp $(DEPS)
	$(CC) -std=c++2a $(LDFLAGS) -c -O3 -o $@ $< $(CPPFLAGS)$(BOOST_ROOT)

domind: $(OBJ)
	$(CC) -std=c++2a -pthread $(LDFLAGS) -O3 -o $@ $^ $(CPPFLAGS)$(BOOST_ROOT)
