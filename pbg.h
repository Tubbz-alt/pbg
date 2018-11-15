/********************************************************************************
 * Prefix Boolean Grammar (PBG), a lightweight C library                        *
 * By Tyler J. Daddio, November 2018                                            *
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
	PBG_UNKNOWN,    // Unknown expression type.
	PBG_LT_TRUE,    // TRUE literal
	PBG_LT_FALSE,   // FALSE literal
	PBG_LT_NUMBER,  // NUMBER literal
	PBG_LT_STRING,  // STRING literal
	PBG_LT_DATE,    // DATE literal
	PBG_LT_KEY,     // KEY literal
	PBG_MAX_LT,     // END OF LITERALS!
	PBG_OP_NOT,   // !
	PBG_OP_AND,   // &
	PBG_OP_OR,    // |
	PBG_OP_EQ,    // =
	PBG_OP_LT,    // <
	PBG_OP_GT,    // >
	PBG_OP_EXST,  // ?
	PBG_OP_NEQ,   // !=
	PBG_OP_LTE,   // <=
	PBG_OP_GTE,   // >=
	PBG_MAX_OP    // END OF OPS!
} pbg_node_type;

/**
 * This struct represents a Prefix Boolean Expression. The value of the
 * operator constrains both the number of children and the type of children.
 */
typedef struct {
	pbg_node_type  _type;  /* Node type, determines the type/size of data. */
	int            _int;   /* Type determines what this is used for! */
	void*          _data;  /* Arbitrary data! */
} pbg_expr_node;

/**
 * This struct represents a container/manager for a Prefix Boolean Expression.
 * The benefit of using this struct instead of a pbg_expr_node directly is the 
 * opportunity for indirection: this struct can store "global" expressions to
 * help reduce the memory footprint of the entire tree. For example, this
 * struct tracks all pbg_expr_node instances representing a KEY to avoid duplicate
 * creation and reduced workload during KEY resolution.
 */
typedef struct {
	pbg_expr_node*  _static;     /* Static nodes (read: not KEYs). */
	pbg_expr_node*  _dynamic;    /* Dynamic nodes (read: KEYs). */
	int             _staticsz;   /* Number of static nodes. */
	int             _dynamicsz;  /* Number of dynamic nodes. */
} pbg_expr;

/**
 * This struct represents a PBG DATE literal.
 */
typedef struct {
	unsigned int  _YYYY;  /* year */
	unsigned int  _MM;    /* month */
	unsigned int  _DD;    /* day */
} pbg_type_date;

/**
 * Converts the string to a PBG expression operator. This does not parse
 * literals! Use the appropriate pbg_to* function for that.
 * @param op  Container to save parsed value to.
 * @param str String to parse.
 * @param n   Length of string to parse.
 * @param PBG_UNKNOWN if parsing fails, PBG_OP_* if success.
 */
pbg_node_type pbg_toop(char* str, int n);

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
 * Destroys the PBG expression instance and frees all associated resources.
 * @param e PBG expression to destroy.
 */
void pbg_free(pbg_expr* e);

/**
 * Evaluates the Prefix Boolean Expression with the provided assignments.
 * @param e    PBG expression to evaluate.
 * @param dict Dictionary used to resolve KEY names.
 * @return 1 if the PBG expression evaluates to true with the given dictionary. 
 *         0 otherwise.
 */
int pbg_evaluate(pbg_expr* e, pbg_expr* (*dict)(char*, int));

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
char* pbg_gets(pbg_expr* e, char** bufptr, int n);


void pbg_print(pbg_expr* e);

#endif  /* __PBG_H__ */
