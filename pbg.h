/********************************************************************************
 * Prefix Boolean Grammar (PBG), a lightweight C library                        *
 * By Tyler J. Daddio, November 2018                                            *
 ********************************************************************************/

#ifndef __PBG_H__
#define __PBG_H__

/* Used to suppress compiler warnings for intentionally unused arguments. This
 * isn't as foolproof as GCC's unused attribute, but it is not compiler-
 * dependent, which is just dandy. */
#define PBG_UNUSED(x) (void)(x)

/* Used to represent the result of an expression evaluation. */
#define PBG_FALSE  0
#define PBG_TRUE   1
#define PBG_ERROR -1

/*****************************
 *                           *
 * EXPRESSION REPRESENTATION *
 *                           *
 *****************************/

/**
 * Types of PBG fields.
 */
typedef enum {
	PBG_NULL,  /* NULL: Unknown field type */
	
	/* Type literals. */
	PBG_MIN_LT_TP,
	PBG_LT_TP_DATE,    /* DATE TYPE literal */
	PBG_LT_TP_BOOL,    /* TRUE/FALSE TYPE literal */
	PBG_LT_TP_NUMBER,  /* NUMBER TYPE literal */
	PBG_LT_TP_STRING,  /* STRING TYPE literal */
	/* Add more TYPE literals here. */
	PBG_MAX_LT_TP,
	
	/* Literals. */
	PBG_MIN_LT,
	PBG_LT_TRUE,    /* TRUE literal */
	PBG_LT_FALSE,   /* FALSE literal */
	PBG_LT_NUMBER,  /* NUMBER literal */
	PBG_LT_STRING,  /* STRING literal */
	PBG_LT_DATE,    /* DATE literal */
	PBG_LT_VAR,     /* VARiable literal */
	/* Add more literals here. */
	PBG_MAX_LT,
	
	/* Operators. */
	PBG_MIN_OP,
	PBG_OP_NOT,   /* !   NOT */
	PBG_OP_AND,   /* &   AND */
	PBG_OP_OR,    /* |   OR */
	PBG_OP_EQ,    /* =   EQUAL */
	PBG_OP_LT,    /* <   LESS THAN */
	PBG_OP_GT,    /* >   GREATER THAN */
	PBG_OP_EXST,  /* ?   EXISTS */
	PBG_OP_NEQ,   /* !=  NOT EQUAL */
	PBG_OP_LTE,   /* <=  LESS THAN OR EQUAL TO */
	PBG_OP_GTE,   /* >=  GREATER THAN OR EQUAL TO */
	PBG_OP_TYPE,  /* @   TYPE OF */
	/* Add more operators here. */
	PBG_MAX_OP
} pbg_field_type;

/**
 * This struct represents a PBG field. A field can be either a literal or an 
 * operator. This is determined by its type. For operators, the data pointer
 * describes a list of pointers to other fields in the abstract syntax tree.
 * For constants, it describes data relevant to the field type.
 */
typedef struct {
	pbg_field_type  _type;  /* Node type, determines the type/size of data. */
	int             _int;   /* Type determines what this is used for! */
	void*           _data;  /* Arbitrary data! */
} pbg_field;

/**
 * This struct represents a PBG expression. There are two arrays in this 
 * representation: one for constants, and one for variables. Both types are
 * represented by fields. 
 */
typedef struct {
	pbg_field*  _constants;  /* Constants. */
	pbg_field*  _variables;  /* Variables. */
	int         _numconst;   /* Number of constants. */
	int         _numvars;    /* Number of variables. */
} pbg_expr;


/************************
 *                      *
 * ERROR REPRESENTATION *
 *                      *
 ************************/

/**
 * Type specifiers for all possible PBG errors.
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
 * Represents a PBG error. Errors may be generated during parsing and 
 * evaluation and should be checked by the caller.
 */
typedef struct {
	pbg_error_type  _type;  /* Error type. */
	int             _line;  /* Line of file where error occurred. */
	char*           _file;  /* File in which error occurred. */
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
 * @param e    PBG expression instance to initialize.
 * @param err  Container to store error, if any occurs.
 * @param str  String to parse.
 * @param n    Length of the string.
 */
void pbg_parse(pbg_expr* e, pbg_error* err, char* str, int n);

/**
 * Destroys the PBG expression instance and frees all associated resources.
 * This function does not free the provided pointer.
 * @param e PBG expression to destroy.
 */
void pbg_free(pbg_expr* e);

/**
 * Evaluates the PBG expression with the provided assignments.
 * @param e     PBG expression to evaluate.
 * @param err   Container to store error, if any occurs.
 * @param dict  Dictionary used to resolve VAR names.
 * @return 1 if the PBG expression evaluates to true with the given dictionary. 
 *         0 otherwise.
 */
int pbg_evaluate(pbg_expr* e, pbg_error* err, pbg_field (*dict)(char*, int));


/**************
 *            *
 *   FIELDS   *
 *            *
 **************/

/**
 * Makes a field representing the given type. Initializes everything other than 
 * the type to zero.
 * @param type  Type of the field.
 * @return a new field with the given type.
 */
pbg_field pbg_make_field(pbg_field_type type);

/**
 * Makes a field representing a DATE. Attempts to parse the given string as a
 * DATE. If an error occurs during conversion, then err will be initialized with
 * the relevant error, if it is not NULL.
 * @param err  Used to store error, if any.
 * @param str  String to parse as a DATE.
 * @param n    Length of str.
 * @return a new field with the PBG_LT_DATE type if successful,
 *         a field with the PBG_NULL type otherwise.
 */
pbg_field pbg_make_date(pbg_error* err, char* str, int n);

/**
 * Makes a field representing a NUMBER. Attempts to parse the given string as a
 * NUMBER. If an error occurs during conversion, then err will be initialized 
 * with the relevant error, if it is not NULL.
 * @param err  Used to store error, if any.
 * @param str  String to parse as a NUMBER.
 * @param n    Length of str.
 * @return a new field with the PBG_LT_NUMBER type if successful,
 *         a field with the PBG_NULL type otherwise.
 */
pbg_field pbg_make_number(pbg_error* err, char* str, int n);

/**
 * Makes a field representing a STRING. Attempts to parse the given string as a 
 * STRING. If an error occurs during conversion, then err will be initialized 
 * with the relevant error, if it is not NULL.
 * @param err  Used to store error, if any.
 * @param str  String to parse as a STRING.
 * @param n    Length of str.
 * @return a new field with the PBG_LT_STRING type if successful,
 *         a field with the PBG_NULL type otherwise.
 */
pbg_field pbg_make_string(pbg_error* err, char* str, int n);


/***************
 *             *
 *   ERRORS    *
 *             *
 ***************/

/**
 * Checks if the given error has been initialized with error data.
 * @param err  Error to check.
 * @return 1 if the given error has been initialized, 0 otherwise.
 */
int pbg_iserror(pbg_error* err);

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
