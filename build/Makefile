OBJECTS= tools.o util.o result.o latexparser.o equation.o math.o

INCLUDES=tools.h util.h result.h latexparser.h equation.h mathwordtools.h

RAOPT=

CC=g++
CCC=g++
CXX=g++

CPPFLAGS=-O0 -Wall -fPIC  -g
#CPPFLAGS= -g -Wall 


latexWord: $(OBJECTS)
	$(CXX) $(OBJECTS) -shared -o liblatexWord.so

lxp: $(OBJECTS)
	purify  -chain_length=10 -cache-dir=/u/medee/0/miaou/grimm/raweb/lx/TMP g++ -o lxp $(OBJECTS) -g

	 $(CCC) $(CPPFLAGS) -c $.cpp $(PAORT)
