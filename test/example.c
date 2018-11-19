#include "../pbg.h"
#include <stdio.h>
#include <string.h>

/* Our dictionary prototype! */
pbg_field dictionary(char* key, int n);

int main(void)
{
	pbg_error err;
	pbg_expr e;
	
	/* This is the expression string we'll evaluate using our dictionary. */
	/* "Are [a] and [b] the same, and does [d] exist?" */
	char* str = "(&(=[a][b])(?[d]))";
	
	/* Parse the expression string and check if 
	 * there were any compilation errors. */
	pbg_parse(&e, &err, str, strlen(str));
	if(pbg_iserror(&err)) {
		pbg_error_print(&err);
		return 1;
	}
	
	/* Evaluate the expression string and check if 
	 * there were any runtime errors. */
	int result = pbg_evaluate(&e, &err, dictionary);
	if(pbg_iserror(&err)) {
		pbg_error_print(&err);
		return 1;
	}
	
	/* Success! Print the output. */
	printf("%s is %s\n", str, (result == 1) ? "TRUE" : "FALSE");
	return 0;
}

/* This is a simple, handmade dictionary. A more general implementation would 
 * (hopefully) utilize a dictionary data structure. */
pbg_field dictionary(char* key, int n)
{
	PBG_UNUSED(n);  /* Ignore compiler warnings. */
	if(key[0] == 'a' || key[0] == 'b')
		return pbg_make_number(NULL, "5.0", strlen("5.0"));
	if(key[0] == 'c')
		return pbg_make_number(NULL, "6.0", strlen("6.0"));
	return pbg_make_field(PBG_NULL);
}