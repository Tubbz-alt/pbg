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

int test_pbg_evaluate(char* str, int expected)
{
	pbg_expr e;
	pbg_parse(&e, str, strlen(str));
	int result = pbg_evaluate(&e, NULL) != expected;
	if(result == 1)
		printf("failed: \"%s\", expected=%d\n", str, expected);
	pbg_free(&e);
	return result;
}

int test_pbg_parse(char* str)
{
	pbg_expr e;
	pbg_parse(&e, str, strlen(str));
	char* getstr = pbg_gets(&e, NULL, 0);
	printf("%s\n", getstr);
	free(getstr);
	pbg_free(&e);
}

int test_pbg_print(char* str) 
{
	pbg_expr e;
	pbg_parse(&e, str, strlen(str));
	char* getstr = pbg_gets(&e, NULL, 0);
	printf("given: %s\n", str);
	printf("parse: %s\n", getstr);
	printf("parse tree:\n");
	pbg_print(&e);
	free(getstr);
	pbg_free(&e);
}

int test_pbg_toop(char* str, pbg_node_type expected)
{
	pbg_node_type op = pbg_toop(str, strlen(str));
	int result = op != expected;
	if(result == 1)
		printf("failed: \"%s\", expected=%d, got=%d\n", str, expected, op);
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
	result += test_pbg_istype(pbg_isnumber, "3.14e1", 1);
	result += test_pbg_istype(pbg_isnumber, "3", 1);
	result += test_pbg_istype(pbg_isnumber, "03", 0);
	result += test_pbg_istype(pbg_isnumber, "3.", 0);
	result += test_pbg_istype(pbg_isnumber, "3.0", 1);
	result += test_pbg_istype(pbg_isnumber, ".", 0);
	result += test_pbg_istype(pbg_isnumber, "0", 1);
	result += test_pbg_istype(pbg_isnumber, "0.123a45", 0);
	result += test_pbg_istype(pbg_isnumber, "0.0.1", 0);
	result += test_pbg_istype(pbg_isnumber, "0.0", 1);
	result += test_pbg_istype(pbg_isnumber, "e10", 0);
	result += test_pbg_istype(pbg_isnumber, ".0", 0);
	result += test_pbg_istype(pbg_isnumber, "0e10", 1);
	result += test_pbg_istype(pbg_isnumber, "0e", 0);
	result += test_pbg_istype(pbg_isnumber, "'hi'", 0);
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
	
	/* pbg_toop */
	result = 0;
	result += test_pbg_toop("!", PBG_OP_NOT);
	result += test_pbg_toop("&", PBG_OP_AND);
	result += test_pbg_toop("|", PBG_OP_OR);
	result += test_pbg_toop("=", PBG_OP_EQ);
	result += test_pbg_toop("<", PBG_OP_LT);
	result += test_pbg_toop(">", PBG_OP_GT);
	result += test_pbg_toop("?", PBG_OP_EXST);
	result += test_pbg_toop("!=", PBG_OP_NEQ);
	result += test_pbg_toop(">=", PBG_OP_GTE);
	result += test_pbg_toop("<=", PBG_OP_LTE);
	PBG_RESULT("pbg_toop", result);
	
	/* pbg_parse */
	result = 0;
	test_pbg_parse("(&,TRUE,TRUE)");
	PBG_RESULT("pbg_parse", result);
	
	/* pbg_evaluate */
	result = 0;
	/* NOT */
	result += test_pbg_evaluate("(!,FALSE)", 1);
	result += test_pbg_evaluate("(!,TRUE)", 0);
	result += test_pbg_evaluate("(!,(=,10,10))", 0);
	result += test_pbg_evaluate("(!,(=,9,10))", 1);
	result += test_pbg_evaluate("(!,(!,(=,9,10)))", 0);
	/* AND */
	result += test_pbg_evaluate("(&,TRUE,TRUE)", 1);
	result += test_pbg_evaluate("(&,TRUE,FALSE)", 0);
	result += test_pbg_evaluate("(&,FALSE,TRUE)", 0);
	result += test_pbg_evaluate("(&,FALSE,FALSE)", 0);
	/* OR */
	result += test_pbg_evaluate("(|,TRUE,TRUE)", 1);
	result += test_pbg_evaluate("(|,TRUE,FALSE)", 1);
	result += test_pbg_evaluate("(|,FALSE,TRUE)", 1);
	result += test_pbg_evaluate("(|,FALSE,FALSE)", 0);
	/* EQUAL */
	result += test_pbg_evaluate("(=,10,10)", 1);
	result += test_pbg_evaluate("(=,9,10)", 0);
	result += test_pbg_evaluate("(=,10,9)", 0);
	result += test_pbg_evaluate("(=,'hi',9)", 0);
	result += test_pbg_evaluate("(=,-10,'hi')", 0);
	result += test_pbg_evaluate("(=,-10,-10)", 1);
	result += test_pbg_evaluate("(=,'hi','hi')", 1);
	result += test_pbg_evaluate("(=,'hia','hi')", 0);
	result += test_pbg_evaluate("(=,'hi','h ')", 0);
	result += test_pbg_evaluate("(=,2018-10-12,'h ')", 0);
	result += test_pbg_evaluate("(=,2018-10-12,2018-10-12)", 1);
	result += test_pbg_evaluate("(=,2018-10-12,2018-10-13)", 0);
	result += test_pbg_evaluate("(=,2018-10-13,2018-10-12)", 0);
	result += test_pbg_evaluate("(=,2018-10-13,2017-10-13)", 0);
	result += test_pbg_evaluate("(=,2018-10-13,2018-11-13)", 0);
	result += test_pbg_evaluate("(=,[ab],[ab])", 1);
	result += test_pbg_evaluate("(=,[ab],[ac])", 0);
	result += test_pbg_evaluate("(=,[abc],[ab])", 0);
	result += test_pbg_evaluate("(=,TRUE,TRUE)", 1);
	result += test_pbg_evaluate("(=,TRUE,FALSE)", 0);
	result += test_pbg_evaluate("(=,FALSE,TRUE)", 0);
	result += test_pbg_evaluate("(=,FALSE,FALSE)", 1);
	/* LESS THAN */
	result += test_pbg_evaluate("(<,2,3)", 1);
	result += test_pbg_evaluate("(<,3,3)", 0);
	result += test_pbg_evaluate("(<,-3,3)", 1);
	/* GREATER THAN */
	result += test_pbg_evaluate("(>,3,2)", 1);
	result += test_pbg_evaluate("(>,3,3)", 0);
	result += test_pbg_evaluate("(>,3,-3)", 1);
	/* LESS THAN OR EQUAL */
	result += test_pbg_evaluate("(<=,2,3)", 1);
	result += test_pbg_evaluate("(<=,3,3)", 1);
	result += test_pbg_evaluate("(<=,-3,3)", 1);
	result += test_pbg_evaluate("(<=,3,-3)", 0);
	/* GREATER THAN OR EQUAL */
	result += test_pbg_evaluate("(>=,3,2)", 1);
	result += test_pbg_evaluate("(>=,3,3)", 1);
	result += test_pbg_evaluate("(>=,3,-3)", 1);
	result += test_pbg_evaluate("(>=,-3,3)", 0);
	PBG_RESULT("pbg_evaluate", result);
	
	return 0;
}
