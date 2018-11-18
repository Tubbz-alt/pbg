#include "pbg.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/****************************
 *                          *
 * LOCAL FUNCTION DIRECTORY *
 *                          *
 ****************************/

/* NODE MANAGEMENT */
pbg_expr_node* pbg_get_node(pbg_expr* e, int index);
void pbg_free_node(pbg_expr_node* node);

/* ERROR CONSTRUCTION */
void pbg_err_alloc(pbg_error* err, int line, char* file);
void pbg_err_unknown_type(pbg_error* err, int line, char* file);
void pbg_err_syntax(pbg_error* err, int line, char* file, char* msg);
void pbg_err_op_arity(pbg_error* err, int line, char* file, pbg_node_type type, int arity);
void pbg_err_state(pbg_error* err, int line, char* file, char* msg);
void pbg_err_op_arg_type(pbg_error* err, int line, char* file);

/* NODE CREATION TOOLKIT */
int pbg_create_op(pbg_expr* e, pbg_error* err, pbg_node_type type, int numchildren);
int pbg_create_lt_key(pbg_expr* e, pbg_error* err, char* str, int n);
int pbg_create_lt_date(pbg_expr* e, pbg_error* err, char* str, int n);
int pbg_create_lt_number(pbg_expr* e, pbg_error* err, char* str, int n);
int pbg_create_lt_string(pbg_expr* e, pbg_error* err, char* str, int n);
int pbg_create_lt_true(pbg_expr* e, pbg_error* err, char* str, int n);
int pbg_create_lt_false(pbg_expr* e, pbg_error* err, char* str, int n);

/* NODE PARSING TOOLKIT */
int pbg_check_op_arity(pbg_node_type type, int numargs);
int pbg_parse_r(pbg_expr* e, pbg_error* err, char* str, int** fields, int** lengths, int** closings);

/* NODE EVALUATION TOOLKIT */
int pbg_evaluate_r(pbg_expr* e, pbg_error* err, pbg_expr_node* node);

/* JANITORIAL FUNCTIONS */
// No local functions.

/* PRINTING & VISUALIZING TOOLKIT */
int  pbg_gets_r(pbg_expr* e, pbg_expr_node* node, char* buf, int i);
void pbg_print_h(pbg_expr* e, pbg_expr_node* node, int depth);

/* CONVERSION & CHECKING TOOLKIT */
// No local functions.

/* HELPER FUNCTIONS */
int is_a_digit(char c);
int is_whitespace(char c);


/*******************
 *                 *
 * NODE MANAGEMENT *
 *                 *
 *******************/

/**
 * Static nodes are indexed by 1,2,3,... Dynamic nodes are indexed by 
 * -1,-2,-3,..., where these correspond to the first, second, third, etc nodes 
 * in the list. As such, this function converts the index to an array index in 
 * either list depending on its sign and value.
 * @param e      PBG expression to search.
 * @param index  Index of the node to return.
 * @return Pointer to the pbg_expr_node* in e specified by the index.
 */
pbg_expr_node* pbg_get_node(pbg_expr* e, int index)
{
	if(index < 0)
		return e->_dynamic - (index+1);
	else
		return e->_static + (index-1);
}

/**
 * Free's the single pbg_expr_node pointed to by the specified pointer.
 * @param node  pbg_expr_node to free.
 */
void pbg_free_node(pbg_expr_node* node)
{
	if(node->_data != NULL)
		free(node->_data);
}


/**********************
 *                    *
 * ERROR CONSTRUCTION *
 *                    *
 **********************/

void pbg_error_print(pbg_error* err)
{
	if(err->_type == PBG_ERR_NONE)
		return;
	printf("error %s at %s:%d", pbg_error_str(err->_type), err->_file, err->_line);
	switch(err->_type) {
		case PBG_ERR_SYNTAX:
		case PBG_ERR_STATE:
			printf(": %s", (char*) err->_data);
			break;
	}
	printf("\n");
}

void pbg_err_alloc(pbg_error* err, int line, char* file)
{
	err->_type = PBG_ERR_ALLOC;
	err->_line = line;
	err->_file = file;
	err->_int = 0;
	err->_data = NULL;
}

void pbg_err_unknown_type(pbg_error* err, int line, char* file)
{
	err->_type = PBG_ERR_UNKNOWN_TYPE;
	err->_line = line;
	err->_file = file;
	err->_int = 0;
	err->_data = NULL;
}

void pbg_err_syntax(pbg_error* err, int line, char* file, char* msg)
{
	err->_type = PBG_ERR_SYNTAX;
	err->_line = line;
	err->_file = file;
	err->_int = 0;
	err->_data = msg;
}

void pbg_err_op_arity(pbg_error* err, int line, char* file, pbg_node_type type, int arity)
{
	err->_type = PBG_ERR_OP_ARITY;
	err->_line = line;
	err->_file = file;
	err->_int = sizeof(pbg_node_type);
	err->_data = malloc(err->_int);
	if(err->_data == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__); /* unfortunate. */
		return;
	}
	*((pbg_node_type*)err->_data) = type;
}

