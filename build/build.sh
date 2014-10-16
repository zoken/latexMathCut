#!/bin/sh

g++ -fPIC -c -x c++ TransCode.hpp 
g++ -fPIC -c -x c++ ISegment.hpp -include TransCode.hpp
g++ -fPIC -c -x c++ Trie.hpp
g++ -fPIC -c -x c++ DictTrie.hpp
g++ -fPIC -c -x c++ HMMSegment.hpp
g++ -fPIC -c -x c++ KeywordExtractor.hpp
g++ -fPIC -c -x c++ MPSegment.hpp
g++ -fPIC -c -x c++ QuerySegment.hpp
g++ -fPIC -c -x c++ FullSegment.hpp
g++ -fPIC -c -x c++ MixSegment.hpp
g++ -fPIC -c -x c++ PosTagger.hpp
g++ -fPIC -c -x c++ SegmentBase.hpp
