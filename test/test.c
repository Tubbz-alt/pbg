#include "../pbg.h"
#include "test.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Test suites in this file. */
pbg_field dict(char* key, int n);
int suite_evaluate(void);
int suite_gettype(void);

/* Run and summarize test suites. */
int main(void)
{
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
pbg_field dict(char* key, int n)
{
	pbg_field keylt;
	PBG_UNUSED(n);
	keylt._type = PBG_NULL;
	keylt._int = 0;
	keylt._data = NULL;
	if(key[0] == 'a' || key[0] == 'b' || key[0] == '1') {
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
	check(test_evaluate(&err, "TRUE", dict, PBG_TRUE));
	check(test_evaluate(&err, "  TRUE", dict, PBG_TRUE));
	check(test_evaluate(&err, "TRUE  ", dict, PBG_TRUE));
	check(test_evaluate(&err, "((TRUE))", dict, PBG_ERROR));
	check(test_evaluate(&err, "(TRUE)", dict, PBG_ERROR));
	check(test_evaluate(&err, "(TRUE)", dict, PBG_ERROR));
	check(test_evaluate(&err, "( TRUE)", dict, PBG_ERROR));
	check(test_evaluate(&err, "(TRUE )", dict, PBG_ERROR));
	check(test_evaluate(&err, "( TRUE )", dict, PBG_ERROR));
	check(test_evaluate(&err, "(TRU)", dict, PBG_ERROR));
	/* FALSE */
	check(test_evaluate(&err, "FALSE", dict, PBG_FALSE));
	check(test_evaluate(&err, "  FALSE", dict, PBG_FALSE));
	check(test_evaluate(&err, "FALSE  ", dict, PBG_FALSE));
	check(test_evaluate(&err, "(FALSE)", dict, PBG_ERROR));
	check(test_evaluate(&err, "( FALSE)", dict, PBG_ERROR));
	check(test_evaluate(&err, "(FALSE )", dict, PBG_ERROR));
	check(test_evaluate(&err, "( FALSE )", dict, PBG_ERROR));
	check(test_evaluate(&err, "(FALS)", dict, PBG_ERROR));
	/* NOT */
	check(test_evaluate(&err, "(! FALSE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(! TRUE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(! (= 10 10))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(! (= 10 10))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(! (= 9 10))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(! (! (= 9 10)))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(! [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(! [0])", dict, PBG_ERROR));
	/* AND */
	check(test_evaluate(&err, "(& TRUE TRUE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(& TRUE FALSE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(& FALSE TRUE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(& FALSE FALSE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(& TRUE TRUE TRUE TRUE TRUE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(& TRUE TRUE TRUE FALSE TRUE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(& (& FALSE TRUE) TRUE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(& [1] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(& [0] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(& [1] [0])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(& [0] [0])", dict, PBG_ERROR));
	/* OR */
	check(test_evaluate(&err, "(| TRUE TRUE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(| TRUE TRUE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(| TRUE FALSE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(| FALSE TRUE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(| FALSE FALSE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(| FALSE FALSE FALSE FALSE FALSE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(| FALSE TRUE FALSE FALSE FALSE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(| [1] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(| [0] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(| [1] [0])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(| [0] [0])", dict, PBG_ERROR));
	/* EXST */
	check(test_evaluate(&err, "(? [c])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(? [d])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(? [c] [c])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(? [a] [c] [c])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(? [a] [c] [d])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(? [a] [d] [c])", dict, PBG_FALSE));
	/* EQUAL */
	check(test_evaluate(&err, "(= 10 10)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= 10 10 10 10 10)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= 10 10 10 9 10)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 9 10)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 10 9)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 'hi' 9)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= -10 'hi')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= -10 -10)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= 'hi' 'hi')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= 'a, b\\'c' 'a, b\\'c')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= 'ab\\'c' 'ab\\'d')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 'hia' 'hi')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 'hi' 'h ')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 2018-10-12 'h ')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 2018-10-12 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= 2018-10-12 2018-10-13)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 2018-10-13 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 2018-10-13 2017-10-13)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 2018-10-13 2018-11-13)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= [a] [a])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= [a] [b])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= [a] [c])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= [c] [b])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= [c] [b)", dict, PBG_ERROR));
	check(test_evaluate(&err, "(= [c [b])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(= TRUE TRUE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= TRUE FALSE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= FALSE TRUE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= FALSE FALSE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= [1] [1])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= [1] [0])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(= [0] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(= [0] [0])", dict, PBG_ERROR));
	/* NEQ */
	check(test_evaluate(&err, "(!= 10 10)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= 9 10)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= 10 9)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= 'hi' 9)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= -10 'hi')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= -10 -10)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= 'hi' 'hi')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= 'a,b\\'c' 'a,b\\'c')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= 'ab\\'c' 'ab\\'d')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= 'hia' 'hi')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= 'hi' 'h ')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= 2018-10-12 'h ')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= 2018-10-12 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= 2018-10-12 2018-10-13)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= 2018-10-13 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= 2018-10-13 2017-10-13)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= 2018-10-13 2018-11-13)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= [a] [a])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= [a] [b])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= [a] [c])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= [c] [b])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= TRUE TRUE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= TRUE FALSE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= FALSE TRUE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= FALSE FALSE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= [1] [1])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= [1] [0])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(!= [0] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(!= [0] [0])", dict, PBG_ERROR));
	/* LESS THAN */
	check(test_evaluate(&err, "(< 2 3)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(< 3 3)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< -3 3)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(< 'a' 'a')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< 'a' 'b')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(< 'b' 'a')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< 'aaa' 'aab')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(< 2018-10-12 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< 2018-10-11 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(< 2018-10-12 2018-10-11)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< 2018-09-12 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(< 2018-09-12 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(< 2018-10-12 2018-09-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< 2017-10-12 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(< 2018-10-12 2017-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< [1] [1])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< [1] [0])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(< [0] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(< [0] [0])", dict, PBG_ERROR));
	/* GREATER THAN */
	check(test_evaluate(&err, "(> 3 2)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(> 3 3)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> 3 -3)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(> 'a' 'a')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> 'a' 'b')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> 'b' 'a')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(> 'aaa' 'aab')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> 2018-10-12 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> 2018-10-11 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> 2018-10-12 2018-10-11)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(> 2018-09-12 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> 2018-10-12 2018-09-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(> 2017-10-12 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> 2018-10-12 2017-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(> [1] [1])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> [1] [0])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(> [0] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(> [0] [0])", dict, PBG_ERROR));
	/* LESS THAN OR EQUAL */
	check(test_evaluate(&err, "(<= 2 3)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= 3 3)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= -3 3)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= 3 -3)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(<= 'a' 'a')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= 'a' 'b')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= 'b' 'a')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(<= 'aaa' 'aab')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= 2018-10-12 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= 2018-10-11 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= 2018-10-12 2018-10-11)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(<= 2018-09-12 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= 2018-10-12 2018-09-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(<= 2017-10-12 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= 2018-10-12 2017-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(<= [1] [1])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= [1] [0])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(<= [0] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(<= [0] [0])", dict, PBG_ERROR));
	/* GREATER THAN OR EQUAL */
	check(test_evaluate(&err, "(>= 3 2)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= 3 3)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= 3 -3)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= -3 3)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(>= 'a' 'a')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= 'a' 'b')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(>= 'b' 'a')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= 'aaa' 'aab')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(>= 2018-10-12 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= 2018-10-11 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(>= 2018-10-12 2018-10-11)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= 2018-09-12 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(>= 2018-10-12 2018-09-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= 2017-10-12 2018-10-12)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(>= 2018-10-12 2017-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= [1] [1])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= [1] [0])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(>= [0] [1])", dict, PBG_ERROR));
	check(test_evaluate(&err, "(>= [0] [0])", dict, PBG_ERROR));
	/* TYPE */
	check(test_evaluate(&err, "(@ DATE 2018-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ DATE [a])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(@ DATE 2018-10-12 2017-10-12)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ DATE 2018-10-12 10)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(@ NUMBER 10)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ NUMBER [a])", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ NUMBER 10 12 13)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ NUMBER 10 12 'hi' 13)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(@ STRING 'hi')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ STRING [a])", dict, PBG_FALSE));
	check(test_evaluate(&err, "(@ STRING 'hi' 'a' 'b')", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ STRING 'a' 12 'hi' 'b')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(@ BOOL TRUE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ BOOL FALSE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ BOOL TRUE FALSE TRUE TRUE FALSE)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ BOOL TRUE FALSE 10 TRUE FALSE)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(@ BOOL 10)", dict, PBG_FALSE));
	check(test_evaluate(&err, "(@ BOOL 'hi')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(@ BOOL (& 10 10))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@ BOOL (& 10 10) 'hi' TRUE (! FALSE))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(@ BOOL (! FALSE) (? [a]) (& FALSE FALSE) (& FALSE TRUE))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(@)", dict, PBG_ERROR));
	check(test_evaluate(&err, "(@ 10 10)", dict, PBG_ERROR));
	
	/* Whitespace insensitivity. */
	check(test_evaluate(&err, "(>=  3  2)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(=  2  2 )", dict, PBG_TRUE));
	check(test_evaluate(&err, "( = 2 2)", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= ' hi ' ' hi')", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= 2 2)  ", dict, PBG_TRUE));
	check(test_evaluate(&err, "(=\n\t2\n\t2\n)  ", dict, PBG_TRUE));
	check(test_evaluate(&err, "    (   =    	2     2)  ", dict, PBG_TRUE));
	
	/* Stress test the syntax. */
	check(test_evaluate(&err, "(>= 'hi' 2)", dict, PBG_ERROR));
	check(test_evaluate(&err, "(>= 2 'hi')", dict, PBG_ERROR));
	check(test_evaluate(&err, "(= 'a' 97)", dict, PBG_FALSE));
	check(test_evaluate(&err, "()", dict, PBG_ERROR));
	check(test_evaluate(&err, "(=)", dict, PBG_ERROR));
	check(test_evaluate(&err, "(?)", dict, PBG_ERROR));
	check(test_evaluate(&err, "(= 'hi' 'hi)", dict, PBG_ERROR));
	check(test_evaluate(&err, "'test'", dict, PBG_ERROR));
	check(test_evaluate(&err, "'", dict, PBG_ERROR));
	check(test_evaluate(&err, "(! ()", dict, PBG_ERROR));
	check(test_evaluate(&err, ")(! PBG_TRUE)", dict, PBG_ERROR));
	check(test_evaluate(&err, "((?))", dict, PBG_ERROR));
	check(test_evaluate(&err, "(!(= 10 10))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(&(= 10 10)(= 20 20))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(&(?[a])(?[b]))", dict, PBG_TRUE));
	
	/* Ensure operators work as expected with subexpressions. */
	check(test_evaluate(&err, "(< (?[1])(?[1]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< (?[0])(?[1]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(< (?[1])(?[0]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(< (?[0])(?[0]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(<= (?[1])(?[1]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= (?[0])(?[1]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(<= (?[1])(?[0]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(<= (?[0])(?[0]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(> (?[1])(?[1]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> (?[0])(?[1]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(> (?[1])(?[0]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(> (?[0])(?[0]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(>= (?[1])(?[1]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= (?[0])(?[1]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(>= (?[1])(?[0]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(>= (?[0])(?[0]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= (?[1])(?[1]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(= (?[0])(?[1]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= (?[1])(?[0]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(= (?[0])(?[0]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= (?[1])(?[1]))", dict, PBG_FALSE));
	check(test_evaluate(&err, "(!= (?[0])(?[1]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= (?[1])(?[0]))", dict, PBG_TRUE));
	check(test_evaluate(&err, "(!= (?[0])(?[0]))", dict, PBG_FALSE));
	
	end_test();
}


/**************************
 *                        *
 * UNIT TESTING FUNCTIONS *
 *                        *
 **************************/

int test_evaluate(pbg_error* err, char* str, pbg_field (*dict)(char*,int), int expect)
{
	pbg_expr e;
	int output;
	/* Parse the string expression. */
	pbg_parse(&e, err, str);
	/* Return if there's an error. */
	if(err->_type != PBG_ERR_NONE)
		return (expect == PBG_ERROR) ? PBG_TEST_PASS : PBG_TEST_FAIL;
	/* Evaluate the expression with the given dictionary. */
	output = pbg_evaluate(&e, err, dict);
	/* Clean up. */
	pbg_free(&e);
	/* Return if there's an error. */
	if(err->_type != PBG_ERR_NONE)
		return (expect == PBG_ERROR) ? PBG_TEST_PASS : PBG_TEST_FAIL;
	/* Did we pass?? */
	return (expect == output) ? PBG_TEST_PASS : PBG_TEST_FAIL;
}


void pbg_err_print(pbg_error* err)
{
	if(err->_type != PBG_ERR_NONE) {
		printf("---");
		pbg_error_print(err);
	}
}
