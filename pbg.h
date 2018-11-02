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

#ifndef __PBG_H__
#define __PBG_H__

/**
 * These are the PBG operators supported by this implementation. They can be
 * cross-referenced with the grammar pseudocode above. Note: some values are
 * no-ops, i.e. they are symbolic values used to indicate leaves in the
 * expression tree.
 */
typedef enum {
	PBG_LT_TRUE,    // TRUE literal
	PBG_LT_FALSE,   // FALSE literal
	PBG_LT_NUMBER,  // NUMBER literal
	PBG_LT_STRING,  // STRING literal
	PBG_LT_DATE,    // DATE literal
	PBG_LT_KEY,     // KEY literal
	PBG_OP_NOT,   // !
	PBG_OP_AND,   // &
	PBG_OP_OR,    // |
	PBG_OP_EQ,    // =
	PBG_OP_NEQ,   // !=
	PBG_OP_LT,    // <
	PBG_OP_GT,    // >
	PBG_OP_LTE,   // <=
	PBG_OP_GTE,   // >=
	PBG_OP_EXST   // ?
} pbg_expr_type;

/**
 * This struct represents a Prefix Boolean Expression. The value of the
 * operator constrains both the number of children and the type of children.
 */
typedef struct {
	pbg_expr_type  _op;    /* Node type, determines the type/size of data. */
	void*          _data;  /* Arbitrary data! */
	int            _size;  /* Size of _data, in bytes. */
} pbg_expr;

/**
 * This struct represents a PBG DATE literal.
 */
typedef struct {
	unsigned int   _YYYY;  /* year */
	unsigned char  _MM;    /* month */
	unsigned char  _DD;    /* day */
} pbg_type_date;

/**
 * Checks if the given string encodes a valid PBG TRUE literal.
 * @param str String encoding of PBG TRUE.
 * @param n   Length of the string.
 * @return 1 if str encodes a valid PBG TRUE literal, 0 otherwise.
 */
int pbg_istrue(char* str, int n);

/**
 * Checks if the given string encodes a valid PBG FALSE literal.
 * @param str String encoding of PBG FALSE.
 * @param n   Length of the string.
 * @return 1 if str encodes a valid PBG FALSE literal, 0 otherwise.
 */
int pbg_isfalse(char* str, int n);

/**
 * Checks if the given string encodes a valid PBG NUMBER literal.
 * @param str String encoding of PBG NUMBER.
 * @param n   Length of the string.
 * @return 1 if str encodes a valid PBG NUMBER literal, 0 otherwise.
 */
int pbg_isnumber(char* str, int n);

/**
 * Checks if the given string encodes a valid PBG KEY identifier.
 * @param str String encoding of PBG KEY.
 * @param n   Length of the string.
 * @return 1 if str encodes a valid PBG KEY identifier, 0 otherwise.
 */
int pbg_iskey(char* str, int n);

/**
 * Checks if the given string encodes a valid PBG STRING literal.
 * @param str String encoding of PBG STRING.
 * @param n   Length of the string.
 * @return 1 if str encodes a valid PBG STRING literal, 0 otherwise.
 */
int pbg_isstring(char* str, int n);

/**
 * Checks if the given string encodes a valid PBG DATE literal.
 * @param str String encoding of PBG DATE.
 * @param n   Length of the string.
 * @return 1 if str encodes a valid PBG DATE literal, 0 otherwise.
 */
int pbg_isdate(char* str, int n);

/**
 * Converts the string to a PBG DATE literal.
 * @param ptr PBG DATA struct to store conversion.
 * @param str String to parse.
 * @param n   Length of string to parse.
 */
void pbg_todate(pbg_type_date* ptr, char* str, int n);

/**
 * Parses the string as a boolean expression in Prefix Boolean Grammar.
 * @param e   PBG expression instance to initialize.
 * @param str String to parse.
 * @param n   Length of the string.
 * @return 0 if successful, an error code if unsuccessful.
 */
int pbg_parse(pbg_expr* e, char* str, int n);

/**
 * Evaluates the Prefix Boolean Expression with the provided assignments.
 * @param e PBG expression to evaluate.
 */
void pbg_evaluate(pbg_expr* e);

/**
 * Prints the Prefix Boolean Expression to the char pointer provided. If ptr is
 * NULL and n is 0, then this function will allocate memory on the heap which
 * must then be free'd by the caller. In this way, this function behaves in a
 * manner reminiscent of fgets.
 * @param e      PBG expression to print.
 * @param bufptr Pointer to the output buffer.
 * @param n      Length of the buffer pointed to by bufptr.
 * @return Number of characters written to the buffer.
 */
void pbg_gets(pbg_expr* e, char** bufptr, int n);

#endif  /* __PBG_H__ */