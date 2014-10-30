#include <pthread.h>
#include <iostream>
#include "latexparser.h"
#include "equation.h"
#include "mathwordtools.h"
extern "C"{
#include "tools.h"
}
using namespace std; 

MathWordTool mwt ;
void* thread(void* arg)
{
    int i;
    double start = getTimeMillis();
    list<char*> resultlist ;
    int num = 1;
    for (i = 0 ; i < num ;i++){
        mwt.cut("$|{-\\frac{7}{9}}|÷（{\\frac{2}{3}-\\frac{1}{5}}）-\\frac{1}{3}×{（{-4}）^2}$",&resultlist);
        list<char*>::iterator iter ;
 //       for(iter = resultlist.begin();iter!=resultlist.end();iter++){
 //           cout << *iter << endl;
 //       }
    }
    double end = getTimeMillis();
    if((end-start)>=1)
        cout << (end - start) << endl ;
}
int main(int n,char** argv)
{
    int i,ret ;
    //-------------------------------finished init
    int customernum =1000;
    pthread_t id[1000];
    for(i = 0 ; i < customernum ; i++){
        ret=pthread_create(&id[i],NULL,thread,NULL);
//    for (int i = 0 ; i < 10 ;i++){
//        cout << "main " << i << endl ;
//        the_parser.the_result.clear();
//        the_parser.init(M.input_content);
//        the_parser.translate_all();
//        list<Xmlp> result = the_parser.the_result ;
//        sleep(1);
//    }
        if(ret!=0){
            return 1;
        }       
    }
    long start = getTimeMillis();
    for(i = 0 ; i < customernum ; i++){
        pthread_join(id[i],NULL);
    }
    long end = getTimeMillis();
    std::cout << "join Time:" << end-start <<std::endl;
    return (0);
}
