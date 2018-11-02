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
 * @return 0 if successful, an error code if unsuccessful.
 */
int pbe_parse(pbe* e, char* str, int n)
{
	int i, j;
	
	/* Initialize provided struct. */
	pbe->degree = 0;
	
	/* Every internal node is surrounded by one pair of parentheses. */
	if(str[0] == '(' && str[n-1] == ')') {
		/* Adjust string bounds appropriately. */
		str++, n--;
		
		/* Count number of children nodes. */
		int depth = 0;
		for(i = 0; i < n && depth >= 0; i++)
			if(str[i] == '(') depth++;
			else if(str[i] == ')') depth--;
			else if(str[i] == ',' && depth == 0)
				pbe->degree++;
		if(depth) {
			// TODO unbalanced parentheses
		}
		if(pbe->degree == 0) {
			// TODO no arguments provided to operator
		}
		
		/* Allocate space for children. */
		pbe->children = malloc(pbe->degree * sizeof(pbe));
		if(pbe->children == NULL) {
			// TODO failed to malloc enough space for node
		}
		
		/* Get length of each child node. */
		int* lens = (int*) malloc((pbe->degree+1) * sizeof(int));
		if(lens == NULL) {
			// TODO failed to malloc enough space for array
		}
		for(i = 0; i < n; i++)
			if(str[i] == '(') depth++;
			else if(str[i] == ')') depth--;
			else if(str[i] == ',' && depth == 0) j++;
			else lens[j]++;
		
		/* Parse the operator. */
		if(lens[0] == 1) {
			if(str[0] == '!') pbe->op = NOT;
			else if(str[0] == '&') pbe->op = AND;
			else if(str[0] == '|') pbe->op = OR;
			else if(str[0] == '=') pbe->op = EQ;
			else if(str[0] == '<') pbe->op = LT;
			else if(str[0] == '>') pbe->op = GT;
			else if(str[0] == '?') pbe->op = EXST;
			else {
				// TODO unsupported operation
			}
		}else if(lens[0] == 2) {
			if(str[0] == '!' && str[1] == '=') pbe->op = NEQ;
			else if(str[0] == '<' && str[1] == '=') pbe->op = LTE;
			else if(str[0] == '>' && str[1] == '=') pbe->op = GTE;
			else {
				// TODO unsupported operation
			}
		}else{
			// TODO unsupported operation
		}

		/* Enforce operator arity. */
		if((pbe->op == NOT || pbe->op == EXST) && pbe->degree != 1) {
			// TODO these are unary operators
		}else if((pbe->op == AND || pbe->op == OR || pbe->op == EQ) && pbe->degree < 2) {
			// TODO these are 2+ary operators
		}else if((pbe->op == NEQ || pbe->op == LT || pbe->op == GT || pbe->op == GTE || 
				pbe->op == LTE) && pbe->degree != 2) {
			// TODO these are binary operators
		}
		
		/* Parse each child. */
		j = 1;
		depth = 0;
		for(i = lens[0]; i < n; i++)
			if(str[i] == '(') depth++;
			else if(str[i] == ')') depth--;
			else if(str[i] == ',' && depth == 0) {
				if(!lens[j]) {
					// TODO no argument provided
				}
				int err = pbe_parse(pbe->children+no, str+i+1, lens[j++]);
				if(err) {
					// TODO error in child
				}
			}
		
		/* No error! */
		return NULL;
		
		
	/* No leaf node is surrounded by parentheses. */
	}else{
		if(str[0] == '[' && str[n-1] == ']') {
			
		}else if(pbe_isdate(str, n)) {
			
		}else if(pbe_isnumber(str, n)) {
			
		}else if(pbe_isstring(str, n)) {
			
		}else{
			// TODO not a valid literal
		}
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
