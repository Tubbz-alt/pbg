#ifndef __PBG_TEST_H__
#define __PBG_TEST_H__

#include "../pbg.h"

/* These are the return values of each test function. */
#define PBG_TEST_PASS 0
#define PBG_TEST_FAIL 1

/* These are the expect values for test_evaluate. */
#define FALSE  0
#define TRUE   1
#define ERROR  2

/**
 * Prints the given error in a human-readable format.
 * @param err  Error to translate.
 */
void pbg_err_print(pbg_error* err);

#define check(test) do { int _res = (test); if(_res != PBG_TEST_PASS) { _numfail++; printf("-failed: %s:%d\n", __FILE__, __LINE__); pbg_err_print(&err); } err._type=PBG_ERR_NONE; pbg_error_free(&err); } while(0)
#define init_test() pbg_error err = (pbg_error) { PBG_ERR_NONE, 0, NULL, 0, NULL }; int _numfail = 0;
#define end_test() if(err._type != PBG_ERR_NONE) pbg_error_free(&err); return _numfail
#define summ_test(name,tester) do { int _numfail = (tester); if(_numfail != 0) printf("%s\tfailed %d tests!\n", (name), _numfail); else printf("%s\tpassed!\n", (name)); } while(0)

/**
 * Tests pbg_gettype.
 * @param str     String expression to parse.
 * @param expect  Expected type of string as represented by pbg_field_type.
 * @return PBG_TEST_PASS if expect == pbg_gettype,
 *         PBG_TEST_FAIL if not.
 */
int test_gettype(char* str, pbg_field_type expect);

/**
 * Tests pbg_evaluate.
 * @param err     Container to store parse & evaluation errors to, if any.
 * @param str     String expression to parse.
 * @param dict    Key resolution dictionary.
 * @param expect  Expected result of evaluation.
 * @return PBG_TEST_PASS if evaluation matches expect,
 *         PBG_TEST_FAIL if not.
 */
int test_evaluate(pbg_error* err, char* str, 
		pbg_field (*dict)(char*,int), int expect);


#endif /* __PBG_TEST_H__ */
