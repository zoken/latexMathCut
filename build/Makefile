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

	 $(CCC) $(CPPFLAGS) -c $.cpp $(PAORT)
