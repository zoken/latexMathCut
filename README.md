LATEX WORD CUT
-------------
2014.10.17

###NOTES
    1. This project contains cppjieba and latex2rtf.
    2. I have modified latex2rtf to latexWordCut. latexWordCut is used for cut latex word from latex sentence.
    3. This project's sample file is "sample.cpp" and "test.cpp". 
        - test.cpp use class of MathWordTool's cut method. CUT is the function interface for user using cut function.
        - sample.cpp is multi-thread cppfile.
        - testfile is the test input file.
        - TEST COMMANDS:
            `./test \`cat testfile\``
    4. cppjieba's code = *.hpp & Limonp/. we use it as source code.
    5. other testinputs : ~/jy.test(PRIVATE)
