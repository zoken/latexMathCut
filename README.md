LATEX WORD CUT
-------------
2014.10.22


###NOTES
    1. This project contains cppjieba and latex2rtf.
    2. I have modified latex2rtf to latexWordCut. latexWordCut is used for cut latex word from latex sentence.
    3. This project's sample file is "sample.cpp" and "test.cpp". 
        - test.cpp use class of MathSegmentTool's cut method. CUT is the function interface for user using cut function.
        - testfile is the test input file.
        - TEST COMMANDS:
            ./runTest.sh testfile
    4. cppjieba's code = *.hpp & Limonp/. we use it as source code

###USAGE
    1. cd build
    2. make (to create libWordCut.so)
    3. import libWordCut.so & mathinterface.h
    4. use like test.cpp and sample.cpp
        MathSegmentTool mwt ;
        vector<WordPos*> results ;
        const char* sentence = "[中文|英文|exp]+";
        mwt.cut(sentence,results);

####TAGS
    1. v1.0 finished.
    2. v1.1 finished. fix some bugs and add more math operate.
    3. v1.5 finished. change usage. add word location info. see test.cpp & mathinterface.h
    

