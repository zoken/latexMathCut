#include "mathwordtools.h"
#include <stdlib.h>
#include <string.h>
#include "latexparser.h"
#include "equation.h"
extern "C"{
#include "util.h"
#include "tools.h"
}
void handleBuf(char* buf,int bufindex,list<char*>* resultlist){
    if(bufindex<=1) //if =1 then . the new add data is not avaliable. so del.
        return ;
    buf[bufindex-1] = '\0';
    char * ret = (char*)malloc(sizeof(char)*bufindex);
    strcpy(ret,buf);
    resultlist->push_back(ret);
    return ;
}

void MathWordTool::cut(const char* sentence, list<char*>* resultlist){
	Parser p ;
	p.setSentence(sentence);
	Equation q ;
	q.regParser(&p);
    char cThis = '\n';
	char cLast = '\0';
	char cnext,cnext_next;
	char* cmd,*tmpsentence;
    char buf[1024];
    int lastindex =0, multiindex=0; 
    int index,bufindex = 0,tmpindex ;
    list<char*> tmpeq ;
    char* tmp;
    char* tmq;

    tmpsentence = (char*)malloc(sizeof(char)*(strlen(sentence)+1));
    strcpy(tmpsentence,sentence);
    resultlist->push_back(tmpsentence);
	while ((cThis = p.getTexChar()) && cThis != '\0') {
        buf[bufindex++] = cThis;
		switch (cThis) {
		case '\\':
            index = p.getIndex()-1;
            handleBuf(buf,bufindex,resultlist);
            if(!tmpeq.empty()){
                buf[bufindex-1]='\0';
                tmp = (char*)malloc(sizeof(char)*(bufindex-1));
                strcpy(tmp,buf);
                if(strlen(tmp) > 1)
                    tmpeq.push_back(tmp);
                if(tmp[strlen(tmp)]=='+'||tmp[strlen(tmp)]=='-'){
                    tmq = (char*)malloc(sizeof(char)*strlen(tmp));
                    tmp[strlen(tmp)]='\0';
                    strcpy(tmq,tmp);
                    tmpeq.pop_back();
                    tmpeq.push_back(tmq);
                }
            }
            bufindex = 0 ;
            cnext = p.getTexChar();
            if(cnext == '\\'){
                resultlist->push_back("\\\\");
                break;
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
                cnext = p.getTexChar();
                if(cnext!='.')
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
                p.setIndex(strlen(sentence)+1);
            }else{
                index = p.getIndex();
				param="";
                continue ;
			}
            tmpsentence = p.getChars(index,p.getIndex());
            if(strlen(tmpsentence)!=strlen(sentence))
                resultlist->push_back(tmpsentence);
            cut(param,resultlist);
			break;
		case '{':
            handleBuf(buf,bufindex,resultlist);
            bufindex = 0 ;
            tmp = p.getDelimitedText('{','}',false);
            resultlist->push_back(strdup_together("{}",tmp));
            cut(tmp,resultlist);
			//PushBrace();
			break;
	    case '|':
            handleBuf(buf,bufindex,resultlist);
            bufindex = 0 ;
            tmp = p.getTexUntil("|",false);
            resultlist->push_back(strdup_together("{}",tmp));
            cut(tmp,resultlist);
            break;
		case '}':
			//PopBrace();
			break;
		case '\n': 
			//as ' ' continue ;
			break;


		case '$':
            handleBuf(buf,bufindex,resultlist);
            bufindex = 0 ;
			cnext = p.getTexChar();
			char* eq ;
			if (cnext == '$'){
				eq = q.CmdEquation(EQN_DOLLAR_DOLLAR | ON);
			}
			else {
				p.ungetTexChar();
				eq = q.CmdEquation(EQN_DOLLAR | ON);
			}
            if(strlen(eq)<strlen(sentence)-2)
                resultlist->push_back(eq);
            cut(eq,resultlist);
            break;
		case '&':
			// add new equation?
			break ;

		case '~':
			//continue;
			break;			
			
		case '^':
            char* superparam ;
            buf[bufindex]='\0';
            bufindex = 0 ;
			superparam = q.CmdSuperscript(0);//ping fang ?
            printf("superparam:%s\n",superparam);
            resultlist->push_back(strdup_together(buf,superparam));
			break;

		case '_':		
			char* subparam ;
            subparam = q.CmdSubscript(0);
            resultlist->push_back(subparam);
			break;

		case '-':
            handleBuf(buf,bufindex,resultlist);
            buf[bufindex-1]='-';
            buf[bufindex]='\0';
            tmp = (char*)malloc(sizeof(char)*bufindex);
            strcpy(tmp,buf);
            bufindex = 0 ;
            tmpeq.push_back(tmp);
            lastindex = p.getIndex();
            resultlist->push_back("-");
			//what is it?
			break;
        case '+':
            handleBuf(buf,bufindex,resultlist);
            buf[bufindex-1]='+';
            buf[bufindex]='\0';
            tmp = (char*)malloc(sizeof(char)*bufindex);
            strcpy(tmp,buf);
            bufindex = 0 ;
            tmpeq.push_back(tmp);
            lastindex = p.getIndex();
            resultlist->push_back("+");
			break;			
        case '\303':
            cnext = p.getTexChar();
            if(cnext=='\227'){ //Multiplication '\303'+'\227'
                handleBuf(buf,bufindex,resultlist);
                bufindex = 0 ;
                lastindex = p.getIndex();
                resultlist->push_back("/*");
            }else if(cnext=='\267'){ //Division '\303'+'\267'
                handleBuf(buf,bufindex,resultlist);
                bufindex = 0 ;
                lastindex = p.getIndex();
                resultlist->push_back("//");
            }else{
                p.ungetTexChar();
            }
            break;
        case '\342':
            tmpindex = p.getIndex()-1;
            cnext = p.getTexChar();
            cnext_next = p.getTexChar();
            if(cnext == '\210'&&cnext_next=='\240'){//jiaodu
                q.CmdChars();
                handleBuf(buf,bufindex,resultlist);
                bufindex = 0 ;
                resultlist->push_back(p.getChars(tmpindex,p.getIndex()));
            }
            else if(cnext == '\226'&&cnext_next=='\263'){//sanjiaoxing
                q.CmdChars();
                handleBuf(buf,bufindex--,resultlist);
                bufindex = 0 ;
                resultlist->push_back(p.getChars(tmpindex,p.getIndex()));
            }
            else{
                p.ungetTexChar();
                p.ungetTexChar();
            }
            break;
		case '\'':
			//continue ;
			break;
			
		case '`':
			//continue ;
			break;
			
		case '\"':
			//continue ;
			break;

		case '<':
			//continue ;
			break;

		case '>':
			//continue ;
			break;

		case '!':
			//continue ;
			break;	
			
		case '?':
			//continue ;
			break;
			
		case ':':
			//continue ;
			break;	

		case '.':
			// continue ;
			break;
			
		case '\t':
			diagnostics(WARNING, "This should not happen, ignoring \\t");
			cThis = ' ';
		case '\r':
			diagnostics(WARNING, "This should not happen, ignoring \\r");
			cThis = ' ';
		case '%':
			diagnostics(WARNING, "This should not happen, ignoring %%");
			cThis = ' ';
        case ' ':
            bufindex--;//ignore ' '
            break;
		case '(':
			//continue ;
			break;
			
		case ')':
			//continue ;
			break;

		case ';':
			//continue ;
			break;

		case ',':
			//continue ;/+

			break;
		case '=':
			handleBuf(buf,bufindex,resultlist);
            buf[bufindex-1] = '\0';
            tmp = (char*)malloc(sizeof(char)*bufindex);
            strcpy(tmp,buf);
            tmpeq.push_back(tmp);
            bufindex = 0 ;
            resultlist->push_back("=");
			break ;
		default:
			//continue;
			//add to buf.
//			buf[bufIndex++]=cThis;
			break;
		}
		cLast = cThis;
	}
    if(bufindex < p.getIndex()-1){
        buf[bufindex++] = '\0';
        handleBuf(buf,bufindex,resultlist);
    }
 //   tmpeq.push_back(p.getChars(lastindex,p.getIndex()));
    char* tmpstr =NULL  ;
    char* tmpstrfinal = NULL ;
    while(!tmpeq.empty()){
        if(tmpstr == NULL){
            tmpstr = tmpeq.front();
            tmpeq.pop_front();
            if(tmpstr[strlen(tmpstr)-1]=='+'||tmpstr[strlen(tmpstr)-1]=='-')
                ;
            else
                tmpstr = NULL ;
            continue ;
        }
        else
            tmpstr = strdup_together(tmpstr,tmpeq.front());
        tmpeq.pop_front();
        if(tmpstr[strlen(tmpstr)-1]=='+'||tmpstr[strlen(tmpstr)-1]=='-'){
            tmpstrfinal = (char*)malloc(sizeof(char)*(strlen(tmpstr)+1));
            strcpy(tmpstrfinal,tmpstr);
            tmpstrfinal[strlen(tmpstr)-1]='\0';
            resultlist->push_back(tmpstrfinal);
        }else{
            resultlist->push_back(tmpstr);
            tmpstr = NULL ;
        }
    }
    if(tmpstr == NULL){
        ;
    }else if(tmpstr[strlen(tmpstr)-1]=='+'||tmpstr[strlen(tmpstr)-1]=='-' ){
       tmpstr = strdup_together(tmpstr,p.getChars(lastindex,p.getIndex()));
       if(strcmp(tmpstr,sentence)!=0)
            resultlist->push_back(tmpstr);
    }else
        ;
}
