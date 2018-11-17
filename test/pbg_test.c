#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pbg_test.h"
#include "../pbg.h"


/* Test suites in this file. */
int suite_evaluate();
int suite_gettype();


/* Run and summarize test suites. */
int main()
{
	summ_test("pbg_gettype", suite_gettype());
	summ_test("pbg_evaluate", suite_evaluate());
	return 0;
}


/***************
 *             *
 * TEST SUITES *
 *             *
 ***************/

/* This is a dictionary used for testing purposes. 
 * It defines keys [a]=5.0, [b]=5.0, and [c]=6.0. */
pbg_expr_node dict(char* key, int n)
{
	pbg_expr_node keylt;
	keylt._type = PBG_UNKNOWN;
	keylt._int = -1;
	keylt._data = NULL;
	if(key[0] == 'a' || key[0] == 'b') {
		keylt._type = PBG_LT_NUMBER;
		keylt._int = sizeof(double);
		keylt._data = malloc(keylt._int);
		*((double*)keylt._data) = 5.0;
	}else if(key[0] == 'c') {
		keylt._type = PBG_LT_NUMBER;
		keylt._int = sizeof(double);
		keylt._data = malloc(keylt._int);
		*((double*)keylt._data) = 6.0;
	}
	return keylt;
}


/* Tests for pbg_evaluate. */
int suite_evaluate()
{
	init_test();
	
	/* TRUE */
	check(test_evaluate(&err, "(TRUE)", dict, TRUE));
	/* FALSE */
	check(test_evaluate(&err, "(FALSE)", dict, FALSE));
	/* NOT */
    check(test_evaluate(&err, "(!,FALSE)", dict, TRUE));
    check(test_evaluate(&err, "(!,TRUE)", dict, FALSE));
    check(test_evaluate(&err, "(!,(=,10,10))", dict, FALSE));
    check(test_evaluate(&err, "(!,(=,9,10))", dict, TRUE));
    check(test_evaluate(&err, "(!,(!,(=,9,10)))", dict, FALSE));
    /* AND */
    check(test_evaluate(&err, "(&,TRUE,TRUE)", dict, TRUE));
    check(test_evaluate(&err, "(&,TRUE,FALSE)", dict, FALSE));
    check(test_evaluate(&err, "(&,FALSE,TRUE)", dict, FALSE));
    check(test_evaluate(&err, "(&,FALSE,FALSE)", dict, FALSE));
    check(test_evaluate(&err, "(&,TRUE,TRUE,TRUE,TRUE,TRUE)", dict, TRUE));
    check(test_evaluate(&err, "(&,TRUE,TRUE,TRUE,FALSE,TRUE)", dict, FALSE));
    /* OR */
    check(test_evaluate(&err, "(|,TRUE,TRUE)", dict, TRUE));
    check(test_evaluate(&err, "(|,TRUE,FALSE)", dict, TRUE));
    check(test_evaluate(&err, "(|,FALSE,TRUE)", dict, TRUE));
    check(test_evaluate(&err, "(|,FALSE,FALSE)", dict, FALSE));
    check(test_evaluate(&err, "(|,FALSE,FALSE,FALSE,FALSE,FALSE)", dict, FALSE));
    check(test_evaluate(&err, "(|,FALSE,TRUE,FALSE,FALSE,FALSE)", dict, TRUE));
    /* EXST */
    check(test_evaluate(&err, "(?,[c])", dict, TRUE));
    check(test_evaluate(&err, "(?,[d])", dict, FALSE));
	/* EQUAL */
    check(test_evaluate(&err, "(=,10,10)", dict, TRUE));
    check(test_evaluate(&err, "(=,9,10)", dict, FALSE));
    check(test_evaluate(&err, "(=,10,9)", dict, FALSE));
    check(test_evaluate(&err, "(=,'hi',9)", dict, FALSE));
    check(test_evaluate(&err, "(=,-10,'hi')", dict, FALSE));
    check(test_evaluate(&err, "(=,-10,-10)", dict, TRUE));
    check(test_evaluate(&err, "(=,'hi','hi')", dict, TRUE));
    check(test_evaluate(&err, "(=,'a,b\\'c','a,b\\'c')", dict, TRUE));
    check(test_evaluate(&err, "(=,'ab\\'c','ab\\'d')", dict, FALSE));
    check(test_evaluate(&err, "(=,'hia','hi')", dict, FALSE));
    check(test_evaluate(&err, "(=,'hi','h ')", dict, FALSE));
    check(test_evaluate(&err, "(=,2018-10-12,'h ')", dict, FALSE));
    check(test_evaluate(&err, "(=,2018-10-12,2018-10-12)", dict, TRUE));
    check(test_evaluate(&err, "(=,2018-10-12,2018-10-13)", dict, FALSE));
    check(test_evaluate(&err, "(=,2018-10-13,2018-10-12)", dict, FALSE));
    check(test_evaluate(&err, "(=,2018-10-13,2017-10-13)", dict, FALSE));
    check(test_evaluate(&err, "(=,2018-10-13,2018-11-13)", dict, FALSE));
    check(test_evaluate(&err, "(=,[a],[a])", dict, TRUE));
    check(test_evaluate(&err, "(=,[a],[b])", dict, TRUE));
    check(test_evaluate(&err, "(=,[a],[c])", dict, FALSE));
    check(test_evaluate(&err, "(=,[c],[b])", dict, FALSE));
    check(test_evaluate(&err, "(=,TRUE,TRUE)", dict, TRUE));
    check(test_evaluate(&err, "(=,TRUE,FALSE)", dict, FALSE));
    check(test_evaluate(&err, "(=,FALSE,TRUE)", dict, FALSE));
    check(test_evaluate(&err, "(=,FALSE,FALSE)", dict, TRUE));
	/* NEQ */
    check(test_evaluate(&err, "(!=,10,10)", dict, FALSE));
    check(test_evaluate(&err, "(!=,9,10)", dict, TRUE));
    check(test_evaluate(&err, "(!=,10,9)", dict, TRUE));
    check(test_evaluate(&err, "(!=,'hi',9)", dict, TRUE));
    check(test_evaluate(&err, "(!=,-10,'hi')", dict, TRUE));
    check(test_evaluate(&err, "(!=,-10,-10)", dict, FALSE));
    check(test_evaluate(&err, "(!=,'hi','hi')", dict, FALSE));
    check(test_evaluate(&err, "(!=,'a,b\\'c','a,b\\'c')", dict, FALSE));
    check(test_evaluate(&err, "(!=,'ab\\'c','ab\\'d')", dict, TRUE));
    check(test_evaluate(&err, "(!=,'hia','hi')", dict, TRUE));
    check(test_evaluate(&err, "(!=,'hi','h ')", dict, TRUE));
    check(test_evaluate(&err, "(!=,2018-10-12,'h ')", dict, TRUE));
    check(test_evaluate(&err, "(!=,2018-10-12,2018-10-12)", dict, FALSE));
    check(test_evaluate(&err, "(!=,2018-10-12,2018-10-13)", dict, TRUE));
    check(test_evaluate(&err, "(!=,2018-10-13,2018-10-12)", dict, TRUE));
    check(test_evaluate(&err, "(!=,2018-10-13,2017-10-13)", dict, TRUE));
    check(test_evaluate(&err, "(!=,2018-10-13,2018-11-13)", dict, TRUE));
    check(test_evaluate(&err, "(!=,[a],[a])", dict, FALSE));
    check(test_evaluate(&err, "(!=,[a],[b])", dict, FALSE));
    check(test_evaluate(&err, "(!=,[a],[c])", dict, TRUE));
    check(test_evaluate(&err, "(!=,[c],[b])", dict, TRUE));
    check(test_evaluate(&err, "(!=,TRUE,TRUE)", dict, FALSE));
    check(test_evaluate(&err, "(!=,TRUE,FALSE)", dict, TRUE));
    check(test_evaluate(&err, "(!=,FALSE,TRUE)", dict, TRUE));
    check(test_evaluate(&err, "(!=,FALSE,FALSE)", dict, FALSE));
    /* LESS THAN */
    check(test_evaluate(&err, "(<,2,3)", dict, TRUE));
    check(test_evaluate(&err, "(<,3,3)", dict, FALSE));
    check(test_evaluate(&err, "(<,-3,3)", dict, TRUE));
    /* GREATER THAN */
    check(test_evaluate(&err, "(>,3,2)", dict, TRUE));
    check(test_evaluate(&err, "(>,3,3)", dict, FALSE));
    check(test_evaluate(&err, "(>,3,-3)", dict, TRUE));
    /* LESS THAN OR EQUAL */
    check(test_evaluate(&err, "(<=,2,3)", dict, TRUE));
    check(test_evaluate(&err, "(<=,3,3)", dict, TRUE));
    check(test_evaluate(&err, "(<=,-3,3)", dict, TRUE));
    check(test_evaluate(&err, "(<=,3,-3)", dict, FALSE));
    /* GREATER THAN OR EQUAL */
    check(test_evaluate(&err, "(>=,3,2)", dict, TRUE));
    check(test_evaluate(&err, "(>=,3,3)", dict, TRUE));
    check(test_evaluate(&err, "(>=,3,-3)", dict, TRUE));
    check(test_evaluate(&err, "(>=,-3,3)", dict, FALSE));
	
	end_test();
}


