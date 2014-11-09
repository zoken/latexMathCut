OBJECTS= tools.o util.o latexparser.o equation.o mathcutword.o eqs.o chineseCut.o

INCLUDES=tools.h util.h latexparser.h equation.h mathwordtools.h eqs.h chineseCut.h mathinterface.h

RAOPT=

CC=g++
CCC=g++
CXX=g++

CPPFLAGS=-O0 -Wall -fPIC  -g
#CPPFLAGS= -g -Wall 


latexWord: $(OBJECTS)
	$(CXX) $(OBJECTS) -shared -o libWordCut.so

lxp: $(OBJECTS)
	purify  -chain_length=10 -cache-dir=/u/medee/0/miaou/grimm/raweb/lx/TMP g++ -o lxp $(OBJECTS) -g

	 $(CCC) $(CPPFLAGS) -c $.cpp $(PAORT)
