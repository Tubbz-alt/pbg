#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../pbg.h"

#define PBG_RESULT(name,num) if((num) == 0) printf("%s:\tPassed!\n", name); else printf("%s:\t%d tests failed!\n", name, (num)); result = 0


int test_pbg_istype(int (*pbg_func)(char*,int), char* str, int expected)
{
	int result = (*pbg_func)(str, strlen(str)) != expected;
	if(result == 1)
		printf("failed: \"%s\", expected=%d\n", str, expected);
	return result;
}


int main()
{
	int result;
	
	/* pbg_istrue */
	result = 0;
	result += test_pbg_istype(pbg_istrue, "TRUE", 1);
	result += test_pbg_istype(pbg_istrue, " TRUE", 0);
	result += test_pbg_istype(pbg_istrue, " TRUE ", 0);
	result += test_pbg_istype(pbg_istrue, " TR UE ", 0);
	result += test_pbg_istype(pbg_istrue, "true", 0);
	result += test_pbg_istype(pbg_istrue, "tRue", 0);
	PBG_RESULT("pbg_istrue", result);
	
	/* pbg_isfalse */
	result = 0;
	result += test_pbg_istype(pbg_isfalse, "FALSE", 1);
	result += test_pbg_istype(pbg_isfalse, " FALSE", 0);
	result += test_pbg_istype(pbg_isfalse, " FALSE ", 0);
	result += test_pbg_istype(pbg_isfalse, " FAL SE ", 0);
	result += test_pbg_istype(pbg_isfalse, "false", 0);
	result += test_pbg_istype(pbg_isfalse, "fAlse", 0);
	PBG_RESULT("pbg_isfalse", result);
	
	/* pbg_isnumber */
	result = 0;
	PBG_RESULT("pbg_isnumber", result);
	
	/* pbg_iskey */
	result = 0;
	PBG_RESULT("pbg_iskey", result);
	
	/* pbg_isstring */
	result = 0;
	PBG_RESULT("pbg_isstring", result);
	
	/* pbg_isdate */
	result = 0;
	PBG_RESULT("pbg_isdate", result);
	
	/* pbg_parse */
	pbg_expr* e = (pbg_expr*) malloc(sizeof(pbg_expr));
	pbg_parse(e, "(!,TRUE)", 8);
	pbg_free(e);
	
	return 0;
}