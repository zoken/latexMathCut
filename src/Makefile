OBJECTS=  chars.o main.o cfg.o parser.o l2r_fonts.o convert.o\
        lengths.o preamble.o commands.o counters.o definitions.o \
        direct.o encode.o equation.o funct1.o graphics.o ignore.o \
        letterformat.o stack.o tables.o xref.o util.o\
        mygetopt.o optind.o

INCLUDES=chars.h main.h cfg.h parser.h l2r_fonts.h convert.h\
        lengths.h preamble.h commands.h counters.h definitions.h \
        direct.h encode.h equation.h funct1.h graphics.h ignore.h \
        letterformat.h stack.h tables.h xref.h util.h\
        encode_tables.h version.h

RAOPT=

CC=g++
CCC=g++
CXX=g++

CPPFLAGS=-O0 -Wall -fPIC  -g
#CPPFLAGS= -g -Wall 


latex2rtf: $(OBJECTS)
	$(CXX) -o latex2rtf $(OBJECTS) -lstdc++

latex2rtf_static: $(OBJECTS)
	$(CXX) -o latex2rtf_static -static $(OBJECTS) -lstdc++

lxp: $(OBJECTS)
	purify  -chain_length=10 -cache-dir=/u/medee/0/miaou/grimm/raweb/lx/TMP g++ -o lxp $(OBJECTS) -g


xref.o: xref.cpp $(INCLUDES) xref.h
stack.o: stack.cpp $(INCLUDES) stack.h
util.o: util.c $(INCLUDES) util.h
optind.o: optind.c $(INCLUDES)
mygetopt.o: mygetopt.c $(INCLUDES)
ignore.o: ignore.c  $(INCLUDES) ignore.h
graphics.o: graphics.c $(INCLUDES) graphics.h
funct1.o: funct1.c $(INCLUDES) funct1.h
equation.o: equation.c $(INCLUDES) equation.h
encode.o: encode.c $(INCLUDES) encode.h
direct.o: direct.c $(INCLUDES) direct.h
definitions.o: definitions.c $(INCLUDES) definitions.h
counters.o: counters.c $(INCLUDES) counters.h
commands.o: commands.c $(INCLUDES) commands.h
preamble.o: preamble.c $(INCLUDES) preamble.h
lengths.o: lengths.c $(INCLUDES) lengths.h
convert.o: convert.cpp $(INCLUDES) convert.h
l2r_fonts.o: l2r_fonts.c $(INCLUDES) l2r_fonts.h
parser.o: parser.c $(INCLUDES) parser.h
cfg.o: cfg.c $(INCLUDES) cfg.h
main.o: main.c $(INCLUDES) main.h
chars.o: chars.c $(INCLUDES)
	$(CC) $(CPPFLAGS) -c $*.c $(RAOPT)
	$(CC) $(CPPFLAGS) -c $*.cpp $(PAORT)
