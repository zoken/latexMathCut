OBJECTS= tools.o util.o result.o latexparser.o equation.o mathwordtools.o main.o

INCLUDES=tools.h util.h result.h latexparser.h equation.h mathwordtools.h

RAOPT=

CC=gcc
CCC=g++
CXX=g++

CPPFLAGS=-O0 -Wall -fPIC  -g
#CPPFLAGS= -g -Wall 


latex2rtf: $(OBJECTS)
	$(CXX) -o latexWordCut $(OBJECTS) -lstdc++

latex2rtf_static: $(OBJECTS)
	$(CXX) -o latex2rtf_static -static $(OBJECTS) -lstdc++

lxp: $(OBJECTS)
	purify  -chain_length=10 -cache-dir=/u/medee/0/miaou/grimm/raweb/lx/TMP g++ -o lxp $(OBJECTS) -g

tools.o: tools.cpp $(INCLUDES) tools.h
util.o: util.cpp $(INCLUDES) util.h
result.o: result.cpp $(INCLUDES) result.h
latexparser.o: latexparser.cpp $(INCLUDES) latexparser.h
equation.o: equation.cpp $(INCLUDES) equation.h
mathwordtools.o: mathwordtools.cpp $(INCLUDES) mathwordtools.h
main.o: main.cpp $(INCLUDES) latexparser.h mathwordtools.h
	 $(CCC) $(CPPFLAGS) -c $.cpp $(PAORT)
