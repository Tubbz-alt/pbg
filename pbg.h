/********************************************************************************
 * Lightweight Prefix Boolean Grammar (PBG) C Library                           *
 * By Tyler J. Daddio                                                           *
 *                                                                              *
 * ABOUT                                                                        *
 *                                                                              *
 *    PBG (Prefix Boolean Grammar) is a simple grammar for writing boolean      *
 *    expressions. The grammar itself is supercharged by a variable dictionary. *
 *    In this particular implementation, a simplified JSON file acts serves as  *
 *    the dictionary.                                                           *
 *                                                                              *
 * THE GOALS                                                                    *
 *                                                                              *
 *    PBG is built with three goals in mind.                                    *
 *                                                                              *
 *    First, it must be easy to interpret. The use of commas to separate        *
 *    identifiers permits easy parsing while still allowing for whitespace      *
 *    insensitivity. Prefix notation also significantly reduces overhead in     *
 *    comparison to infix notation.                                             *
 *                                                                              *
 *    Second, it must be unambigous. Universal prefix notation combined with    *
 *    grouping parentheses eliminates the need for operator precedence. Of      *
 *    course this mandates the near-universal use of grouping parentheses,      *
 *    which some find verbose and unreadable. However, this strictness          *
 *    transforms would-be bugs into syntax errors, which is nice.               *
 *                                                                              *
 *    Third, it must be expressive. To achieve this, values may be resolved via *
 *    literals or via dictionary lookup. These values may have any number of    *
 *    built-in operations applied to them. Additionally, PBG optionally         *
 *    supports the use of Prefix Arithmetic Grammar to produce arbitrary        *
 *    numeric expressions. This particular implementation supports this.        *
 *                                                                              *
 * THE GRAMMAR                                                                  *
 *                                                                              *
 *    EXP                                                                       *
 *        =   (!,EXP)                # Inverts truth value of EXP.              *
 *        =   (&,EXP,EXP[,EXP]*)     # Boolean AND of each EXP.                 *
 *        =   (|,EXP,EXP[,EXP]*)     # Boolean OR of each EXP.                  *
 *        =   (=,ANY,ANY[,ANY]*)     # Equal.                                   *
 *        =   (!=,ANY,ANY)           # Not equal.                               *
 *        =   (<,VALUE,VALUE)        # Less than.                               *
 *        =   (>,VALUE,VALUE)        # Greater than.                            *
 *        =   (<=,VALUE,VALUE)       # Less than or equal.                      * 
 *        =   (>=,VALUE,VALUE)       # Greater than or equal.                   *
 *        =   (?,[KEY])              # Is KEY defined in the dictionary?        *
 *        =   TRUE                   # The TRUE expression.                     *
 *        =   FALSE                  # The FALSE expression.                    *
 *     ANY                                                                      *
 *        =   VALUE                                                             *
 *        =   STRING                                                            *
 *     VALUE                                                                    *
 *        =   NUMBER                                                            *
 *        =   DATE                                                              *
 *     DATE                                                                     *
 *        =   YYYY                                                              *
 *        =   YYYY-MM                                                           *
 *        =   YYYY-MM-DD                                                        *
 *        =   YYYY-MM-DD@hh:mm                                                  *
 *        =   YYYY-MM-DD@hh:mm:ss                                               *
 *        =   [KEY]                                                             *
 *     NUMBER                                                                   *
 *        =   [+/-]((0/(1-9))[0-9]*)[.(0-9)[0-9]*][(e/E)[+/-](0-9)[0-9]*]       *
 *        =   (#,numeric expression)                                            *
 *        =   [KEY]                                                             *
 *     STRING                                                                   *
 *        =   '.*'                                                              *
 *        =   [KEY]                                                             *
 *     [KEY]                                                                    *
 *       =   Resolved via dictionary lookup.                                    *
 *                                                                              *
 ********************************************************************************/

/**
 * These are the PBG operators supported by this implementation. They can be
 * cross-referenced with the grammar pseudocode above. Note: some values are
 * no-ops, i.e. they are symbolic values used to indicate leaves in the
 * expression tree.
 */
enum pbg_op {
	_TRUE,    // not an op, denotes the TRUE literal
	_FALSE,   // not an op, denotes the FALSE literal
	_NUMBER,  // not an op, denotes a number literal
	_STRING,  // not an op, denotes a string literal
	_DATE,    // not an op, denotes a date literal
	_KEY,     // not an op, denotes a key string
	NOT,   // !
	AND,   // &
	OR,    // |
	EQ,    // =
	NEQ,   // !=
	LT,    // <
	GT,    // >
	LTE,   // <=
	GTE,   // >=
	EXST;  // ?
};

/* This struct represents a DATE value. */
typedef struct {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
} pbe_date;

/**
 * This struct represents a Prefix Boolean Expression. The value of the
 * operator constrains both the number of children and the type of children.
 */
typedef struct {
	int     degree;    /* Number of children. */
	void*   children;  /* Array of children. */
	pbg_op  op;        /* Operator to be applied. */
} pbe;

/**
 * Parses the given date and stores it into the provided date struct.
 * @param date The date struct to return to.
 * @param str  String to parse.
 * @param n    Length of the string.
 */
void pbe_date(pbe_date* date, char* str, int n)
{

}

/**
 * Parses the string as a boolean expression in Prefix Boolean Grammar.
 * @param e   The boolean expression instance to initialize.
 * @param str String to parse.
 * @param n   Length of the string.
 */
void pbe_parse(pbe* e, char* str, int n)
{
	if(IS_DATE(str, n)) {
		pbe->degree = 1;
		pbe->children = malloc(sizeof(pbe_date));
		pbe_date((pbe_date*)pbe->children, str, n);
		pbe->op = _DATE;
		
	}else if(IS_NUMBER(str, n)) {
		pbe->degree = 1;
		pbe->children = malloc(sizeof(double));
		char old = str[n];
		str[n] = '\0';
		pbe->children[0] = atof(str);
		str[n] = old;
		pbe->op = _NUMBER;
		
	}else if(IS_STRING(str, n)) {
		pbe->degree = 1;
		pbe->children = malloc(n+1);
		char old = str[n];
		str[n] = '\0';
		strcpy(pbe->children, str);
		str[n] = old;
		pbe->op = _STRING;
	
	}else if(IS_TRUE(str, n)) {
		pbe->degree = 0;
		pbe->children = NULL;
		pbe->op = _TRUE;
	
	}else if(IS_FALSE(str, n)) {
		pbe->degree = 0;
		pbe->children = NULL;
		pbe->op = _FALSE;

	}else if(IS_KEY(str, n)) {
		pbe->degree = 0;
		pbe->children = malloc(n+1);
		char old = str[n];
		str[n] = '\0';
		strcpy(pbe->children, str);
		str[n] = old;
		pbe->op = _KEY;
		
	}else{
		// Handle all operators!
	}
}

/**
 * Evaluates the Prefix Boolean Expression with the provided assignments.
 * @param e The PBE to evaluate.
 */
void pbe_evaluate(pbe* e)
{
	
}

/**
 * Prints the Prefix Boolean Expression to the char pointer provided. If ptr is
 * NULL and n is 0, then this function will allocate memory on the heap which
 * must then be free'd by the caller. In this way, this function behaves in a
 * manner reminiscent of fgets.
 * @param e      The PBE to print.
 * @param bufptr The pointer to the output buffer.
 * @param n      The length of the buffer pointed to by bufptr.
 * @return Number of characters written to the buffer.
 */
void pbe_gets(pbe* e, char** bufptr, int n)
{
	
}