void pbg_err_state(pbg_error* err, int line, char* file, char* msg)
{
	err->_type = PBG_ERR_STATE;
	err->_line = line;
	err->_file = file;
	err->_int = 0;
	err->_data = msg;
}

void pbg_err_op_arg_type(pbg_error* err, int line, char* file)
{
	err->_type = PBG_ERR_OP_ARG_TYPE;
	err->_line = line;
	err->_file = file;
	err->_int = 0;
	err->_data = NULL;
}

void pbg_error_free(pbg_error* err)
{
	if(err->_int != 0)
		free(err->_data);
}


/*************************
 *                       *
 * NODE CREATION TOOLKIT *
 *                       *
 *************************/

int pbg_create_op(pbg_expr* e, pbg_error* err, pbg_node_type type, int numchildren)
{
	/* Static nodes have positive indices. */
	int nodei = 1 + e->_staticsz++;
	pbg_expr_node* node = pbg_get_node(e, nodei);
	node->_type = type;
	node->_int = 0;
	node->_data = malloc(numchildren * sizeof(int));
	if(node->_data == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__);
		return 0;
	}
	/* Done! */
	return nodei;
}

int pbg_create_lt_key(pbg_expr* e, pbg_error* err, char* str, int n)
{
	/* Dynamic nodes have negative indices. 
	 * Subtract 1 to offset first element to -1 from 0. */
	int nodei = -(1 + e->_dynamicsz++);
	/* Initialize node! */
	pbg_expr_node* node = pbg_get_node(e, nodei);
	node->_type = PBG_LT_KEY;
	node->_int = (n-2) * sizeof(char);
	node->_data = malloc(node->_int);
	if(node->_data == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__);
		return 0;
	}
	memcpy(node->_data, str+1, n-2);
	// TODO ensure each key node in e->_dynamic is unique?
	/* Done! */
	return nodei;
}

int pbg_create_lt_date(pbg_expr* e, pbg_error* err, char* str, int n)
{
	/* Static nodes have positive indices. */
	int nodei = 1 + e->_staticsz++;
	/* Initialize node! */
	pbg_expr_node* node = pbg_get_node(e, nodei);
	node->_type = PBG_LT_DATE;
	node->_int = sizeof(pbg_date_lt);
	node->_data = malloc(node->_int);
	if(node->_data == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__);
		return 0;
	}
	pbg_todate((pbg_date_lt*)node->_data, str, n);
	/* Done! */
	return nodei;
}

int pbg_create_lt_number(pbg_expr* e, pbg_error* err, char* str, int n)
{
	/* Static nodes have positive indices. */
	int nodei = 1 + e->_staticsz++;
	/* Initialize node! */
	pbg_expr_node* node = pbg_get_node(e, nodei);
	node->_type = PBG_LT_NUMBER;
	node->_int = sizeof(pbg_number_lt);
	node->_data = malloc(node->_int);
	if(node->_data == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__);
		return 0;
	}
	pbg_tonumber((pbg_number_lt*)node->_data, str, n);
	/* Done! */
	return nodei;
}

int pbg_create_lt_string(pbg_expr* e, pbg_error* err, char* str, int n)
{
	/* Static nodes have positive indices. */
	int nodei = 1 + e->_staticsz++;
	/* Initialize node! */
	pbg_expr_node* node = pbg_get_node(e, nodei);
	node->_type = PBG_LT_STRING;
	node->_int = (n-2) * sizeof(char);
	node->_data = malloc(node->_int);
	if(node->_data == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__);
		return 0;
	}
	memcpy(node->_data, str+1, n-2);
	/* Done! */
	return nodei;
}

int pbg_create_lt_true(pbg_expr* e, pbg_error* err, char* str, int n)
{
	/* Static nodes have positive indices. */
	int nodei = 1 + e->_staticsz++;
	/* Initialize node! */
	pbg_expr_node* node = pbg_get_node(e, nodei);
	node->_type = PBG_LT_TRUE;
	node->_int = 0;
	node->_data = NULL;
	/* Done! */
	return nodei;
}

int pbg_create_lt_false(pbg_expr* e, pbg_error* err, char* str, int n)
{
	/* Static nodes have positive indices. */
	int nodei = 1 + e->_staticsz++;
	/* Initialize node! */
	pbg_expr_node* node = pbg_get_node(e, nodei);
	node->_type = PBG_LT_FALSE;
	node->_int = 0;
	node->_data = NULL;
	/* Done! */
	return nodei;
}


/************************
 *                      *
 * NODE PARSING TOOLKIT *
 *                      *
 ************************/

/**
 * Checks if the operator can legally take the specified number of arguments.
 * This function encodes the rules for operator arity and should be modified if
 * a new operator is added.
 * @param type     Type of operator.
 * @param numargs  Number of arguments to operator.
 * @return 1 if the number of arguments can be legally given to the operator, 
 *         0 if not or if type does not refer to an operator.
 */
