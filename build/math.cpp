#include "mathwordtools.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "latexparser.h"
#include "equation.h"
#include "EQS.h"
#include "Limonp/Config.hpp"
#include "MPSegment.hpp"
#include "HMMSegment.hpp"
#include "MixSegment.hpp"



#define BIGOP 2
#define MINOP 2
#define EQUALOP 2
#define BIGEQUALOP 2
#define MINEQUALOP 2
#define ADDOP 3
#define PLUSOP 3
#define MULOP 4
#define DIVOP 4
#define FLATOP 5

CppJieba::MixSegment _segment("../dict/jieba.dict.utf8","../dict/hmm_model.utf8","./");

char* getBeginMathString(char* sentence){
    int len,i;
    len = strlen(sentence);
    for(i = 0 ; i < len ; i++){
        if((sentence[i]>='a'&&sentence[i]<='z')||(sentence[i]>='A'&&sentence[i]<='Z')
                 ||(sentence[i]>='0'&&sentence[i]<='9'))
            continue ;
        else if(sentence[i]=='\342'){
            i++;
            if(sentence[i] == '\210' && sentence[i+1] == '\240')
                ;
            else if(sentence[i] == '\226' && sentence[i+1] == '\263')
                ;
            else if(sentence[i] == '\211' && sentence[i+1] == '\214')
                ;
            else
                i--;
            //sanjiaoxing & jiaodu& dushu
        }else if(sentence[i] == '\302'){
            i++;
            if(sentence[i] == '\260' && sentence[i+1] == '\357')
                ;
            else
                i--;
        }else
            break;
    }
    if(i == len)
        i = 0 ;
    char* result = (char*)malloc(sizeof(char)*(i+1));
    if(i!=0)
        memcpy(result,sentence,i);
    result[i] = '\0';
    return result ;
}

char* getLastString(char* sentence){
    int len = strlen(sentence);
    int index ;
    for(index = len-1 ; index >= 0 ; index--){
        if((sentence[index]>='a'&&sentence[index]<='z')||(sentence[index]>='A'&&sentence[index]<='Z')
                    ||(sentence[index]>='0'&&sentence[index]<='9')){
            continue ;
        }else if(sentence[index] == '\240' && sentence[index-1] == '\210' && sentence[index-2] == '\342'){
            index = index-2;
        }else if(sentence[index] == '\214' && sentence[index-1] == '\211' && sentence[index-2] == '\342'){
            index = index-2;
        }else if(sentence[index] == '\263' && sentence[index-1] == '\226' && sentence[index-2] == '\342'){
            index = index -2;
        }else if(sentence[index] == '\357' && sentence[index-1] == '\260' && sentence[index-2] == '\302'){
            index = index -2;
        }else
            break;
    }
    if(index<0)
        index = 0 ;
    char* result = (char*)malloc(sizeof(char)*(len-index));
    strcpy(result,sentence+index+1);
    return result;
}

bool isString(const char* sentence){
    int len,i;
    len = strlen(sentence);
    for(i = 0 ; i < len ; i++){
        if((sentence[i]>='a'&&sentence[i]<='z')||(sentence[i]>='A'&&sentence[i]<='Z')
            ||(sentence[i]>='0'&&sentence[i]<='9'))
            continue ;
        else
            return false ;
    }
    return true ;
}

