/* parser.c - parser for LaTeX code

Copyright (C) 1998-2002 The Free Software Foundation

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

This file is available from http://sourceforge.net/projects/latex2rtf/
 
Authors:
    1998-2000 Georg Lehner
    2001-2002 Scott Prahl
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "latexparser.h"
extern "C"
{
#include "util.h"
#include "tools.h"
}
typedef struct InputStackType
{
   char  *string;
   char  *string_start;
   FILE  *file;
   char  *file_name;
   long   file_line;
} InputStackType;

#define PARSER_SOURCE_MAX 100

static InputStackType   g_parser_stack[PARSER_SOURCE_MAX];

static int              g_parser_depth = -1;
static char            *g_parser_string = "stdin";
static FILE            *g_parser_file   = NULL;
static int 				g_parser_line   = 1;
static int 				g_parser_include_level=0;

static char             g_parser_currentChar;	/* Global current character */
static char             g_parser_lastChar;
static char             g_parser_penultimateChar;
static int				g_parser_backslashes;

#define TRACK_LINE_NUMBER_MAX 10
static int				g_track_line_number_stack[TRACK_LINE_NUMBER_MAX];
static int				g_track_line_number=-1;

static void     parseBracket();


#define CR (char) 0x0d
#define LF (char) 0x0a

char* Parser::getCmd(){
    char buf[20];
    int num = 0 ;
    char c ;
    c = getTexChar();
    if(c=='{'){
        buf[num++]= c;
        
    }else{
        while((c<='z'&&c>='a')||(c<='Z'&&c>='A')){
            buf[num++] = c ;
            c = getTexChar();
        }   
        ungetTexChar();
    }
    buf[num++]='\0';
    char* ret = (char*)malloc(sizeof(char)*num);
    strcpy(ret,buf);
    return ret ;
}
int Parser::getIndex(){
	return readIndex;
}
void Parser::setIndex(int index){
	readIndex = index ;
}
char* Parser::getChars(int beg,int end){
	if(beg<0||beg>=end)
		return "";
    if(end > this->sentencelen)
        end = this->sentencelen ;
	char* ret = (char*) malloc(sizeof(char)*(end-beg+1));//do not contain end.
	memcpy(ret,this->sentence+beg,end-beg);
	ret[end-beg]='\0';
	return ret ;
}
void Parser::setSentence(const char* _sentence){
	if(_sentence == 0){
		this->readIndex = 0 ; 
		this->sentencelen = 0 ;
		return ;
	}else{
		this->sentence = _sentence ;
		this->readIndex = 0 ;
		this->sentencelen = strlen(_sentence);
	}
}

char Parser::getRawTexChar()
/***************************************************************************
 purpose:     get the next character from the input stream with minimal
              filtering  (CRLF or CR or LF ->  \n) and '\t' -> ' '
			  it also keeps track of the line number
              should only be used by \verb and \verbatim and getTexChar()
****************************************************************************/
{
	char	thechar;
	if(readIndex >= sentencelen){
        readIndex++;
		return '\0';
	}else{
		thechar = sentence[readIndex++];
		return thechar; 
	}
}

char Parser::getTexChar()
/***************************************************************************
 purpose:     get the next character from the input stream
              This should be the usual place to access the LaTeX file
			  It filters the input stream so that % is handled properly
****************************************************************************/
{
	char            cThis;
	char            cSave = g_parser_lastChar;
	char            cSave2 = g_parser_penultimateChar;

	cThis = getRawTexChar();
	while (cThis == '%' && g_parser_backslashes % 2 == 0) {
		skipToEOL();
		g_parser_penultimateChar = cSave2;
		g_parser_lastChar = cSave;
		cThis = getRawTexChar();
	}
	
	if (cThis == '\\') 
		g_parser_backslashes++;
	else
		g_parser_backslashes=0;
	return cThis;
}
void Parser::ungetTexChar(){
	if(readIndex>0)
		readIndex -= 1 ;

}

void Parser::skipToEOL(void)
/****************************************************************************
purpose: ignores anything from inputfile until the end of line.  
         uses getRawTexChar() because % are not important
 ****************************************************************************/
{
	char	cThis;
	while ((cThis=getRawTexChar()) && cThis != '\n'){}
}

char Parser::getNonBlank(void)
/***************************************************************************
 Description: get the next non-blank character from the input stream
****************************************************************************/
{
	char	c;
	while ((c = getTexChar()) && (c == ' ' || c == '\n')){}
	return c;
}