int pbg_check_op_arity(pbg_node_type type, int numargs)
{
	int arity = 0;
	/* Positive arity specifies "exact arity," i.e. the number of arguments 
	 * must be exact. Negative arity specifies a "minimum arity," i.e. the 
	 * minimum number of arguments needed. */
	switch(type) {
		case PBG_OP_NOT:  arity =  1; break;
		case PBG_OP_AND:  arity = -2; break;
		case PBG_OP_OR:   arity = -2; break;
		case PBG_OP_EQ:   arity = -2; break;
		case PBG_OP_LT:   arity =  2; break;
		case PBG_OP_GT:   arity =  2; break;
		case PBG_OP_EXST: arity =  1; break;
		case PBG_OP_NEQ:  arity =  2; break;
		case PBG_OP_LTE:  arity =  2; break;
		case PBG_OP_GTE:  arity =  2; break;
		default:
			return 0;
	}
	if((arity > 0 && numargs != arity) || 
			(-arity > 0 && numargs < -arity))
		return 0;
	return 1;
}

/**
 * TODO
 */
int pbg_parse_r(pbg_expr* e, pbg_error* err, char* str, 
		int** fields, int** lengths, int** closings)
{
	int nodei;  /* Index of this node. This is the return value. */
	
	/* Cache length of field for easier referencing. */
	int n = **lengths;
	int fieldi = **fields;
	
	/* Update pointers for next node. */
	(*fields)++, (*lengths)++;
	
	/* Identify type of field. If the type cannot be resolve, throw an error. */
	pbg_node_type type = pbg_gettype(str + fieldi, n);
	if(type == PBG_UNKNOWN) {
		pbg_err_unknown_type(err, __LINE__, __FILE__);
		return 0;
	}
	
	/* This field is an operator. */
	if(type > PBG_MAX_LT && type < PBG_MAX_OP) {
		pbg_expr_node* node;
		
		/* Maximum number of children this node has allocated space for. */
		int maxc = 2;
		/* Initialize node and record node index. */
		nodei = pbg_create_op(e, err, type, maxc);
		node = pbg_get_node(e, nodei);
		/* Propagate error back to caller, if any. */
		if(nodei == 0) return 0;
		
		/* Recursively build subtree rooted at this operator node. pbg_evaluate 
		 * set last element in fields to -1. This is used to ensure we don't 
		 * run past the end of the expression string. */
		while(**fields != -1 && **fields < **closings) {
			/* Make recursive call to construct subtree rooted at child. */
			int childi = pbg_parse_r(e, err, str, fields, lengths, closings);
			/* Propagate error back to caller, if any. */
			if(childi == 0) return 0;
			/* Expand array of children if necessary. */
			if(node->_int == maxc) {
				maxc *= 2;  // doubling gives amortized O(1) time insertion
				int* children = (int*) realloc(node->_data, maxc * sizeof(int));
				if(children == NULL) {
					pbg_err_alloc(err, __LINE__, __FILE__);
					return 0;
				}
				node->_data = (void*) children;
			}
			/* Store index of child node. */
			((int*)node->_data)[node->_int++] = childi;
		}
		
		/* Enforce operator arity. */
		if(pbg_check_op_arity(type, node->_int) == 0) {
			pbg_err_op_arity(err, __LINE__, __FILE__, type, node->_int);
			return 0;
		}
		
		/* Tighten list of children and save it. */
		int* children = (int*) realloc(node->_data, node->_int * sizeof(int));
		if(children == NULL) {
			pbg_err_alloc(err, __LINE__, __FILE__);
			return 0;
		}
		node->_data = (void*) children;
		
		/* This node read all of its children until the next closing. 
		 * The parent node will need to read until the end of the next 
		 * next one. */
		(*closings)++;
		
	/* This field is a literal. */
	}else{
		/* Move str to correct starting position. */
		str += fieldi;
		/* Identify type of literal and initialize the node! */
		if(type == PBG_LT_KEY)         nodei = pbg_create_lt_key(e, err, str, n);
		else if(type == PBG_LT_DATE)   nodei = pbg_create_lt_date(e, err, str, n);
		else if(type == PBG_LT_NUMBER) nodei = pbg_create_lt_number(e, err, str, n);
		else if(type == PBG_LT_STRING) nodei = pbg_create_lt_string(e, err, str, n);
		else if(type == PBG_LT_TRUE)   nodei = pbg_create_lt_true(e, err, str, n);
		else if(type == PBG_LT_FALSE)  nodei = pbg_create_lt_false(e, err, str, n);
		else{
			pbg_err_unknown_type(err, __LINE__, __FILE__);
			return 0;
		}
	}
	
	/* Done! */
	return nodei;
}