void combineEquations(struct EQS *head , list<char*>* results){
    int currentMaxOp = 0 ;
    struct EQS *maxEquations,*current,*tofree_eq ;
    char* tmp ;
    if(head == NULL)
        return ;
    while(head->nexteq!=NULL){
        current = head ;
        maxEquations = head ;
        currentMaxOp = 0;
        while(current->nexteq!=NULL){
            if(current->op == 0)
                break ;
            if(current->op > currentMaxOp){
                currentMaxOp = current->op ;
                maxEquations = current ;
            }
            current = current->nexteq ;
        }
        if(head == current){
            head = current->nexteq ;
 //           free(current->content);
            delete current;
            continue ;
        }
        current = maxEquations;
        if(current->content==NULL||current->nexteq->content==NULL){
            head = current->nexteq; //error operate.
            
            continue ;
        }
        int currentLen = strlen(current->content);
        int nextLen = strlen(current->nexteq->content);
        if(current->beiyong_opname=='\0'){
            tmp = (char*)malloc(sizeof(char)*(currentLen+nextLen+2));
            strcpy(tmp,current->content);
            tmp[currentLen] = current->opname ;
            strcpy(tmp+currentLen+1,current->nexteq->content);
        }else{
            tmp = (char*)malloc(sizeof(char)*(currentLen+nextLen+3));
            strcpy(tmp,current->content);
            tmp[currentLen] = current->opname ;
            tmp[currentLen+1] = current->beiyong_opname;
            strcpy(tmp+currentLen+2,current->nexteq->content);
        }
//        free(current->content);
//        free(current->nexteq->content);
        current->content = tmp ;
        current->op = current->nexteq->op ;
        current->beiyong_opname = current->nexteq->beiyong_opname;
        current->opname = current->nexteq->opname ;
        tofree_eq = current->nexteq ;
        current->nexteq = current->nexteq->nexteq; 
        delete tofree_eq;
        results->push_back(current->content);
    }
}
void MathWordTool::cut(const char* sentence, list<char*>* resultlist){
    if(isString(sentence))
        return ;
	Parser p ;
	p.setSentence(sentence);
	Equation q ;
	q.regParser(&p);
    char cThis = '\n';
    char cEnd = '\n';
    char cNext = '\n';
    int buflastindex = 0; 
    int bufcurrentindex = 0;
    int op = 0 ; //must init. else op!=0 will execute with error message.
    char * word ;
    char* tmp ;
    char * cmd ;

    EQS *head;
    EQS *current; 
    vector<string> words ;
    vector<string>::iterator it;
//    CppJieba::MixSegment _segment("../dict/jieba.dict.utf8","../dict/hmm_model.utf8","./");
    head = new EQS();
    current = head ;
    list<char*> currentResult;
    while((cThis = p.getTexChar())!='\0'){
        bufcurrentindex = p.getIndex();
        switch(cThis){
            case '$':
                if(bufcurrentindex > (buflastindex+1)){
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex-1);
                    if(isString(word) == false){
                        _segment.cut(word,words);
                        for(it=words.begin();it!=words.end();it++){
                            word = (char*)malloc(sizeof(char)*(strlen((*it).c_str())+1));
                            strcpy(word,(*it).c_str());
                            resultlist->push_back(word);
                        }
                    }
                    for(it=words.begin();it!=words.end();it++){
                           word = (char*)malloc(sizeof(char)*(strlen((*it).c_str())+1));
                           strcpy(word,(*it).c_str());
                           resultlist->push_back(word);
                     }
              //      currentResult.push_back(word);
                }
                buflastindex = bufcurrentindex-1;
                cNext = p.getTexChar();
                if (cNext == '$'){
                    word = q.CmdEquation(EQN_DOLLAR_DOLLAR | ON);
                }else {
                    p.ungetTexChar();
                    word = q.CmdEquation(EQN_DOLLAR | ON);
                }
                bufcurrentindex = p.getIndex();
                currentResult.push_back(p.getChars(buflastindex,bufcurrentindex));
                buflastindex = bufcurrentindex ;
                if(current->content!=NULL){
                    current->op = 0 ; 
                    current->nexteq = new EQS();
                    current = current->nexteq ;
                }
                current->content = currentResult.back();
                cut(word,resultlist);
                break;
            case '\\':
                if(bufcurrentindex > buflastindex+1){
                    bufcurrentindex = p.getIndex();
                    word = p.getChars(buflastindex,bufcurrentindex-1);
                    currentResult.push_back(word);
                    if(isString(word) == false){
                        _segment.cut(word,words);
                        for(it=words.begin();it!=words.end();it++){
                            word = (char*)malloc(sizeof(char)*(strlen((*it).c_str())+1));
                            strcpy(word,(*it).c_str());
                            resultlist->push_back(word);
                        }
                    }
                }else{
                    word = NULL ;
                }
                buflastindex = bufcurrentindex -1; 
//                word = q.Cmd();
                cNext = p.getTexChar();
                if(cNext == '\\'){
                    buflastindex = p.getIndex();
                    break ;
                }else{
                    p.ungetTexChar();
                }
                cmd = p.getCmd();
                char* param ;
                if(strcmp(cmd,"frac")==0||strcmp(cmd,"dfrac")==0||strcmp(cmd,"Frac")==0){
                    param = q.CmdFraction(0);
                }else if(strcmp(cmd,"sqrt")==0||strcmp(cmd,"root")==0){
                    param = q.CmdRoot(0);
                }else if(strcmp(cmd,"lim")==0||strcmp(cmd,"limsup")==0||strcmp(cmd,"liminf")==0){
                    param = q.CmdLim(0);
                }else if(strcmp(cmd,"sum")==0||strcmp(cmd,"prod")==0||strcmp(cmd,"int")==0||strcmp(cmd,"iint")==0){
                    param = q.CmdIntegral(0);
                }else if(strcmp(cmd,"left")==0){
                    cNext = p.getTexChar();
                    if(cNext!='.')
                        p.ungetTexChar();
                    param = q.CmdLeftRight(0);
                }else if(strcmp(cmd,"stackrel")==0){
                    param = q.CmdStackrel(0);
                }else if(strcmp(cmd,"textsuperscript")==0){
                    param = q.CmdSuperscript(0);
                }else if(strcmp(cmd,"begin")==0){
                    param = q.CmdBeginEnd(0);
                }else if(strlen(cmd)>0){
                    param = p.getChars(p.getIndex(),strlen(sentence));
                    printf("cmd:%s\nparam:%s\n",cmd,param);
                    p.setIndex(strlen(sentence)+1);// to break
                }else{
                    break ;
                }
                if(word!=NULL&&strlen(word)>0){
                    if(current->content!=NULL){
                        current->op = 0 ;
                        current->nexteq = new EQS();
                        current = current->nexteq ;
                    }
                    current->content = word;
                    current->op = MULOP;
                    current->opname=' ';
                    current->nexteq = new EQS();
                    current = current->nexteq;
                }
                bufcurrentindex = p.getIndex();
                currentResult.push_back(p.getChars(buflastindex,bufcurrentindex));
                buflastindex = bufcurrentindex ;
                if(current->content!=NULL){
                    current->op = 0 ;
                    current->nexteq = new EQS();
                    current = current->nexteq ;
                }
                current->content = currentResult.back();
                cut(param,resultlist);
                break ;
            case '{':
                cEnd = '}';
                break;
            case '[':
                cEnd = ']';
                break;
            case '(':
                cEnd = ')';
                break;
            case '|':
                cEnd = '|';
                break;
            case '\357':
                cNext = p.getTexChar();
                if(cNext == '\274'){
                    cNext = p.getTexChar();
                    if(cNext == '\210'){
                        cThis = '(';
                        cEnd = '?';
                    }
                    else{
                        p.ungetTexChar();
                        p.ungetTexChar();
                    }
                }else{
                    p.ungetTexChar();
                }
                break;
            case '^':
                op = FLATOP ;
                break ;
            case '_':
                op = FLATOP;
                break;
            case '+':
                op = ADDOP;
                break; 
            case '-':
                op = PLUSOP;
                break;
            case '*':
                op = MULOP;
                break;
            case '/':
                op = DIVOP;
                break;
            case '\303':
                cNext = p.getTexChar();
                if(cNext=='\227'){ //Multiplication '\303'+'\227'
                    cThis = '*';
                    op = MULOP ;
                }else if(cNext=='\267'){ //Division '\303'+'\267'
                    cThis = '/';
                    op = DIVOP ;
                }else{
                    p.ungetTexChar();
                }
                break;
            case '\342': //dian cheng
                cNext = p.getTexChar();
                if(cNext != '\200')
                    p.ungetTexChar();
                else{
                    cNext = p.getTexChar();
                    if(cNext == '\242'){
                        cThis = '*';
                        op = MULOP;
                    }else{
                        p.ungetTexChar();
                        p.ungetTexChar();
                    }
                 }
                 break;
            case '>':
                op = BIGOP ;
                break;
            case '<':
                op = MINOP ;
                break;
            case '=':
                op = EQUALOP;
                break;
            case ',':
                if(bufcurrentindex > (buflastindex+1)){
                    if(current->content!=NULL){
                        current->op = 0 ;
                        current->nexteq = new EQS();
                        current = current->nexteq ;
                    }
                    word = p.getChars(buflastindex, bufcurrentindex-1);
                    current->content = word ;
                    buflastindex = p.getIndex();
                 }
                 break;
            case '.':
                if(bufcurrentindex > (buflastindex+1)){
                    if(current->content!=NULL){
                        current->op = 0 ;
                        current->nexteq = new EQS();
                        current = current->nexteq ;
                    }
                    word = p.getChars(buflastindex, bufcurrentindex-1);
                    current->content = word ;
                    buflastindex = p.getIndex();
                }
                break;                
            default:
                break ;
        }
        if(cEnd!='\n'){
            if(bufcurrentindex > (buflastindex+1)){
                if(current->content!=NULL){
                    current->op = 0 ;
                    current->nexteq = new EQS();
                    current = current->nexteq ;
                }
                word = p.getChars(buflastindex, bufcurrentindex-1);
                current->content = word ;
                if(isString(current->content)==false){
                    _segment.cut(current->content,words);
                    for(it=words.begin();it!=words.end();it++){
                        word = (char*)malloc(sizeof(char)*(strlen((*it).c_str())+1));
                        strcpy(word,(*it).c_str());
                        resultlist->push_back(word);
                    }
                }else
                    currentResult.push_back(current->content);
            }
            if(cThis == '(' && cEnd == '?' )
                buflastindex = p.getIndex()-3;
            else
                buflastindex = p.getIndex()-1;
            word = q.CmdKuoHao(cThis,cEnd);
            bufcurrentindex = p.getIndex();
            currentResult.push_back(p.getChars(buflastindex,bufcurrentindex));
            buflastindex = p.getIndex() ;
            if(current->content!=NULL){
                current->op = 0 ;
                current->nexteq = new EQS();
                current = current->nexteq ;
           }
            current->content = currentResult.back();
            cut(word,resultlist);
            cEnd = '\n';//handle '{}' and so on.
            buflastindex = p.getIndex();
        }
        if(op!=0){ 
            if(bufcurrentindex > (buflastindex+1)){
                if(current->content!=NULL){
                    current->op = 0 ;
                    current->nexteq = new EQS();
                    current = current->nexteq ;
                 }
                 tmp = p.getChars(buflastindex, bufcurrentindex-1);
                 if(isString(tmp)){
                    current->content = tmp ;
                    currentResult.push_back(current->content);
                 }else{
                    word = getBeginMathString(tmp);//if to end .then return null; 
                    if(strlen(word)>0){
                        current->content = word; 
                        current->op = 0;
                        current->nexteq = new EQS();
                        current = current->nexteq ;
                    }
                    current->content = getLastString(tmp);
                    _segment.cut(tmp,words);
                    for(it=words.begin();it!=words.end();it++){
                        word = (char*)malloc(sizeof(char)*(strlen((*it).c_str())+1));
                        strcpy(word,(*it).c_str());
                        resultlist->push_back(word);
                    }
                    free(tmp);
                 }
            } 
            cNext = p.getTexChar();
            if(cNext == '=')
                current->beiyong_opname = '=';
            else
                p.ungetTexChar();
            if(current->content == NULL){
                word = (char*)malloc(sizeof(char));
                word[0] = '\0';
                current->content = word ;
            }
            current->op = op ;
            current->opname = cThis ;
            current->nexteq = new EQS();
            current = current->nexteq ;
            buflastindex = p.getIndex();
            op = 0 ;
        }
    }
    //zhe bufen shifang caozuo zenme zuo 
    if(current->content!=NULL){
        current->op = 0 ;
        current->nexteq = new EQS();
        current = current->nexteq ;
    }
    current->content = p.getChars(buflastindex,bufcurrentindex);
    current->nexteq = NULL ;
    if(strlen(current->content)>0){
 //       currentResult.push_back(current->content);
        _segment.cut(current->content,words);
        for(it=words.begin();it!=words.end();it++){
            word = (char*)malloc(sizeof(char)*(strlen((*it).c_str())+1));
            strcpy(word,(*it).c_str());
            resultlist->push_back(word);
        }
    }
    combineEquations(head,&currentResult);
    list<char*>::iterator iter ;
    for(iter = currentResult.begin() ; iter != currentResult.end() ; iter++){
        resultlist->push_back(*iter);
    }
}
