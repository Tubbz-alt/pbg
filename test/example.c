#include "../pbg.h"
#include <stdio.h>
#include <string.h>

/* Our dictionary prototype! */
pbg_field dictionary(char* key, int n);

int main(void)
{
	pbg_error err;
	pbg_expr e;
	int result;
	
	/* This is the expression string we'll evaluate using our dictionary.
	 * It asks, "Are [a] and [b] the same, AND does [d] exist?" */
	char* str = "(&(=[a][b])(?[d]))";
	
	/* Parse the expression string and check if 
	 * there were any compilation errors. */
	pbg_parse(&e, &err, str);
	if(pbg_iserror(&err)) {
		pbg_error_print(&err);
		return 1;
	}
	
	/* Evaluate the expression string and check if 
	 * there were any runtime errors. */
	result = pbg_evaluate(&e, &err, dictionary);
	if(pbg_iserror(&err)) {
		pbg_error_print(&err);
		return 1;
	}
	
	/* Success! Print the output. */
	printf("%s is %s\n", str, (result == PBG_TRUE) ? "TRUE" : "FALSE");
	return 0;
}

/* This is a simple, handmade dictionary. A more general implementation would 
 * (hopefully) utilize a dictionary data structure. */
pbg_field dictionary(char* key, int n)
{
	PBG_UNUSED(n);  /* Ignore compiler warnings. */
	if(key[0] == 'a' || key[0] == 'b')
		return pbg_make_number(5.0);
	if(key[0] == 'c')
		return pbg_make_number(6.0);
	return pbg_make_null();
}