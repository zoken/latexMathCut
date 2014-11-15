#include "stdlib.h"
#include <vector>
#include "eqs.h"
#include "JiebaCut/MixSegment.hpp"
#include "JiebaCut/FullSegment.hpp"

class WordPos {
    public:
    char    *word;
    int     pos;
    bool    bcw;  // 用户自定义词 ? true : false
    int     cls;  // (用户自定义词 && TC_CLS) ? 用户自定义词分类号 : NULL
};
class ChineseCut{
    private:
    public:
        ChineseCut(){}
        virtual ~ChineseCut(){}
        virtual void cut(const char* word,std::vector<char*>& words){};
};
class Jieba_Cut : public ChineseCut{
    private:
        CppJieba::MixSegment* _segment;
        CppJieba::FullSegment* _fullsegment ;
    public:
        Jieba_Cut(char* jieba_dict_path,char* hmm_model_path,char* user_dict_path){
            _segment = new CppJieba::MixSegment(jieba_dict_path,hmm_model_path,user_dict_path);
        }
        Jieba_Cut(const char* dict_path){
            _fullsegment = new CppJieba::FullSegment(dict_path);
        }
        virtual ~Jieba_Cut(){}
        virtual void cut(const char* word,std::vector<char*>& words);
};
class MathSegmentTool{
    private:
        ChineseCut *_segment ;
        void handleContent(char*, EQS**, int, std::vector<WordPos*>&);
        void cutChineseWord(char*, int, std::vector<WordPos*>&);
        void cut(const char* sentence, std::vector<WordPos*>& results, int index);
    public:
        void cut(const char* sentence, std::vector<WordPos*>& result);
        void fullCut(const char* sentence, int start, int strlen, std::vector<WordPos*>& results);
        void regChineseCutTool(ChineseCut* chinesecut){
            _segment = chinesecut;
        }
};