char Parser::getNonSpace(void)
/***************************************************************************
 Description: get the next non-space character from the input stream
****************************************************************************/
{
	char	c;
	while ((c = getTexChar()) && c == ' '){}
	return c;
}

void Parser::skipSpaces(void)
/***************************************************************************
 Description: skip to the next non-space character from the input stream
****************************************************************************/
{
	char            c;
	while ((c = getTexChar()) && c == ' '){}
	ungetTexChar();
}

int Parser::getSameChar(char c)
/***************************************************************************
 Description: returns the number of characters that are the same as c
****************************************************************************/
{
	char            cThis;
	int 			count=-1;
	
	do {
		cThis = getTexChar();
		count++;
	} while (cThis == c);

	ungetTexChar();

	return count;
}

char* Parser::getDelimitedText(char left, char right, bool raw)
/******************************************************************************
  purpose: general scanning routine that allocates and returns a string
  		   that is between "left" and "right" that accounts for escaping by '\'
  		   
  		   Example for getDelimitedText('{','}',TRUE) 
  		   
  		   "{the \{ is shown {\it by} a\\}" ----> "the \{ is shown {\it by} a\\"
  		    
  		    Note the missing opening brace in the example above
 ******************************************************************************/
{
	char            buffer[5000];
	int				size = -1;
	int				lefts_needed = 1;
	char			marker = ' ';
	char			last_char = ' ';
	while (lefts_needed && size < 4999) {

		size++;
		last_char = marker;
		buffer[size] = (raw) ? getRawTexChar() : getTexChar();
		marker = buffer[size];

		if (buffer[size] != right || last_char == '\\') {    	/* avoid \}  */
			if (buffer[size] == left && last_char != '\\') 		/* avoid \{ */
				lefts_needed++;
			else {
				if (buffer[size] == '\\' && last_char == '\\')   /* avoid \\} */
					marker = ' ';
			}
		} else 
			lefts_needed--;
	}
	buffer[size] = '\0';		/* overwrite final delimeter */
	if (size == 4999)
		diagnostics(ERROR, "Misplaced '%c' (Not found within 5000 chars)");
		
	return strdup(buffer);
}

void Parser::parseBrace(void)
/****************************************************************************
  Description: Skip text to balancing close brace                          
 ****************************************************************************/
{
	char *s = getDelimitedText('{','}',FALSE);	
	free(s);
}

void Parser::parseBracket(void)
/****************************************************************************
  Description: Skip text to balancing close bracket
 ****************************************************************************/
{
	char *s = getDelimitedText('[',']',FALSE);
	free(s);
}

void Parser::CmdIgnoreParameter(int code)
/****************************************************************************
   Description: Ignore the parameters of a command 
   Example    : CmdIgnoreParameter(21) for \command[opt1]{reg1}{reg2}

   code is a decimal # of the form "op" where `o' is the number of
   optional parameters (0-9) and `p' is the # of required parameters.    
                                                
   The specified number of parameters is ignored.  The order of the parameters
   in the LaTeX file does not matter.                      
****************************************************************************/
{
	int             optParmCount = code / 10;
	int             regParmCount = code % 10;
	char            cThis;
	
	diagnostics(4, "CmdIgnoreParameter [%d] {%d}", optParmCount, regParmCount);

	while (regParmCount) {
		cThis = getNonBlank();
		switch (cThis) {
		case '{':

			regParmCount--;
			parseBrace();
			break;

		case '[':

			optParmCount--;
			parseBracket();
			break;

		default:
			diagnostics(WARNING,"Ignored command missing {} expected %d - found %d", code%10, code%10-regParmCount);
			ungetTexChar();
			return;
		}
	}

	/* Check for trailing optional parameter e.g., \item[label] */

	if (optParmCount > 0) {
		cThis=getNonSpace();
		if (cThis == '[') 
			parseBracket();
		else {
			ungetTexChar();
			return;
		}
	}
	return;
}

