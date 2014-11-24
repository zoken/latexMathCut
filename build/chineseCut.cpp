#include "chineseCut.h"
//#include "mathinterface.h"
using namespace std;

void Jieba_Cut::cut(const char* word,vector<string>& words){
    if(_fullsegment!=NULL){
        _fullsegment->cut(word,words);
    }else if(_segment!=NULL){
        _segment->cut(word,words);
    }
}
/*
void findWordPoses(char* sentence ,int start_index , vector<char*>& words , vector<WordPos*>& result){
    vector<char*>::iterator iter,count_iter;
    vector<int> starts ;
    vector<int> ends ;
    int i,timeCount ,tmppos;
    int len = strlen(sentence);
    char* tmp_sentence ;
    WordPos* tmpwordpos = NULL ;
    for(iter = words.begin(); iter!=words.end();iter++){
    //查找*iter已出现的次数    
        timeCount = 0 ;
        for(count_iter = words.begin() ; count_iter!=iter ;count_iter++){
            if(strcmp(*iter,*count_iter)==0){
                timeCount++;
            }
        } 
        tmp_sentence = sentence ;
        for( i = 0 ; i < timeCount ; i++){
            tmp_sentence = strstr(tmp_sentence , *iter);
            if(tmp_sentence == NULL){
                break ;
            }
            tmp_sentence++;//指向下一段字符串，以便寻找下一个出现的单词
        }
        if(tmp_sentence ==NULL)
            continue ;
        else{
            tmp_sentence = strstr(tmp_sentence , *iter);
            if(tmp_sentence == NULL)
                tmppos = -1;
            else{
                tmp_sentence = strstr(tmp_sentence , *iter);
                if(tmp_sentence == NULL)
                    tmppos = -1;
                else{
                    tmppos = len - strlen(tmp_sentence);
                }
                tmppos += start_index;
                tmpwordpos = new  WordPos();
                tmpwordpos->word = *iter ;
                tmpwordpos->pos = tmppos ;
                result.push_back(tmpwordpos);
            }
        }
    }
}
void Jieba_Cut::cut(const char* word, vector<char*>& results){
    vector<string> tmp_results ;
    if(_fullsegment!=NULL){
        _fullsegment->cut(word,tmp_results);
    }else if(_segment!=NULL){
        _segment->cut(word,tmp_results);
    }
    results.
}
*/
