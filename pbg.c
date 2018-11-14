#include "pbg.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void pbg_toop(pbg_node_type* op, char* str, int n)
{
	if(n == 1)
		if(str[0] == '!')      *op = PBG_OP_NOT;
		else if(str[0] == '&') *op = PBG_OP_AND;
		else if(str[0] == '|') *op = PBG_OP_OR;
		else if(str[0] == '=') *op = PBG_OP_EQ;
		else if(str[0] == '<') *op = PBG_OP_LT;
		else if(str[0] == '>') *op = PBG_OP_GT;
		else if(str[0] == '?') *op = PBG_OP_EXST;
		else *op = PBG_UNKNOWN;
	else if(n == 2)
		if(str[0] == '!' && str[1] == '=')      *op = PBG_OP_NEQ;
		else if(str[0] == '<' && str[1] == '=') *op = PBG_OP_LTE;
		else if(str[0] == '>' && str[1] == '=') *op = PBG_OP_GTE;
		else *op = PBG_UNKNOWN;
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


int is_a_digit(char c) { return c >= '0' && c <= '9'; }

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


int pbg_keyhash(char* key, int n)
{
	int i = 0, hash = 0;
	while(i < n) hash = 31 * hash + key[i++];
	return hash;
}

int pbg_parse_h(pbg_expr* e, pbg_expr_node* node, char* str, int n)
{
	int i, j;
	
	/* Initialize provided struct. */
	node->_size = 0;
	node->_type = PBG_UNKNOWN;
	
	/* Every internal node is surrounded by one pair of parentheses. */
	if(str[0] == '(' && str[n-1] == ')') {
		
		/* Count number of children nodes. */
		int depth = 0;
		for(i = 0; i < n && depth >= 0; i++)
			if(str[i] == '(') depth++;
			else if(str[i] == ')') depth--;
			else if(str[i] == ',' && depth == 1) node->_size++;
		if(depth) {
			// TODO unbalanced parentheses
		}
		if(node->_size == 0) {
			// TODO no arguments provided to operator
		}
		
		/* Get string length of each child node. */
		int* lens = (int*) calloc(node->_size+1, sizeof(int));
		if(lens == NULL) {
			// TODO failed to malloc enough space for array
		}
		for(i = 0, j = 0; i < n; i++) {
			/* Track depth in tree. */
			if(str[i] == '(') depth++;
			else if(str[i] == ')') depth--;

			/* Compute length of child. */
			if(str[i] == ',' && depth == 1) j++;
			else if(depth > 0 && i != 0) lens[j]++;
		}
		
		/* Parse the operator. */
		pbg_toop(&(node->_type), str+1, lens[0]);
		if(node->_type == PBG_UNKNOWN) {
			// TODO unsupported operation
		}
		
		/* Enforce operator arity. */
		if((node->_type == PBG_OP_NOT || node->_type == PBG_OP_EXST) && node->_size != 1) {
			// TODO these are unary operators
		}else if((node->_type == PBG_OP_AND || node->_type == PBG_OP_OR || 
					node->_type == PBG_OP_EQ) && node->_size < 2) {
			// TODO these are 2+ary operators
		}else if((node->_type == PBG_OP_NEQ || node->_type == PBG_OP_LT || 
					node->_type == PBG_OP_GT || node->_type == PBG_OP_GTE || 
					node->_type == PBG_OP_LTE) && node->_size != 2) {
			// TODO these are binary operators
		}
		
		/* Allocate space for children. */
		node->_data = malloc(node->_size * sizeof(pbg_expr_node));
		if(node->_data == NULL) {
			// TODO failed to malloc enough space for node
		}
		
		/* Parse each child. */
		pbg_expr_node* children = (pbg_expr_node*) node->_data;
		for(i = 0, j = 1, depth = 0; i < n; i++)
			if(str[i] == '(') depth++;
			else if(str[i] == ')') depth--;
			else if(str[i] == ',' && depth == 1) {
				if(!lens[j]) {
					// TODO no argument provided
				}
				int err = pbg_parse_h(e, children+j-1, str+i+1, lens[j]);
				if(err) {
					// TODO error in child
				}
				j++;
			}

		/* Done! Clean up. */
		free(lens);
		
	/* No leaf node is surrounded by parentheses. */
	}else{
		/* KEY. Copy key identifier into string. */
		if(pbg_iskey(str, n)) {
			/* Get global KEY if it exists. If not, create it. */
			// TODO
			/* Make child pointer point to associated global KEY struct. */
			// TODO
			
			node->_size = (n-2) * sizeof(char);
			node->_data = malloc(node->_size);
			if(node->_data == NULL) {
				// TODO failed to allocate memory for node
			}
			strncpy(node->_data, str+1, n-2);
			node->_type = PBG_LT_KEY;
			
		/* DATE. Convert to PBG DATE constant. */
		}else if(pbg_isdate(str, n)) {
			node->_size = sizeof(pbg_type_date);
			node->_data = malloc(node->_size);
			if(node->_data == NULL) {
				// TODO failed to allocate memory for node
			}
			pbg_todate((pbg_type_date*)node->_data, str, n);
			node->_type = PBG_LT_DATE;
			
		/* NUMBER. Parse entire element as a float. */
		}else if(pbg_isnumber(str, n)) {
			node->_size = sizeof(double);
			node->_data = malloc(node->_size);
			if(node->_data == NULL) {
				// TODO failed to allocate memory for node
			}
			*((double*)node->_data) = atof(str);
			node->_type = PBG_LT_NUMBER;
		
		/* STRING. Copy everything between single quotes. */
		}else if(pbg_isstring(str, n)) {
			node->_size = (n-2) * sizeof(char);
			node->_data = malloc(node->_size);
			if(node->_data == NULL) {
				// TODO failed to allocate memory for node
			}
			strncpy((char*)node->_data, str+1, n-2);
			node->_type = PBG_LT_STRING;
			
		/* TRUE. No need for any data. */
		}else if(pbg_istrue(str, n)) {
			node->_size = 0;
			node->_data = NULL;
			node->_type = PBG_LT_TRUE;
		
		/* FALSE. No need for any data. */
		}else if(pbg_isfalse(str, n)) {
			node->_size = 0;
			node->_data = NULL;
			node->_type = PBG_LT_FALSE;
			
		}else{
			// TODO not a valid literal
		}
	}
	/* No error! */
	return 0;
}

int pbg_parse(pbg_expr* e, char* str, int n)
{
	if((str[0] != '(' || str[n-1] != ')') && !pbg_istrue(str, n) && 
			!pbg_isfalse(str, n)) {
		// TODO outermost unit must have a truth value
	}
	
	/* Initialize expression struct. */
	e->_root = (pbg_expr_node*) malloc(sizeof(pbg_expr_node));
	if(e->_root == NULL) {
		// TODO failed to malloc necessary memory
	}
	e->_keysz = 8;
	e->_keys = (pbg_expr_node*) malloc(e->_keysz * sizeof(pbg_expr_node));
	if(e->_keys == NULL) {
		// TODO failed to malloc necessary memory
	}
	e->_keyc = 0;
	
	/* Parse all nodes in expression. */
	return pbg_parse_h(e, e->_root, str, n);
}


void pbg_free_r(pbg_expr_node* node)
{
	/* This node is a literal. Just need to free the _data. */
	if(node->_type < PBG_MAX_LT) {
		switch(node->_type) {
			case PBG_LT_KEY:
			case PBG_LT_NUMBER:
			case PBG_LT_DATE:
			case PBG_LT_STRING:
				free(node->_data);
			default: break;
		}
		
	/* This node is an operator. Free all of its children and the _data. */
	}else if(node->_type < PBG_MAX_OP) {
		pbg_expr_node* children = (pbg_expr_node*) node->_data;
		for(int i = 0; i < node->_size; i++)
			pbg_free_r(children+i);
		free(node->_data);
	}
}

void pbg_free(pbg_expr* e)
{
	pbg_free_r(e->_root);
}


int pbg_evaluate_h(pbg_expr* e, pbg_expr_node* node, pbg_expr* (*dict)(char*, int))
{
	/* This is a literal node. */
	if(node->_type < PBG_MAX_LT) {
		if(node->_type == PBG_LT_TRUE) return 1;
		if(node->_type == PBG_LT_FALSE) return 0;
		return 0;  // TODO the only literals with truth values are TRUE and FALSE.
		
	/* This is an operator node. */
	}else if(node->_type < PBG_MAX_OP) {
		pbg_expr_node* children = (pbg_expr_node*) node->_data;
		int size = node->_size;
		switch(node->_type) {
			/* NOT: invert the truth value of the contained expression. */
			case PBG_OP_NOT:
				return (pbg_evaluate_h(e, (pbg_expr_node*) node->_data, dict) == 0);
				break;
			/* AND: true only if all subexpressions are true. */
			case PBG_OP_AND:
				for(int i = 0; i < size; i++)
					if(pbg_evaluate_h(e, ((pbg_expr_node*)node->_data)+i, dict) == 0)
						return 0;
				return 1;
				break;
			/* OR: true if any of the subexpressions are true. */
			case PBG_OP_OR:
				for(int i = 0; i < size; i++)
					if(pbg_evaluate_h(e, ((pbg_expr_node*)node->_data)+i, dict) == 1)
						return 1;
				return 0;
				break;
			/* EQ: true only all children are equal to each other. */
			case PBG_OP_EQ:
				/* Ensure type and size of all children are identical. */
				for(int i = 1; i < size; i++)
					if(children[i]._type != children[0]._type || 
							children[i]._size != children[0]._size)
						return 0;
						
					/* Ensure each data byte is identical. */
				for(int i = 1; i < size; i++)
					for(int j = 0; j < children[0]._size; j++)
						if(((char*)children[0]._data)[j] != ((char*)children[i]._data)[j])
							return 0;
				return 1;
				break;
			/* LT: true only if the first child is less than the second. */
			case PBG_OP_LT:
				return *((double*)children[0]._data) < *((double*)children[1]._data);
				break;
			/* GT: true only if the first child is greater than the second. */
			case PBG_OP_GT:
				return *((double*)children[0]._data) > *((double*)children[1]._data);
				break;
			/* EXST: true only if the KEY exists in the given dictionary. */
			case PBG_OP_EXST:
				return dict((char*) node->_data, node->_size) != NULL;
				break;
			/* NEQ: true only if the two children are different. */
			case PBG_OP_NEQ:
				return children[1]._type != children[0]._type || 
						children[1]._size != children[0]._size || 
						strncmp(children[1]._data, children[0]._data, children[0]._size);
				break;
			/* LTE: true only if the first child is at most the second. */
			case PBG_OP_LTE:
				return *((double*)children[0]._data) <= *((double*)children[1]._data);
				break;
			/* GTE: true only if the first child is at least the second. */
			case PBG_OP_GTE:
				return *((double*)children[0]._data) >= *((double*)children[1]._data);
				break;
		}
	}else{
		// TODO should never get here!
	}
}

int pbg_evaluate(pbg_expr* e, pbg_expr* (*dict)(char*, int))
{
	return pbg_evaluate_h(e, e->_root, dict);
}


int pbg_gets_r(pbg_expr_node* node, char* buf, int i)
{
	/* This node is a literal. */
	if(node->_type < PBG_MAX_LT)
	{
		char* data;
		pbg_type_date* dt;
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
				for(int j = 0; j < node->_size; j++)
					buf[i++] = data[j];
				buf[i++] = '\'';
				break;
			case PBG_LT_DATE:
				dt = (pbg_type_date*) node->_data;
				i += snprintf(buf+i, 5, "%04d", dt->_YYYY);
				buf[i++] = '-';
				i += snprintf(buf+i, 3, "%02d", dt->_MM);
				buf[i++] = '-';
				i += snprintf(buf+i, 3, "%02d", dt->_DD);
				break;
			case PBG_LT_KEY:
				data = (char*) node->_data;
				buf[i++] = '[';
				for(int j = 0; j < node->_size; j++)
					buf[i++] = data[j];
				buf[i++] = ']';
				break;
			case PBG_LT_NUMBER:
				i += sprintf(buf+i, "%.2lf", *((double*)node->_data));
				break;
			default:
				// TODO unknown operator!
				break;
		}
	
	/* This node is an operator. */
	}else if(node->_type < PBG_MAX_OP) {
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
				// TODO unknown operator!
				break;
		}
		buf[i++] = ',';
		pbg_expr_node* children = (pbg_expr_node*)node->_data;
		for(int j = 0; j < node->_size; j++) {
			i = pbg_gets_r(&children[j], buf, i);
			if(j != node->_size-1)
				buf[i++] = ',';
		}
		buf[i++] = ')';
	}
	return i;
}

char* pbg_gets(pbg_expr* e, char** bufptr, int n)
{
	char* buf = (char*) malloc(1000);  // TODO fix me then mix me
	int len = pbg_gets_r(e->_root, buf, 0);
	buf[len] = '\0';
	return buf;
}

void pbg_print_h(pbg_expr_node* node, int depth)
{
	for(int i = 0; i < depth; i++)
		printf("  ");
	if(node->_type < PBG_MAX_LT) {
		pbg_type_date* date;
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
				for(int i = 0; i < node->_size; i++)
					printf("%c", ((char*)node->_data)[i]);
				printf("'\n");
				break;
			case PBG_LT_DATE:
				date = (pbg_type_date*)node->_data;
				printf("DATE : %4d-%2d-%2d\n", date->_YYYY, date->_MM, date->_DD);
				break;
			case PBG_LT_KEY:
				printf("KEY : [");
				for(int i = 0; i < node->_size; i++)
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
		int i = 0, size = node->_size;
		pbg_expr_node* children = (pbg_expr_node*)node->_data;
		while(i != size)
			pbg_print_h(children+i++, depth+1);
	}
}

void pbg_print(pbg_expr* e)
{
	pbg_print_h(e->_root, 0);
}
