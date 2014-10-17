#include <pthread.h>
#include <time.h>
#include <iostream>
#include "latexparser.h"
#include "equation.h"
#include "mathwordtools.h"

using namespace std; 

MathWordTool mwt ;
void* thread(void* arg)
{
int i;
    int start = clock();
    list<char*> resultlist ;
    int num = 1;
    for (i = 0 ; i < num ;i++){
        mwt.cut("$1+1$",&resultlist);
        list<char*>::iterator iter ;
        for(iter = resultlist.begin();iter!=resultlist.end();iter++){
            cout << *iter << endl;
        }
    }
    int end = clock();
    if((end-start)>=10000)
        cout << (end - start) << endl ;
}
int main(int n,char** argv)
{
    int i,ret ;
    //-------------------------------finished init
    int customernum =1000 ;
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
            printf ("Create pthread error!\n");
            return 1;
        }       
    }
    for(i = 0 ; i < customernum ; i++){
        pthread_join(id[i],NULL);
    }
    return (0);
}
