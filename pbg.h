/********************************************************************************
 * Prefix Boolean Grammar (PBG), a lightweight C library                        *
 * By Tyler J. Daddio, November 2018                                            *
 ********************************************************************************/

#ifndef __PBG_H__
#define __PBG_H__

/* Used to suppress compiler warnings for intentionally unused arguments. 
 * This isn't as foolproof as GCC's unused attribute, but it is not compiler-
 * dependent, which is just dandy. */
#define PBG_UNUSED(x) (void)(x)


/*****************************
 *                           *
 * EXPRESSION REPRESENTATION *
 *                           *
 *****************************/

/**
 * These are the PBG operators supported by this implementation. They can be
 * cross-referenced with the grammar pseudocode above. Note: some values are
 * no-ops, i.e. they are symbolic values used to indicate leaves in the
 * expression tree.
 */
typedef enum {
	PBG_UNKNOWN,  // Unknown expression type.
	
	/* Type literals. */
	PBG_MIN_LT_TP,
	PBG_LT_TP_DATE,    // DATE TYPE literal
	PBG_LT_TP_BOOL,    // TRUE/FALSE TYPE literal
	PBG_LT_TP_NUMBER,  // NUMBER TYPE literal
	PBG_LT_TP_STRING,  // STRING TYPE literal
	// Add more TYPE literals here.
	PBG_MAX_LT_TP,
	
	/* Literals. */
	PBG_MIN_LT,
	PBG_LT_TRUE,    // TRUE literal
	PBG_LT_FALSE,   // FALSE literal
	PBG_LT_NUMBER,  // NUMBER literal
	PBG_LT_STRING,  // STRING literal
	PBG_LT_DATE,    // DATE literal
	PBG_LT_KEY,     // KEY literal
	// Add more literals here.
	PBG_MAX_LT,     // END OF LITERALS!
	
	/* Operators. */
	PBG_MIN_OP,
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
	PBG_OP_TYPE,  // @
	// Add more operators here.
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
 * This struct represents a Prefix Boolean Expression. The AST is represented
 * using a 'flat' representation: two arrays are used to compactly and locally
 * store each node.
 *
 * The static array stores non-KEY nodes. The dynamic array stores KEY nodes.
 */
typedef struct {
	pbg_expr_node*  _static;     /* Static nodes (read: not KEYs). */
	pbg_expr_node*  _dynamic;    /* Dynamic nodes (read: KEYs). */
	int             _staticsz;   /* Number of static nodes. */
	int             _dynamicsz;  /* Number of dynamic nodes. */
} pbg_expr;


/************************
 *                      *
 * ERROR REPRESENTATION *
 *                      *
 ************************/

/**
 * These are type specifiers for all possible PBG errors.
 */
typedef enum {
	PBG_ERR_NONE,
	PBG_ERR_ALLOC,
	PBG_ERR_STATE,
	PBG_ERR_SYNTAX,
	PBG_ERR_UNKNOWN_TYPE,
	PBG_ERR_OP_ARITY,
	PBG_ERR_OP_ARG_TYPE
} pbg_error_type;

/**
 * This struct represents a PBG error. Errors can be generated during parsing 
 * and evaluation and should be checked by the caller.
 */
typedef struct {
	pbg_error_type  _type;  /* Error type. */
	int             _line;  /* Line of file where error occurred. */
	char*           _file;  /* File containing error. */
	int             _int;   /* Type determines what this is used for! */
	void*           _data;  /* Data to be included with error report. */
} pbg_error;


/***************
 *             *
 * EXPRESSIONS *
 *             *
 ***************/

/**
 * Parses the string as a boolean expression in Prefix Boolean Grammar.
 * @param e   PBG expression instance to initialize.
 * @param err Container to store error, if any occurs.
 * @param str String to parse.
 * @param n   Length of the string.
 */
void pbg_parse(pbg_expr* e, pbg_error* err, char* str, int n);

/**
 * Destroys the PBG expression instance and frees all associated resources.
 * This function does not free the provided pointer.
 * @param e PBG expression to destroy.
 */
void pbg_free(pbg_expr* e);

/**
 * Evaluates the Prefix Boolean Expression with the provided assignments.
 * @param e    PBG expression to evaluate.
 * @param err  Container to store error, if any occurs.
 * @param dict Dictionary used to resolve KEY names.
 * @return 1 if the PBG expression evaluates to true with the given dictionary. 
 *         0 otherwise.
 */
int pbg_evaluate(pbg_expr* e, pbg_error* err, pbg_expr_node (*dict)(char*, int));

/** 
 * Identifies the PBG expression type of the given string. This function works
 * for both literal and operator types.
 * @param str String to parse.
 * @param n   Length of the string.
 * @return the pbg_node_type associated with the given string, PBG_UNKNOWN if
 *         none.
 */
pbg_node_type pbg_gettype(char* str, int n);


/***************
 *             *
 *   ERRORS    *
 *             *
 ***************/

/**
 * Prints a human-readable representation of the given pbg_error to the
 * standard output.
 * @param err  Error to print.
 */
void pbg_error_print(pbg_error* err);

/**
 * Frees resources being used by the given error, if any. This function does
 * not free the provided pointer.
 * @param e  PBG error to clean up.
 */
void pbg_error_free(pbg_error* e);


#endif  /* __PBG_H__ */
