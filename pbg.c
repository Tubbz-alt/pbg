#include "pbg.h"
#include <stdlib.h>
#include <string.h>


void pbg_toop(pbg_expr_type* op, char* str, int n)
{
	if(n == 1)
		     if(str[0] == '!') *op = PBG_OP_NOT;
		else if(str[0] == '&') *op = PBG_OP_AND;
		else if(str[0] == '|') *op = PBG_OP_OR;
		else if(str[0] == '=') *op = PBG_OP_EQ;
		else if(str[0] == '<') *op = PBG_OP_LT;
		else if(str[0] == '>') *op = PBG_OP_GT;
		else if(str[0] == '?') *op = PBG_OP_EXST;
	else if(n == 2)
		     if(str[0] == '!' && str[1] == '=') *op = PBG_OP_NEQ;
		else if(str[0] == '<' && str[1] == '=') *op = PBG_OP_LTE;
		else if(str[0] == '>' && str[1] == '=') *op = PBG_OP_GTE;
	else
		*op = PBG_UNKNOWN;
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
	return 0;
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


void pbg_todate(pbg_type_date* ptr, char* str, int n)
{
	ptr->_YYYY = (str[0]-'0')*1000 + (str[1]-'0')*100 + (str[2]-'0')*10 + (str[3]-'0');
	ptr->_MM = (str[5]-'0')*10 + (str[6]-'0');
	ptr->_DD = (str[8]-'0')*10 + (str[9]-'0');
	// TODO enforce ranges on months and days
}


int pbg_parse(pbg_expr* e, char* str, int n)
{
	int i, j;
	
	/* Initialize provided struct. */
	e->_size = 0;
	
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
				e->_size++;
		if(depth) {
			// TODO unbalanced parentheses
		}
		if(e->_size == 0) {
			// TODO no arguments provided to operator
		}
		
		/* Get length of each child node. */
		int* lens = (int*) malloc((e->_size+1) * sizeof(int));
		if(lens == NULL) {
			// TODO failed to malloc enough space for array
		}
		for(i = 0; i < n; i++)
			if(str[i] == '(') depth++;
			else if(str[i] == ')') depth--;
			else if(str[i] == ',' && depth == 0) j++;
			else lens[j]++;
		
		/* Parse the operator. */
		pbg_toop(&e->_type, str, lens[0]);
		if(e->_type == PBG_UNKNOWN) {
			// TODO unsupported operation
		}
		
		/* Enforce operator arity. */
		if((e->_type == PBG_OP_NOT || e->_type == PBG_OP_EXST) && e->_size != 1) {
			// TODO these are unary operators
		}else if((e->_type == PBG_OP_AND || e->_type == PBG_OP_OR || 
					e->_type == PBG_OP_EQ) && e->_size < 2) {
			// TODO these are 2+ary operators
		}else if((e->_type == PBG_OP_NEQ || e->_type == PBG_OP_LT || 
					e->_type == PBG_OP_GT || e->_type == PBG_OP_GTE || 
					e->_type == PBG_OP_LTE) && e->_size != 2) {
			// TODO these are binary operators
		}
		
		/* Allocate space for children. */
		e->_data = malloc(e->_size * sizeof(pbg_expr));
		if(e->_data == NULL) {
			// TODO failed to malloc enough space for node
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
				int err = pbg_parse(e->_data+j-1, str+i+1, lens[j++]);
				if(err) {
					// TODO error in child
				}
			}
		
		/* No error! */
		return 0;
		
	/* No leaf node is surrounded by parentheses. */
	}else{
		/* KEY. Copy key identifier into string. */
		if(pbg_iskey(str, n)) {
			e->_size = (n-2) * sizeof(char);
			e->_data = malloc(e->_size);
			if(e->_data == NULL) {
				// TODO failed to allocate memory for node
			}
			strncpy(e->_data, str+1, n-2);
			e->_type = PBG_LT_KEY;
			
		/* DATE. Convert to PBG DATE constant. */
		}else if(pbg_isdate(str, n)) {
			e->_size = sizeof(pbg_type_date);
			e->_data = malloc(e->_size);
			if(e->_data == NULL) {
				// TODO failed to allocate memory for node
			}
			pbg_todate((pbg_type_date*)e->_data, str, n);
			e->_type = PBG_LT_DATE;
			
		/* NUMBER. Parse entire element as a float. */
		}else if(pbg_isnumber(str, n)) {
			e->_size = sizeof(double);
			e->_data = malloc(e->_size);
			if(e->_data == NULL) {
				// TODO failed to allocate memory for node
			}
			char old = str[n];  // numbers are always folloed by other chars
			str[n] = '\0';
			*((double*)e->_data) = atof(str);
			str[n] = old;
			e->_type = PBG_LT_NUMBER;
		
		/* STRING. Copy everything between single quotes. */
		}else if(pbg_isstring(str, n)) {
			e->_size = (n-2) * sizeof(char);
			e->_data = malloc(e->_size);
			if(e->_data == NULL) {
				// TODO failed to allocate memory for node
			}
			strncpy((char*)e->_data, str+1, n-2);
			e->_type = PBG_LT_STRING;
			
		/* TRUE. No need for any data. */
		}else if(pbg_istrue(str, n)) {
			e->_size = 0;
			e->_data = NULL;
			e->_type = PBG_LT_TRUE;
		
		/* FALSE. No need for any data. */
		}else if(pbg_isfalse(str, n)) {
			e->_size = 0;
			e->_data = NULL;
			e->_type = PBG_LT_FALSE;
			
		}else{
			// TODO not a valid literal
		}
	}
}


void pbg_free(pbg_expr* e)
{
	
}


void pbg_evaluate(pbg_expr* e)
{
	
}


void pbg_gets(pbg_expr* e, char** bufptr, int n)
{
	
}
