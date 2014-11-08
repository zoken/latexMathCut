#include <vector>
#include <string.h>
#include <stdlib.h>
#include "FullSegment.hpp"
#include "MixSegment.hpp"

class ChineseCut{
    private:
    public:
        ChineseCut(){}
        virtual ~ChineseCut(){}
        virtual void cut(const char* word,std::vector<std::string>& words){};
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
        virtual void cut(const char* word,std::vector<std::string>& words);
};
class TC_Cut : public ChineseCut{

};