void pbg_parse(pbg_expr* e, pbg_error* err, char* str, int n)
{
	// TODO verify str is an element of PBG (syntactically, not semantically)
	
	/* Set to NULL to allow for pbg_free to check if needing free. */
	e->_static = NULL;
	e->_dynamic = NULL;
	
	/* These are initialized to 0 as they are used as counters for the number 
	 * of each type of node created. In the end they should be equal to the 
	 * associated local variables here. */
	e->_staticsz = 0;
	e->_dynamicsz = 0;
	
	/* Count number of non-STRING commas, keys, and closing braces. */
	int numcommas = 0;
	int numkeys = 0;
	int numclosings = 0;
	int instring = 0;
	int depth = 0, reachedend = 0;
	for(int i = 0; i < n; i++) {
		/* Check if we're in a string or not. */
		if((!instring && str[i] == '\'') || 
				(instring && str[i] == '\'' && str[i-1] != '\\'))
			instring = 1 - instring;
		/* Nothing gets counted if we are in a string! */
		if(!instring) {
			if(str[i] == '(') depth++;
			else if(str[i] == ',') numcommas++;
			else if(str[i] == '[') numkeys++;
			else if(str[i] == ')') {
				numclosings++, depth--;
				/* Negative depth only occurs if unmatched closing parentheses. */
				if(depth < 0) {
					pbg_err_syntax(err, __LINE__, __FILE__, "Too many closing parentheses.");
					return;
				}
				/* Depth zero only legally occurs at the end. */
				if(depth == 0 && !reachedend)
					reachedend = 1;
				else if(depth == 0 && reachedend) {
					pbg_err_syntax(err, __LINE__, __FILE__, "Multiple statements.");
					return;
				}
			}
		}
	}
	
	/* Check if string is left unbounded. */
	if(instring) {
		pbg_err_syntax(err, __LINE__, __FILE__, "Unclosed string.");
		return;
	}
	/* Check if some opening parentheses are left unclosed. */
	if(depth != 0) {
		pbg_err_syntax(err, __LINE__, __FILE__, "Unmatched opening parentheses.");
		return;
	}
	/* Check if we have weird stuff. */
	if(numclosings == 0 && !pbg_istrue(str, n) && !pbg_isfalse(str, n)) {
		pbg_err_syntax(err, __LINE__, __FILE__, "Invalid syntax.");
		return;
	}
	
	/* Compute sizes of static and dynamic arrays. */
	/* The relevant fields in e are used as indexing each array during parsing. */
	int numfields = numcommas+1;
	int numstatic = numfields - numkeys;
	int numdynamic = numkeys;
	
	/* Allocate space for needed arrays. */
	int* fields = (int*) malloc((numfields+1) * sizeof(int));
	if(fields == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__);
		return;
	}
	int* lengths = (int*) malloc(numfields * sizeof(int));
	if(lengths == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__);
		return;
	}
	int* closings = (int*) malloc(numclosings * sizeof(int));
	if(closings == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__);
		return;
	}
	
	/* Remove leading whitespace. */
	while(is_whitespace(*str)) str++;
	/* Compute position of first field. */
	if(str[0] == '(') {
		int j = 1;
		while(is_whitespace(str[j])) j++;
		fields[0] = j;
	}else
		fields[0] = 0;
	/* Compute position and lengths of each field & position of closings. */
	for(int i=fields[0], c=0, f=0, open=1, instring=0; i < n; i++) {
		/* Check if we're in a string or not. */
		if((!instring && str[i] == '\'') || 
				(instring && str[i] == '\'' && str[i-1] != '\\'))
			instring = 1 - instring;
		/* Nothing gets opened, closed, or measured if we're in a string! */
		if(!instring) {
			if(str[i] == ')') {
				int j = i-1;
				while(is_whitespace(str[j])) j--;
				closings[c++] = j+1;
			}
			if(open && (str[i] == ')' || (str[i] == ',' && str[i-1] != ')'))) {
				int j = i-1;
				while(is_whitespace(str[j])) j--;
				lengths[f] = j+1 - fields[f++];
				open = 0;
			}
			if(!open && (str[i] == '(' || (str[i] == ',' && str[i+1] != '('))) {
				int j = i+1;
				while(is_whitespace(str[j])) j++;
				fields[f] = j;
				open = 1;
			}
		}
	}
	/* Needed to determine when we've reached the end of the expression string
	 * in pbg_parse_r. */
	fields[numfields] = -1;
	
	/* Allocate space for static and dynamic node arrays. */
	e->_static = (pbg_expr_node*) malloc(numstatic * sizeof(pbg_expr_node));
	if(e->_static == NULL) {
		free(fields), free(lengths), free(closings);
		pbg_err_alloc(err, __LINE__, __FILE__);
		return;
	}
	e->_dynamic = (pbg_expr_node*) malloc(numdynamic * sizeof(pbg_expr_node));
	if(e->_dynamic == NULL) {
		free(e->_static);
		free(fields), free(lengths), free(closings);
		pbg_err_alloc(err, __LINE__, __FILE__);
		return;
	}
	
	/* Recursively parse the expression string to build the expression tree. */
	int* lengths_cpy = lengths, *closings_cpy = closings, *fields_cpy = fields;
	int status = pbg_parse_r(e, err, str, &fields_cpy, &lengths_cpy, &closings_cpy);
	
	/* If an error occurred, clean up. */
	if(status == 0) pbg_free(e);
	
	/* Clean up! */
	free(fields), free(lengths), free(closings);
}


/***************************
 *                         *
 * NODE EVALUATION TOOLKIT *
 *                         *
 ***************************/

int pbg_evaluate_op_not(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	int child0 = ((int*)node->_data)[0];
	int result = pbg_evaluate_r(e, err, pbg_get_node(e, child0));
	if(result == -1) return -1;  /* Pass error through. */
	return result == 0;
}

