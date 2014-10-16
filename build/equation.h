#define EQN_DOLLAR         2	/* ('$')  */
#define EQN_RND_OPEN       3	/* ('/(') */
#define EQN_BRACKET_OPEN   4	/* ('/[') */
#define EQN_RND_CLOSE      5	/* ('/)') */
#define EQN_BRACKET_CLOSE  6	/* ('/]') */
#define EQN_ARRAY          8	/* eqnarray environment */
#define EQN_ARRAY_STAR     9	/* eqnarray* environment */
#define EQN_EQUATION      10	/* equation environment */
#define EQN_EQUATION_STAR 11  /* equation* environment */
#define EQN_MATH          12  /* \begin{math} ... \end{math} */
#define EQN_DISPLAYMATH   13  /* \begin{displaymath} ... \end{displaymath} */
#define EQN_DOLLAR_DOLLAR 14  /* \begin{displaymath} ... \end{displaymath} */
#define EQN_NO_NUMBER     15	/* \nonumber */
#define ON 0x4000
#define OFF 0x0000

class Equation{
private:
	Parser* parser ;
public:
	void			regParser(Parser* _parser);
    void            CmdChars();
    char*           CmdKuoHao(char start,char end);
	char*			CmdEquation(int code);
	char*           CmdFraction(int code);
	char*           CmdRoot(int code);
	char*           CmdLim(int code);
	char*           CmdIntegral(int code);
	char*           CmdSuperscript(int code);
	char*           CmdSubscript(int code);
	char*           CmdNonumber(int code);
	char*			CmdArray(int code);
	char*			CmdLeftRight(int code);
	char*			CmdStackrel(int code);
    char*           CmdBeginEnd(int code);
	int				EquationNeedsFields(char *eq);
	void			SlurpEquation(int code, char **pre, char **eq, char **post);
	char*			SlurpDollarEquation(void);

};
