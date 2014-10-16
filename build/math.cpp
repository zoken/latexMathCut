#include "mathwordtools.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "latexparser.h"
#include "equation.h"
#include "EQS.h"

#define ADDOP 1
#define PLUSOP 1
#define MULOP 2
#define DIVOP 2
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
        int currentLen = strlen(current->content);
        int nextLen = strlen(current->nexteq->content);
        tmp = (char*)malloc(sizeof(char)*(currentLen+nextLen+1));
        strcpy(tmp,current->content);
        tmp[currentLen] = current->opname ;
        strcpy(tmp+currentLen+1,current->nexteq->content);
//        free(current->content);
//        free(current->nexteq->content);
        current->content = tmp ;
        current->op = current->nexteq->op ;
        current->opname = current->nexteq->opname ;
        tofree_eq = current->nexteq ;
        current->nexteq = current->nexteq->nexteq; 
        delete tofree_eq;
        results->push_back(current->content);
    }
}
void MathWordTool::cut(const char* sentence, list<char*>* resultlist){
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
    char * cmd ;
    struct EQS *head;
    struct EQS *current; 
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
                    currentResult.push_back(word);
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
                }
                buflastindex = bufcurrentindex -1; 
//                word = q.Cmd();
                cNext = p.getTexChar();
                if(cNext == '\\'){
                    char fanxiegang[3];
                    fanxiegang[0] = '\\';
                    fanxiegang[1] = '\\';
                    fanxiegang[2] = '\0';
                    currentResult.push_back(fanxiegang);
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
                    continue ;
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
//                p.getWord(cThis,cEnd);
    /*
                bufcurrentindex = p.getIndex();
                currentResult.push_back(p.getChars(buflastindex,bufcurrentindex));
                buflastindex = bufcurrentindex ;
                if(current->content!=NULL){
                    current->op = 0 ;
                    current->nexteq = new EQS();
                    current = current->nexteq ;
                }
                current->content = currentResult.back();
                */
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
            case '^':
                break ;
            case '_':
                break;
            default:
                break ;
        }
        if(cEnd!='\n'){
            cEnd = '\n';//handle '{}' and so on.
        }
        if(op!=0){ 
            if(bufcurrentindex > (buflastindex+1)){
                if(current->content!=NULL){
                    current->op = 0 ;
                    current->nexteq = new EQS();
                    current = current->nexteq ;
                 }
                 current->content = p.getChars(buflastindex, bufcurrentindex-1);
                 currentResult.push_back(current->content);
            }                                                                                                                                                                                              current->op = op ;
            current->opname = cThis ;
            current->nexteq = new EQS();
            current = current->nexteq ;
            buflastindex = p.getIndex();
            op = 0 ;
        }
    }
    if(current->content!=NULL){
        current->op = 0 ;
        current->nexteq = new EQS();
        current = current->nexteq ;
    }
    current->content = p.getChars(buflastindex,bufcurrentindex);
    current->nexteq = NULL ;
    if(strlen(current->content)>0)
        currentResult.push_back(current->content);
    combineEquations(head,&currentResult);
    list<char*>::iterator iter ;
    for(iter = currentResult.begin() ; iter != currentResult.end() ; iter++){
        resultlist->push_back(*iter);
    }
}
int main(){
    MathWordTool mwt ;
    list<char*> resultlist ;
    mwt.cut("$1+1*\\dfrac{1}{2}*\\sqrt{3}$",&resultlist);
    list<char*>::iterator iter ;
    for(iter=resultlist.begin();iter!=resultlist.end();iter++){
        cout << *iter << endl;
    }
}   