int pbg_evaluate_op_and(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	int size = node->_int;
	for(int i = 0; i < size; i++) {
		int childi = ((int*)node->_data)[i];
		int result = pbg_evaluate_r(e, err, pbg_get_node(e, childi));
		if(result == -1) return -1;  /* Pass error through. */
		if(result == 0)
			return 0;
	}
	return 1;
}

int pbg_evaluate_op_or(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	int size = node->_int;
	for(int i = 0; i < size; i++) {
		int childi = ((int*)node->_data)[i];
		int result = pbg_evaluate_r(e, err, pbg_get_node(e, childi));
		if(result == -1) return -1;  /* Pass error through. */
		if(result == 1)
			return 1;
	}
	return 0;
}

int pbg_evaluate_op_exst(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	int child0 = ((int*)node->_data)[0];
	pbg_expr_node* childn0 = pbg_get_node(e, child0);
	return childn0->_type != PBG_UNKNOWN;
}

int pbg_evaluate_op_eq(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	/* Ensure type and size of all children are identical. */
	int size = node->_int;
	int child0 = ((int*)node->_data)[0];
	pbg_expr_node* childn0 = pbg_get_node(e, child0);
	for(int i = 1; i < size; i++) {
		int childi = ((int*)node->_data)[i];
		pbg_expr_node* childni = pbg_get_node(e, childi);
		if(childni->_int != childn0->_int || 
				childni->_type != childn0->_type)
			return 0;
		/* Ensure each data byte is identical. */
		if(memcmp(childni->_data, childn0->_data, childn0->_int) != 0)
			return 0;
	}
	return 1;
}

int pbg_evaluate_op_neq(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	int child0 = ((int*)node->_data)[0];
	pbg_expr_node* childn0 = pbg_get_node(e, child0);
	int child1 = ((int*)node->_data)[1];
	pbg_expr_node* childn1 = pbg_get_node(e, child1);
	return childn1->_type != childn0->_type || 
			childn1->_int != childn0->_int || 
			memcmp(childn1->_data, childn0->_data, childn0->_int);
}

int pbg_evaluate_op_lt(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	int child0 = ((int*)node->_data)[0];
	pbg_expr_node* childn0 = pbg_get_node(e, child0);
	int child1 = ((int*)node->_data)[1];
	pbg_expr_node* childn1 = pbg_get_node(e, child1);
	/* Ensure both children are NUMBERs. */
	if(childn0->_type != PBG_LT_NUMBER || 
			childn1->_type != PBG_LT_NUMBER) {
		pbg_err_op_arg_type(err, __LINE__, __FILE__);
		return -1;
	}
	return ((pbg_number_lt*)childn0->_data)->_val <
			((pbg_number_lt*)childn1->_data)->_val;
}

int pbg_evaluate_op_gt(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	int child0 = ((int*)node->_data)[0];
	pbg_expr_node* childn0 = pbg_get_node(e, child0);
	int child1 = ((int*)node->_data)[1];
	pbg_expr_node* childn1 = pbg_get_node(e, child1);
	/* Ensure both children are NUMBERs. */
	if(childn0->_type != PBG_LT_NUMBER || 
			childn1->_type != PBG_LT_NUMBER) {
		pbg_err_op_arg_type(err, __LINE__, __FILE__);
		return -1;
	}
	return ((pbg_number_lt*)childn0->_data)->_val > 
			((pbg_number_lt*)childn1->_data)->_val;
}

int pbg_evaluate_op_lte(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	int child0 = ((int*)node->_data)[0];
	pbg_expr_node* childn0 = pbg_get_node(e, child0);
	int child1 = ((int*)node->_data)[1];
	pbg_expr_node* childn1 = pbg_get_node(e, child1);
	/* Ensure both children are NUMBERs. */
	if(childn0->_type != PBG_LT_NUMBER || 
			childn1->_type != PBG_LT_NUMBER) {
		pbg_err_op_arg_type(err, __LINE__, __FILE__);
		return -1;
	}
	return ((pbg_number_lt*)childn0->_data)->_val <= 
			((pbg_number_lt*)childn1->_data)->_val;
}

int pbg_evaluate_op_gte(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	int child0 = ((int*)node->_data)[0];
	pbg_expr_node* childn0 = pbg_get_node(e, child0);
	int child1 = ((int*)node->_data)[1];
	pbg_expr_node* childn1 = pbg_get_node(e, child1);
	/* Ensure both children are NUMBERs. */
	if(childn0->_type != PBG_LT_NUMBER || 
			childn1->_type != PBG_LT_NUMBER) {
		pbg_err_op_arg_type(err, __LINE__, __FILE__);
		return -1;
	}
	return ((pbg_number_lt*)childn0->_data)->_val >= 
			((pbg_number_lt*)childn1->_data)->_val;
}

/**
 * TODO
 */
