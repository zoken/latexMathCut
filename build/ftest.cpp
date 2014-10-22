#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <pthread.h>
#include "mathwordtools.h"
extern "C"{
#include "tools.h"
}
using namespace std ;

MathWordTool mwt ;
//list<char*> resultlist ;
void* thread(void* arg)
{
    int i;
    double start = getTimeMillis();
    char* sentence = (char*)arg;
    list<char*> resultlist ;
    int num = 1;
    for (i = 0 ; i < num ;i++){
        mwt.cut(sentence,&resultlist);
    }
    double end = getTimeMillis();
    if((end-start)>=1)
        cout << (end - start) << endl ;
}
int main(){
    char buf[4096] ;
    int customernum =1000 ;
    pthread_t id[1000];
    int i = 0 ;
    int ret = 0 ;
    ifstream testfile("/data/zhuzl/jy.test");
    if(!testfile.is_open()){
        cout << "error opening file" << endl ;
        exit(1);
    }
    else{
        while(!testfile.eof()){
            testfile.getline(buf,4096);
//            mwt.cut(buf,&resultlist);
            ret=pthread_create(&id[i++],NULL,thread,buf);
        }
    }
    ret = i ;
    for(i = 0 ; i <ret; i++){
            pthread_join(id[i],NULL);
    }
//    cout << resultlist.size() << endl;
/**
    list<char*>::iterator iter ;
    for(iter = resultlist.begin(); iter!=resultlist.end();iter++){
        cout << *iter << endl ;
    }
*/
    cout << "-----end------" << endl;
    return 0 ;
}