/* Tests for pbg_gettype. */
int suite_gettype()
{
	init_test();
	
	/* operators */
	check(test_gettype("!", PBG_OP_NOT));
	check(test_gettype("&", PBG_OP_AND));
	check(test_gettype("|", PBG_OP_OR));
	check(test_gettype("=", PBG_OP_EQ));
	check(test_gettype("<", PBG_OP_LT));
	check(test_gettype(">", PBG_OP_GT));
	check(test_gettype("?", PBG_OP_EXST));
	check(test_gettype("!=", PBG_OP_NEQ));
	check(test_gettype("<=", PBG_OP_LTE));
	check(test_gettype(">=", PBG_OP_GTE));
	check(test_gettype("!!", PBG_UNKNOWN));
	check(test_gettype("&&", PBG_UNKNOWN));
	check(test_gettype("||", PBG_UNKNOWN));
	check(test_gettype("==", PBG_UNKNOWN));
	check(test_gettype("<<", PBG_UNKNOWN));
	check(test_gettype(">>", PBG_UNKNOWN));
	check(test_gettype("??", PBG_UNKNOWN));
	check(test_gettype("!==", PBG_UNKNOWN));
	check(test_gettype("<==", PBG_UNKNOWN));
	check(test_gettype(">==", PBG_UNKNOWN));
	/* pbg_istrue */
	check(test_gettype("TRUE", PBG_LT_TRUE));
	check(test_gettype(" TRUE", PBG_UNKNOWN));
	check(test_gettype(" TRUE ", PBG_UNKNOWN));
	check(test_gettype(" TR UE ", PBG_UNKNOWN));
	check(test_gettype("true", PBG_UNKNOWN));
	check(test_gettype("tRue", PBG_UNKNOWN));
    /* pbg_isfalse */
	check(test_gettype("FALSE", PBG_LT_FALSE));
	check(test_gettype(" FALSE", PBG_UNKNOWN));
	check(test_gettype(" FALSE ", PBG_UNKNOWN));
	check(test_gettype(" FAL SE ", PBG_UNKNOWN));
	check(test_gettype("false", PBG_UNKNOWN));
	check(test_gettype("fAlse", PBG_UNKNOWN));
    /* pbg_isnumber */
	check(test_gettype("3.14e1", PBG_LT_NUMBER));
	check(test_gettype("3", PBG_LT_NUMBER));
	check(test_gettype("03", PBG_UNKNOWN));
	check(test_gettype("3.", PBG_UNKNOWN));
	check(test_gettype("3.0", PBG_LT_NUMBER));
	check(test_gettype(".", PBG_UNKNOWN));
	check(test_gettype("0", PBG_LT_NUMBER));
	check(test_gettype("0.123a45", PBG_UNKNOWN));
	check(test_gettype("0.0.1", PBG_UNKNOWN));
	check(test_gettype("0.0", PBG_LT_NUMBER));
	check(test_gettype("e10", PBG_UNKNOWN));
	check(test_gettype(".0", PBG_UNKNOWN));
	check(test_gettype("0e10", PBG_LT_NUMBER));
	check(test_gettype("0e", PBG_UNKNOWN));
	/* pbg_isstring */
	check(test_gettype("'hi'", PBG_LT_STRING));
	check(test_gettype("''", PBG_LT_STRING));
	check(test_gettype("','", PBG_LT_STRING));
	check(test_gettype("'\''", PBG_LT_STRING));
	check(test_gettype("'unclosed", PBG_UNKNOWN));
	check(test_gettype("unclosed'", PBG_UNKNOWN));
	check(test_gettype("noquotes", PBG_UNKNOWN));
	
	end_test();
}


/**************************
 *                        *
 * UNIT TESTING FUNCTIONS *
 *                        *
 **************************/

int test_gettype(char* str, pbg_node_type expect)
{
	/* Translate given string to type. */
	pbg_node_type output = pbg_gettype(str, strlen(str));
	/* Did we pass?? */
	return (output == expect) ? PBG_TEST_PASS : PBG_TEST_FAIL;
}


int test_parse(pbg_error* err, char* str, int expect)
{
	// TODO implement me!
	return PBG_TEST_PASS;
}


int test_evaluate(pbg_error* err, char* str, pbg_expr_node (*dict)(char*,int), int expect)
{
	pbg_expr e;
	/* Parse the string expression. */
	pbg_parse(&e, err, str, strlen(str));
	/* Return if there's an error. */
	if(err->_type != PBG_ERR_NONE)
		return (expect == -1) ? PBG_TEST_PASS : PBG_TEST_FAIL;
	/* Evaluate the expression with the given dictionary. */
	int output = pbg_evaluate(&e, err, dict);
	/* Clean up. */
	pbg_free(&e);
	/* Did we pass?? */
	return (expect == output) ? PBG_TEST_PASS : PBG_TEST_FAIL;
}