int pbg_evaluate_r(pbg_expr* e, pbg_error* err, pbg_expr_node* node)
{
	/* This is a literal node! */
	if(node->_type < PBG_MAX_LT) {
		if(node->_type == PBG_LT_TRUE)  return 1;
		if(node->_type == PBG_LT_FALSE) return 0;
		
		/* TRUE and FALSE are the only literals with truth values. 
		 * If we get here, there's a bug, and we need to throw an error. */
		pbg_err_state(err, __LINE__, __FILE__, 
				"Cannot evaluate a literal without a truth value.");
		return -1;
		
	/* This is an operator node! */
	}else {
		switch(node->_type) {
			case PBG_OP_NOT:  return pbg_evaluate_op_not(e, err, node);
			case PBG_OP_AND:  return pbg_evaluate_op_and(e, err, node);
			case PBG_OP_OR:   return pbg_evaluate_op_or(e, err, node);
			case PBG_OP_EXST: return pbg_evaluate_op_exst(e, err, node);
			case PBG_OP_EQ:   return pbg_evaluate_op_eq(e, err, node);
			case PBG_OP_NEQ:  return pbg_evaluate_op_neq(e, err, node);
			case PBG_OP_LT:   return pbg_evaluate_op_lt(e, err, node);
			case PBG_OP_GT:   return pbg_evaluate_op_gt(e, err, node);
			case PBG_OP_LTE:  return pbg_evaluate_op_lte(e, err, node);
			case PBG_OP_GTE:  return pbg_evaluate_op_gte(e, err, node);
			default:
				pbg_err_unknown_type(err, __LINE__, __FILE__);
				return -1;
		}
	}
	
	
}

int pbg_evaluate(pbg_expr* e, pbg_error* err, pbg_expr_node (*dict)(char*, int))
{
	/* KEY resolution. Lookup every key in provided dictionary. */
	pbg_expr_node* dynamics;
	dynamics = (pbg_expr_node*) malloc(e->_dynamicsz * sizeof(pbg_expr_node));
	if(dynamics == NULL) {
		pbg_err_alloc(err, __LINE__, __FILE__);
		return -1;
	}
	for(int i = 0; i < e->_dynamicsz; i++) {
		pbg_expr_node* keylt = e->_dynamic+i;
		dynamics[i] = dict((char*)(keylt->_data), keylt->_int);
	}
	
	/* Swap out KEY literals with dictionary equivalents. */
	pbg_expr_node* old = e->_dynamic;
	e->_dynamic = dynamics;
	
	/* Evaluate expression! */
	int result = pbg_evaluate_r(e, err, e->_static);
	
	/* Restore old KEY literal array. */
	e->_dynamic = old;
	
	/* Clean up malloc'd memory. */
	for(int i = 0; i < e->_dynamicsz; i++)
		pbg_free_node(dynamics+i);
	free(dynamics);
	
	/* Done! */
	return result;
}


/************************
 *                      *
 * JANITORIAL FUNCTIONS *
 *                      *
 ************************/

void pbg_free(pbg_expr* e)
{
	/* Free individual static nodes. Some do not have _data malloc'd. */
	for(int i = e->_staticsz-1; i >= 0; i--)
		pbg_free_node(e->_static+i);
	
	/* Free individual dynamic nodes. All have _data malloc'd. */
	for(int i = 0; i < e->_dynamicsz; i++)
		pbg_free_node(e->_dynamic+i);
	
	/* Free internal node arrays. */
	if(e->_static != NULL)
		free(e->_static);
	if(e->_static != NULL)
		free(e->_dynamic);
}


/**********************************
 *                                *
 * PRINTING & VISUALIZING TOOLKIT *
 *                                *
 **********************************/

/**
 * TODO
 */
int pbg_gets_r(pbg_expr* e, pbg_expr_node* node, char* buf, int i)
{
	/* This node is a literal. */
	if(node->_type < PBG_MAX_LT)
	{
		char* data;
		pbg_date_lt* dt;
		switch(node->_type) {
			case PBG_LT_TRUE:
				buf[i++] = 'T';
				buf[i++] = 'R';
				buf[i++] = 'U';
				buf[i++] = 'E';
				break;
			case PBG_LT_FALSE:
				buf[i++] = 'F';
				buf[i++] = 'A';
				buf[i++] = 'L';
				buf[i++] = 'S';
				buf[i++] = 'E';
				break;
			case PBG_LT_STRING:
				data = (char*) node->_data;
				buf[i++] = '\'';
				for(int j = 0; j < node->_int; j++)
					buf[i++] = data[j];
				buf[i++] = '\'';
				break;
			case PBG_LT_DATE:
				dt = (pbg_date_lt*) node->_data;
				i += snprintf(buf+i, 5, "%04d", dt->_YYYY);
				buf[i++] = '-';
				i += snprintf(buf+i, 3, "%02d", dt->_MM);
				buf[i++] = '-';
				i += snprintf(buf+i, 3, "%02d", dt->_DD);
				break;
			case PBG_LT_KEY:
				data = (char*) node->_data;
				buf[i++] = '[';
				for(int j = 0; j < node->_int; j++)
					buf[i++] = data[j];
				buf[i++] = ']';
				break;
			case PBG_LT_NUMBER:
				i += sprintf(buf+i, "%.2lf", *((double*)node->_data));
				break;
			default:
				// TODO error: unknown literal!
				break;
		}
	
	/* This node is an operator. */
	}else{
		buf[i++] = '(';
		switch(node->_type) {
			case PBG_OP_NOT:  buf[i++] = '!'; break;
			case PBG_OP_AND:  buf[i++] = '&'; break;
			case PBG_OP_OR:   buf[i++] = '|'; break;
			case PBG_OP_EQ:   buf[i++] = '='; break;
			case PBG_OP_LT:   buf[i++] = '<'; break;
			case PBG_OP_GT:   buf[i++] = '>'; break;
			case PBG_OP_EXST: buf[i++] = '?'; break;
			case PBG_OP_NEQ:  buf[i++] = '!', buf[i++] = '='; break;
			case PBG_OP_LTE:  buf[i++] = '<', buf[i++] = '='; break;
			case PBG_OP_GTE:  buf[i++] = '>', buf[i++] = '='; break;
			default:
				// TODO error: unknown operator!
				break;
		}
		buf[i++] = ',';
		int* children = (int*)node->_data;
		for(int j = 0; j < node->_int; j++) {
			pbg_expr_node* child = pbg_get_node(e, children[j]);
			i = pbg_gets_r(e, child, buf, i);
			if(j != node->_int-1)
				buf[i++] = ',';
		}
		buf[i++] = ')';
	}
	return i;
}

