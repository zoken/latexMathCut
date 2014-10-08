/*
 * Description: Contains declarations for generic recursive parsering
 * routines and other help routines for parsing LaTeX code
 * 
 * 26th June 1998 - Created initial version - fb                            LEG
 * 070798 adapted Frank Barnes contribution to r2l coding conventions SAP
 */
//for file input.del at 20141006
//char 		   *CurrentFileName(void);
//int 			PushSource(char * filename, char * string);
//int				StillSource(void);
//void 			PopSource(void);

//at input string
class Parser{
private:
		const char* sentence;
		int readIndex ;
		int sentencelen ;
public:
	int getIndex();
	void setIndex(int index);
	char* getChars(int beg,int end);
	void	setSentence(const char* sentence);
	char	getRawTexChar(void);
	char	getTexChar(void);
	char	getNonSpace(void);
	char	getNonBlank(void);
	int		getSameChar(char c);

	void            ungetTexChar();
	void            skipToEOL(void);
	void            skipSpaces(void);

	void            CmdIgnoreParameter(int);
	char           *getBraceParam(void);
	char           *getBracketParam(void);
	char           *getSimpleCommand(void);
	char           *getTexUntil(char * target, int raw);
	int             getDimension(void);
	void			parseBrace(void);
	void			parseBracket(void);
	char		   *getDelimitedText(char left, char right, bool raw);
	void			getSection(char **body, char **header, char **label);
};



//for file's lines. del at 20141006
//int 			CurrentLineNumber(void);
//void 			PushTrackLineNumber(int flag);
//void			PopTrackLineNumber(void);
//void 			UpdateLineNumber(char *s);
