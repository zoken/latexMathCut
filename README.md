LATEX WORD CUT
-------------
2014.10.22


###NOTES
    1. This project contains cppjieba and latex2rtf.
    2. I have modified latex2rtf to latexWordCut. latexWordCut is used for cut latex word from latex sentence.
    3. This project's sample file is "sample.cpp" and "test.cpp". 
        - test.cpp use class of MathWordTool's cut method. CUT is the function interface for user using cut function.
        - testfile is the test input file.
        - TEST COMMANDS:
            ./test `cat testfile`
    4. cppjieba's code = *.hpp & Limonp/. we use it as source code.
    5. other testinputs : ~/jy.test(PRIVATE)

###USAGE
    1. import libWordCut.so & mathwordtools.h
    2. use like test.cpp and sample.cpp
        MathWordTool mwt ;
        list<char*> resultlist ;
        const char* sentence = "[中文|英文|exp]+";
        mwt.cut(sentence,&resultlist);

####TAGS
    1. v1.0 finished.
    2. v1.1 finished. fix some bugs and add more math operate.
    