char* pbg_gets(pbg_expr* e, char** bufptr, int n)
{
	char* buf = (char*) malloc(1000);  // TODO fix me then mix me
	int len = pbg_gets_r(e, e->_static, buf, 0);
	buf[len] = '\0';
	return buf;
}

/**
 * TODO
 */
void pbg_print_h(pbg_expr* e, pbg_expr_node* node, int depth)
{
	for(int i = 0; i < depth; i++)
		printf("  ");
	if(node->_type < PBG_MAX_LT) {
		pbg_date_lt* date;
		switch(node->_type) {
			case PBG_LT_TRUE:
				printf("TRUE\n");
				break;
			case PBG_LT_FALSE:
				printf("FALSE\n");
				break;
			case PBG_LT_NUMBER:
				printf("NUMBER : %lf\n", *((double*)node->_data));
				break;
			case PBG_LT_STRING:
				printf("STRING : '");
				for(int i = 0; i < node->_int; i++)
					printf("%c", ((char*)node->_data)[i]);
				printf("'\n");
				break;
			case PBG_LT_DATE:
				date = (pbg_date_lt*)node->_data;
				printf("DATE : %4d-%2d-%2d\n", date->_YYYY, date->_MM, date->_DD);
				break;
			case PBG_LT_KEY:
				printf("KEY : [");
				for(int i = 0; i < node->_int; i++)
					printf("%c", ((char*)node->_data)[i]);
				printf("]\n");
				break;
			default:
				printf("UNKNWON LT\n");
				break;
		}
	}else if(node->_type < PBG_MAX_OP) {
		switch(node->_type) {
			case PBG_OP_NOT: printf("NOT !\n"); break;
			case PBG_OP_AND: printf("AND &\n"); break;
			case PBG_OP_OR: printf("OR |\n"); break;
			case PBG_OP_EQ: printf("EQ =\n"); break;
			case PBG_OP_LT: printf("LT <\n"); break;
			case PBG_OP_GT: printf("GT >\n"); break;
			case PBG_OP_EXST: printf("EXIST ?\n"); break;
			case PBG_OP_NEQ: printf("NEQ !=\n"); break;
			case PBG_OP_LTE: printf("LTE <=\n"); break;
			case PBG_OP_GTE: printf("GTE >=\n"); break;
			default: printf("UNKNOWN OP\n"); break;
		}
		int i = 0, size = node->_int;
		int* children = (int*) node->_data;
		while(i != size) {
			if(children[i] < 0)
				pbg_print_h(e, e->_dynamic + (-children[i++]-1), depth+1);
			else
				pbg_print_h(e, e->_static + children[i++], depth+1);
		}
	}
}

void pbg_print(pbg_expr* e)
{
	pbg_print_h(e, e->_static, 0);
}


/*********************************
 *                               *
 * CONVERSION & CHECKING TOOLKIT *
 *                               *
 *********************************/

char* pbg_error_str(pbg_error_type type)
{
	switch(type) {
		case PBG_ERR_NONE:         return "PBG_ERR_NONE";
		case PBG_ERR_ALLOC:        return "PBG_ERR_ALLOC";
		case PBG_ERR_STATE:        return "PBG_ERR_STATE";
		case PBG_ERR_SYNTAX:       return "PBG_ERR_SYNTAX";
		case PBG_ERR_UNKNOWN_TYPE: return "PBG_ERR_UNKNOWN_TYPE";
		case PBG_ERR_OP_ARITY:     return "PBG_ERR_OP_ARITY";
		case PBG_ERR_OP_ARG_TYPE:  return "PBG_ERR_OP_ARG_TYPE";
	}
	return "PBG_ERR_???";
}

