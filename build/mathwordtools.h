#include "stdlib.h"
#include <list>
#include <vector>
#include "chineseCut.h"
#include "eqs.h"
#include "wordpos.h"
#define CHINESE_USE 1
#define JIEBA_CUT 1
#define TC_CUT 2

class MathSegmentTool{
    private:
        ChineseCut *_segment ;
        void handleContent(char*,EQS**,std::list<char*>* ,std::vector<WordPos*>& results);
    public:
        void findWordPoses(char* sentence ,std::list<char*>& words ,std::vector<WordPos*>& results);
        void cut(const char* sentence,std::vector<WordPos*>& results,std::list<char*>* result);
        void regChineseCutTool(ChineseCut* chinesecut){
            _segment = chinesecut;
        }
        void chineseCut(const char* word,std::list<char*>* resultlist,std::vector<WordPos*>& results);
};
