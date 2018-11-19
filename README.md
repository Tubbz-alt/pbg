# pbg
A simple grammar for writing boolean expressions implemented in a small, portable C library.

[about](#about) | [example](#example) | [design goals](#design-goals) | [formal definition](#formal-definition) | [API](#API)


## about

**pbg**, an initialism for Prefix Boolean Grammar, is a lightweight grammar for writing boolean expressions.

The following is a PBG expression:
```
(& (< [start] [end]) (= [start] 2018-10-12))
```
This expression checks if the variable `start` is less than the variable `end` and if `start` is a date equal to October 12, 2018. It evaluates to `TRUE` only if both conditions are `TRUE`.

PBG is designed to be used as a module within [**tbd**, the Tiny Boolean DBMS](https://github.com/imtjd/tbd).


## example

The below example is found in `test/example.c`. It evaluates the expression `(&(=[a][b])(?[d]))` with `a=5.0`, `b=5.0`, and `c=6.0`. 
```C
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
```
The output is `FALSE` because `(?[d])` asks if the variable `d` is defined, which it is not. The expression `(|(=[a][b])(?[d]))` is `TRUE`, however, because `(=[a][b])` asks if `a` and `b` are equal, which they are.


## design goals

pbg is built with three goals in mind.

First, it must be **simple**. It shouldn't be bogged down by too many redundant features, it should be easy to express thoughts with, and it must be easy to write a compiler for.

Second, it must be **unambigous**. Any expression string must have an unambiguous truth value. Operator precedence invites bugs for the sake of better readability. This seems like a bad idea, so it is avoided.

Third, it must be **expressive**. Thoughts should be easily translated into concise expressions. The grammar falls short of Turing completeness for the sake of simplicity, but it can still go a long way. 


## formal definition

A PBG expression is represented by an abstract syntax tree (AST).

Each node in the tree is represented by a **field**. A field can be either an **operator** or a **literal**. Operators are always internal nodes of the AST; fields, always leaves.

Each field has a **type**. The type of an operator determines its function as well as the types and multiplicity of its children (read: arguments) it can have. The type of a literal determines how the data within the field is interpreted and which operators it can be an argument of.

### literals

Literals are typed. The following types are supported: `DATE`, `BOOL`, `NUMBER`, `STRING`, and `NULL`. 

##### DATE

A `DATE` literal represents a day in the Gregorian calendar and is formatted as `YYYY-MM-DD`. So `2018-10-12` is `October 12, 2018`.

##### BOOL

A `BOOL` literal can be either `TRUE` or `FALSE`.

##### NUMBER

A `NUMBER` literal represents a floating point number and is formatted in the same way a [*JSON* number](http://json.org/). So `3`, `3.14`, `314e-2`, `0.314`, and `0.0` are all valid `NUMBER` literals, but `.314`, `0.`, and `1e` are not.

##### STRING

A `STRING` literal represents text, i.e. a string of characters, and is formatted using single quotes. So `'Hello pbg!'` is a `STRING`, and `''` is the empty `STRING`. To represent a single quote within a `STRING`, it must be escaped: `'I said, \'hi\' before I left.'`.

##### NULL

A `NULL` literal represents... nothing. It is written as `NULL`. If a variable is not defined, it is considered `NULL`.

### opertions

**pbg** supports a small set of essential operations. Every operation should be thought of as a function that accepts arguments of various types and returns a `BOOL`.

##### NOT

The `NOT` operator is written as `!`. It takes a single input of type `BOOL` and inverts it. This is a standard boolean operation.

##### AND

The `AND` operator is written as `&`. It takes two or more inputs of type `BOOL` and returns the boolean and of them. So it returns `TRUE` only if every argument is `TRUE`.

##### OR

The `OR` operator is written as `|`. It takes two or more inputs of type `BOOL` and returns the boolean or of them. So it returns `TRUE` only if at least one of its arguments is `TRUE`.

##### EQ

The `EQ` operator is written as `=`. It takes two or more inputs of any type and returns `TRUE` only if all arguments have equal values.

##### NEQ

The `NEQ` operator is written as `!=`. It takes two inputs of any type and returns `TRUE` only if its arguments are different.

##### LT

The `LT` operator is written as `<`. It takes two inputs of any type and returns `TRUE` only if the first argument is less than the second.

##### GT

The `GT` operator is written as `>`. It takes two inputs of any type and returns `TRUE` only if the first argument is greater than the second.

##### LTE

The `LTE` operator is written as `<=`. It takes two inputs of any type and returns `TRUE` only if the first argument is less than or equal to the second.

##### GTE

The `GTE` operator is written as `>=`. It takes two inputs of any type and returns `TRUE` only if the first argument is greater than or equal to the second.

##### EXST

The `EXST` operator is written as `?`. It takes one input of any type including `NULL` and returns `TRUE` only if it its argument is not `NULL`. `EXST` is useful for checking if a variable is defined.

##### TYPE

The `TYPE` operator is written as `@`. Its first argument is a type literal, and every argument thereafter may be of any type including `NULL`. It returns `TRUE` only if every argument after the first has the type specified by the first. For example, `(@ NUMBER 3.14 'hi' 17)` returns `FALSE` because `'hi'` is not a `NUMBER` even though `3.14` and `17` are. Notice that `(?[a])` and `(@ NULL [a])` have the same truth table.


### the grammar
The following is the grammar generates expression strings in **pgb**:
```
EXPR
  = (! BOOL)
  = (& BOOL BOOL ...)
  = (| BOOL BOOL ...)
  = (= ANY ANY ...)
  = (!= ANY ANY)
  = (< ANY ANY)
  = (> ANY ANY)
  = (<= ANY ANY)
  = (>= ANY ANY)
  = (? ALL)
  = (@ TYPE ALL ...)
  = (BOOL)
ALL
  = ANY
  = NULL
ANY
  = DATE
  = NUMBER
  = STRING
  = BOOL
BOOL
  = TRUE
  = FALSE
```


## API

This repository provides a lightweight implementation of a PBG compiler. It can be incorporated into an existing project by including `pbg.h`. Documentation of each API function is provided in `pbg.h` but is partially reproduced here for visibility. The library reserves the `pbg_` and `PBG_` prefixes.

### functions

```C
/* Parse the string as a PBG expression. If a compilation error occurs, initialize 
 * the provided error argument accordingly. */
void pbg_parse(pbg_expr* e, pbg_error* err, char* str, int n)
```

```C
/* Destroy the PBG expression instance, and free all associated resources. If 
 *`pbg_parse` succeeds, this function must be called to free up internal resources. */
void pbg_free(pbg_expr* e)
```

```C
/* Evaluate the PBG expression with the provided dictionary. If a runtime error 
 * occurs, initialize the provided error argument accordingly. */
int pbg_evaluate(pbg_expr* e, pbg_error* err, pbg_expr_node (*dict)(char*, int))
```

```C
/* Make a field representing the given type. Initialize everything other than 
 * the type to zero. This is useful for creating a `TRUE`, `FALSE`, or `UNKNOWN` field. */
pbg_field pbg_make_field(pbg_field_type type)
```

```C
/* Parse `str` as a `DATE` literal, and return a field representing it. */
pbg_field pbg_make_date(pbg_error* err, char* str, int n)
```

```C
/* Parse `str` as a `NUMBER` literal, and return a field representing it. */
pbg_field pbg_make_number(pbg_error* err, char* str, int n)
```

```C
/* Parse `str` as a `STRING` literal, and return a field representing it. */
pbg_field pbg_make_string(pbg_error* err, char* str, int n)
```

```C
/* Identifies the PBG expression type of the given string. */
pbg_node_type pbg_gettype(char* str, int n)
```

```C
/* Prints a human-readable representation of the given error. */
void pbg_error_print(pbg_error* err)
```

```C
/* Frees resources being used by the given error, if any. */
void pbg_error_free(pbg_error* e)
```