pbg_node_type pbg_gettype(char* str, int n)
{
	/* Is it a literal? */
	if(pbg_istrue(str, n))
		return PBG_LT_TRUE;
	if(pbg_isfalse(str, n))
		return PBG_LT_FALSE;
	if(pbg_isnumber(str, n))
		return PBG_LT_NUMBER;
	if(pbg_isstring(str, n))
		return PBG_LT_STRING;
	if(pbg_isdate(str, n))
		return PBG_LT_DATE;
	if(pbg_iskey(str, n))
		return PBG_LT_KEY;
	
	/* Is it an operator? */
	if(n == 1) {
		if(str[0] == '!') return PBG_OP_NOT;
		if(str[0] == '&') return PBG_OP_AND;
		if(str[0] == '|') return PBG_OP_OR;
		if(str[0] == '=') return PBG_OP_EQ;
		if(str[0] == '<') return PBG_OP_LT;
		if(str[0] == '>') return PBG_OP_GT;
		if(str[0] == '?') return PBG_OP_EXST;
	}
	if(n == 2) {
		if(str[0] == '!' && str[1] == '=') return PBG_OP_NEQ;
		if(str[0] == '<' && str[1] == '=') return PBG_OP_LTE;
		if(str[0] == '>' && str[1] == '=') return PBG_OP_GTE;
	}
	
	/* It isn't anything! */
	return PBG_UNKNOWN;
}

int pbg_istrue(char* str, int n)
{
	return n == 4 && 
		str[0] == 'T' && 
		str[1] == 'R' && 
		str[2] == 'U' && 
		str[3] == 'E';
}

int pbg_isfalse(char* str, int n)
{
	return n == 5 && 
		str[0] == 'F' && 
		str[1] == 'A' && 
		str[2] == 'L' && 
		str[3] == 'S' && 
		str[4] == 'E';
}

int pbg_isnumber(char* str, int n)
{
	int i = 0;
	
	/* Check if negative or positive */
	if(str[i] == '-' || str[i] == '+') i++;
	/* Otherwise, ensure first character is a digit. */
	else if(!is_a_digit(str[i]))
		return 0;
	
	/* Parse everything before the dot. */
	if(str[i] != '0' && is_a_digit(str[i])) {
		while(i != n && is_a_digit(str[i])) i++;
		if(i != n && !is_a_digit(str[i]) && str[i] != '.') return 0;
	}else if(str[i] == '0') {
		if(++i != n && !(str[i] == '.' || str[i] == 'e' || str[i] == 'E')) return 0;
	}
	
	/* Parse everything after the dot. */
	if(str[i] == '.') {
		/* Last character must be a digit. */
		if(i++ == n-1) return 0;
		/* Exhaust all digits. */
		while(i != n && is_a_digit(str[i])) i++;
		if(i != n && !is_a_digit(str[i]) && str[i] != 'e' && str[i] != 'E') return 0;
	}
	
	/* Parse everything after the exponent. */
	if(str[i] == 'e' || str[i] == 'E') {
		/* Last character must be a digit. */
		if(i++ == n-1) return 0;
		/* Parse positive or negative sign. */
		if(str[i] == '-' || str[i] == '+') i++;
		/* Exhaust all digits. */
		while(i != n && is_a_digit(str[i])) i++;
		if(i != n && !is_a_digit(str[i])) return 0;
	}
	
	/* Probably a number! */
	return 1;
}

void pbg_tonumber(pbg_number_lt* ptr, char* str, int n)
{
	ptr->_val = atof(str);
}

int pbg_iskey(char* str, int n)
{
	return str[0] == '[' && str[n-1] == ']';
}

int pbg_isstring(char* str, int n)
{
	return str[0] == '\'' && str[n-1] == '\'';
}

int pbg_isdate(char* str, int n)
{
	return n == 10 &&
		str[0] >= '0' && str[0] <= '9' && 
		str[1] >= '0' && str[1] <= '9' && 
		str[2] >= '0' && str[2] <= '9' && 
		str[3] >= '0' && str[3] <= '9' && 
		str[4] == '-' && 
		str[5] >= '0' && str[5] <= '9' && 
		str[6] >= '0' && str[6] <= '9' && 
		str[7] == '-' && 
		str[8] >= '0' && str[8] <= '9' && 
		str[9] >= '0' && str[9] <= '9';
}

void pbg_todate(pbg_date_lt* ptr, char* str, int n)
{
	/* Year. */
	ptr->_YYYY = (str[0]-'0')*1000 + (str[1]-'0')*100 + (str[2]-'0')*10 + (str[3]-'0');
	/* Month. */
	ptr->_MM = (str[5]-'0')*10 + (str[6]-'0');
	/* Day. */
	ptr->_DD = (str[8]-'0')*10 + (str[9]-'0');
	// TODO enforce ranges on months and days
}


/********************
 *                  *
 * HELPER FUNCTIONS *
 *                  *
 ********************/

/**
 * Checks if the given character is a digit.
 * @param c  Character to check.
 */
int is_a_digit(char c)
{
	return c >= '0' && c <= '9';
}

int is_whitespace(char c)
{
	return c == ' ' || c == '\t' || c == '\n';
}
