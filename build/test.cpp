#include "mathwordtools.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <string.h>

int main(int argc, char* argv[]){
    #if CHINESE_USE == 2
        //INIT DIFFERENT CHINESE.
        //TCSeg
        const char* lpszDirPath = "";
        if(TCInitSeg(lpszDirPath) == false){
            std::cout << "TCInitSeg error. "<< std::endl;
            System.exit(1);
        }
    #endif
    char buf[40960];
//    Jieba_Cut jiebacut("../dict/jieba.dict.utf8","../dict/hmm_model.utf8","../dict/user.dict.utf8");
    Jieba_Cut jiebacut("../dict/extra_dict/jieba.dict.small.utf8");
    MathSegmentTool mwt ;
    mwt.regChineseCutTool(&jiebacut);
    int count = 0 ;
    std::list<char*> resultlist ;
    std::ifstream testfile(argv[1]);
    std::list<char*>::iterator iter ;
    std::vector<WordPos*> results;
    std::vector<WordPos*>::iterator _positer;
    if(!testfile.is_open()){
        std::cout << "error opening file" << std::endl ;
        exit(1);
    }else{
        while(!testfile.eof()){
            count++;
            testfile.getline(buf,40960);
            resultlist.clear();
            mwt.cut(buf,results , &resultlist);
//            if(count%10000==0){
//                for(iter=resultlist.begin();iter!=resultlist.end();iter++){
//                    std::cout << *iter << std::endl;
//                }
            results.clear();
            mwt.findWordPoses(buf,resultlist,results);
                for(_positer = results.begin(); _positer != results.end() ;_positer++){
                    std::cout <<(*_positer)->word << " "<< (*_positer)->pos <<std::endl;
                }
 //               std::cout << count << std::endl;
//                std::cout << "---------------------" << std::endl;
 //               std::cout << strlen(buf) << ":" << buf << std::endl;
                
                sleep(1);
//            }
        }
        testfile.close();
    }

  //  mwt.cut(argv[1],&resultlist);
    /**
    std::list<char*>::iterator iter ;
    for(iter=resultlist.begin();iter!=resultlist.end();iter++){
        std::cout << *iter << std::endl;
    }
    */
}