char* Parser::getSimpleCommand(void)
/**************************************************************************
     purpose: returns a simple command e.g., \alpha\beta will return "\beta"
                                                   ^
 **************************************************************************/
{
	char            buffer[128];
	int             size;

	buffer[0] = getTexChar();

	if (buffer[0] != '\\')
		return NULL;

	for (size = 1; size < 127; size++) {
		buffer[size] = getRawTexChar();    /* \t \r '%' all end command */

		if (!isalpha((int)buffer[size])) {
			ungetTexChar();
			break;
		}
	}

	buffer[size] = '\0';
	if (size == 127){
		diagnostics(WARNING, "Misplaced brace.");
		diagnostics(ERROR, "Cannot find close brace in 127 characters");
	}
	
	diagnostics(5, "getSimpleCommand result <%s>", buffer);
	return strdup(buffer);
}

char* Parser::getBracketParam(void)
/******************************************************************************
  purpose: return bracketed parameter
  			
  \item[1]   --->  "1"        \item[]   --->  ""        \item the  --->  NULL
       ^                           ^                         ^
  \item [1]  --->  "1"        \item []  --->  ""        \item  the --->  NULL
       ^                           ^                         ^
 ******************************************************************************/
{
	char            c, *text;
	
	c = getNonBlank();

	if (c == '[') {
		text = getDelimitedText('[',']',FALSE);
		diagnostics(5, "getBracketParam [%s]", text);

	} else {
		ungetTexChar();
		text = NULL;
		diagnostics(5, "getBracketParam []");
	}
	
	return text;
}

char* Parser::getBraceParam(void)
/**************************************************************************
     purpose: allocates and returns the next parameter in the LaTeX file
              Examples:  (^ indicates the current file position)
              
     \alpha\beta   --->  "\beta"             \bar \alpha   --->  "\alpha"
           ^                                     ^
     \bar{text}    --->  "text"              \bar text     --->  "t"
         ^                                       ^
	_\alpha        ---> "\alpha"             _{\alpha}     ---> "\alpha"
	 ^                                        ^
	_2             ---> "2"                  _{2}          ---> "2"
	 ^                                        ^
 **************************************************************************/
{
	char            s[2], *text;
	
	s[0] = getNonSpace();			/*skip spaces and one possible newline */
	if (s[0] == '\n')
		s[0] = getNonSpace();	

//	PushTrackLineNumber(FALSE);
	
	if (s[0] == '\\') {
		ungetTexChar();
		text=getSimpleCommand();

	} else 	if (s[0] == '{') 
		text = getDelimitedText('{','}',TRUE);
	
	else {
		s[1] = '\0';
		text = strdup(s);
	}
		
//	PopTrackLineNumber();
	diagnostics(5, "Leaving getBraceParam {%s}", text);
	return text;
}

char* Parser::getTexUntil(char * target, int raw)
/**************************************************************************
     purpose: returns the portion of the file to the beginning of target
     returns: NULL if not found
 **************************************************************************/
{
	enum {BUFFSIZE = 8000};
	char            *s;
	char            buffer[BUFFSIZE+1] = {'\0'};
	int				last_i = -1;
	int             i   = 0;       /* size of string that has been read */
	int             j   = 0;       /* number of found characters */
	bool			end_of_file_reached = FALSE;
	int             len = strlen(target);
	
//	PushTrackLineNumber(FALSE);

	diagnostics(5, "getTexUntil target = <%s> raw_search = %d ", target, raw);
	while (j < len && i < BUFFSIZE) {
		if (i > last_i) {
			buffer[i] = (raw) ? getRawTexChar() : getTexChar();
			last_i = i;
			if (buffer[i]!='\n')
				diagnostics(7,"next char = <%c>, %d, %d, %d",buffer[i],i,j,last_i);
			else
				diagnostics(7,"next char = <\\n>");

		}
		if (buffer[i]=='\0') {
			end_of_file_reached = TRUE;
			diagnostics(7,"end of file reached");
			break;
		}
		if (buffer[i] != target[j]) {
			if (j > 0) {			        /* false start, put back what was found */
				diagnostics(8,"failed to match target[%d]=<%c> != buffer[%d]=<%c>", j,target[j], i,buffer[i]);
				i-=j;
				j=0;
			}
		} else {
			j++;
        }

		i++;
	}
	
	if (i == BUFFSIZE) 
		diagnostics(ERROR, "Could not find <%s> in %d characters", BUFFSIZE);
	
	if (!end_of_file_reached) /* do not include target in returned string*/
		buffer[i-len] = '\0';
	
//	PopTrackLineNumber();

	diagnostics(3,"buffer size =[%d], actual=[%d]", strlen(buffer), i-len);

	s = strdup(buffer);
	diagnostics(3,"strdup result = %s",s);
	return s;
}
