# pbg
A simple grammar for writing boolean expressions implemented in a small, portable C library.

[about](#about) | [philosophy](#philosophy) | [features](#features) | [design](#design) | [API & example](#API)


## about

**pbg**, an initialism for Prefix Boolean Grammar, is a lightweight grammar for writing boolean expressions.

The following is a PBG expression:
```
(& (< [start] [end]) (= [start] 2018-10-12))
```
This expression checks if the variable `start` is less than the variable `end` and if `start` is a date equal to October 12, 2018. It evaluates to `TRUE` only if both conditions are `TRUE`.

PBG is designed to be used as a module within [**tbd**, the Tiny Boolean DBMS](https://github.com/imtjd/tbd); however, feel free to use it wherever a use can be found.


## philosophy

pbg (the grammar) is designed to be **simple** (simple syntax with a minimal set of features), **readable** (easy to read and write expressions), and **expressive** (expressions pack the full punch of propositional logic).

pbg (the C library) is designed to be **fast** (quick compilation and evaluation), **robust** (faithful implementation of the grammar, full error reporting on failure), and **simple** (few API functions, flexible dictionary implementation).


## features

- simple, powerful grammar
- fast compilation and evaluation
- compatible with C99
- no dependencies except libc
- small memory footprint


## design

All valid pbg expressions can be generated with the following grammar:
```
EXPR
  = BOOL
BOOL
  = (! BOOL)
  = (& BOOL BOOL ...)
  = (| BOOL BOOL ...)
  = (= ANY ANY ...)
  = (!= ANY ANY)
  = (< ANY ANY)
  = (> ANY ANY)
  = (<= ANY ANY)
  = (>= ANY ANY)
  = (? ANY)
  = (@ TYPE ANY ...)
  = TRUE
  = FALSE
ANY
  = DATE
  = NUMBER
  = STRING
  = BOOL
  = NULL
```

This grammar is defined recursively, so a pbg expression can be elegantly represented with an abstract syntax tree (AST).

Each node in the tree is represented by a **field**. A field can be either an **operator** or a **literal**. Operators are always internal nodes of the AST; literals, always leaves.

Each field has a **type**. The type of an operator determines its function as well as the types and multiplicity of its children (read: inputs). The type of a literal determines how the data within the field is interpreted and which operators it can be an input of.

### literals

Literals are typed. The following types are supported: `DATE`, `BOOL`, `NUMBER`, `STRING`, and `NULL`. There also exist meta-literals called **type literals**. Instead of values having types, these reference the types themselves and are used by the `TYPE` operator.

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

**pbg** supports a conservative set of essential operations. Every operation should be thought of as a function that accepts inputs of various types and returns a `BOOL`.

##### NOT `(! BOOL)`

The boolean not operator, abbreviated `NOT`. Take a single input of type `BOOL`. Return the inverse of its input.
+ `(! TRUE)` is `FALSE`
+ `(! FALSE)` is `TRUE`

##### AND `(& BOOL BOOL ...)`

The boolean and operator, abbreviated `AND`. Take two or more inputs of type `BOOL`. Return `TRUE` only if every input is `TRUE`.
+ `(& TRUE FALSE)` is `FALSE`
+ `(& TRUE TRUE TRUE)` is `TRUE`

##### OR `(| BOOL BOOL ...)`

The boolean or operator, abbreviated `OR`. Take two or more inputs of type `BOOL`. Return `FALSE` only if every input is `FALSE`.
+ `(| FALSE TRUE FALSE)` is `TRUE`
+ `(| FALSE FALSE)` is `FALSE`

##### EQ `(= ANY ANY ...)`

The equality operator, abbreviated `EQ`. Take two or more inputs of any type. Return `TRUE` only if all arguments have equal values.
+ `(= 3 'hi')` is `FALSE`
+ `(= 3 '3')'` is `FALSE`
+ `(= 3 3)` is `TRUE`

##### NEQ `(!= ANY ANY)`

The not equal operator, abbreviated `NEQ`. Take two inputs of any type. Return `TRUE` only if its arguments are different.
+ `(!= 3 'hi')` is `TRUE`
+ `(!= 3 '3')'` is `TRUE`
+ `(!= 3 3)` is `FALSE`

##### LT `(< ANY ANY)`

The less than operator, abbreviated `LT`. Take two inputs of any type. Return `TRUE` only if the first argument is less than the second.
+ `(< 'aa' 'ab')` is `TRUE`
+ `(< 2018-10-12 2018-10-11)` is `TRUE`
+ `(< 5 2)` is `FALSE`

##### GT `(> ANY ANY)`

The greater than operator, abbreviated `GT`. Take two inputs of any type. Return `TRUE` only if the first argument is greater than the second.
+ `(> 'aa' 'ab')` is `FALSE`
+ `(> 2020-12-01 2019-12-01)` is `TRUE`
+ `(> 5 2)` is `TRUE`

##### At Most `(<= ANY ANY)`

The at most operator, abbreviated `LTE`. Take two inputs of any type. Return `TRUE` only if the first argument is less than or equal to the second.
+ `(<= 'aa' 'aa')` is `TRUE`
+ `(<= 2020-12-01 2019-12-01)` is `FALSE`
+ `(<= 5 6)` is `TRUE`

##### At Least `(>= ANY ANY)`

The at least operator, abbreviated `GTE`. Take two inputs of any type. Return `TRUE` only if the first argument is greater than or equal to the second.
+ `(>= 'aa' 'zz')` is `FALSE`
+ `(>= 2020-12-01 2019-12-01)` is `TRUE`
+ `(>= 7 7)` is `TRUE`

##### Existence `(? ALL ...)`

The existence operator, abbreviated `EXST`. Take one input of any type. Return `TRUE` only if it its argument is not `NULL`. `EXST` is useful for checking if a variable is defined.
+ `(? 5)` is `TRUE`
+ `(? 2018-10-12)` is `TRUE`
+ `(? NULL)` is `FALSE`
+ `(? [a])` is `TRUE` only if variable `a` is defined.

##### Type Check `(@ TYPE ALL ...)`

The type check operator is written as `@`. Take a `TYPE` name followed by at least one input. Return `TRUE` only if every argument after the first has the type specified by the first. 
+ Notice that `(?[a])` and `(@ NULL [a])` have the same truth table.
+ `(@ NUMBER 3.14 'hi' 17)` is `FALSE`, because `'hi'` is not a `NUMBER` even though `3.14` and `17` are. 
+ `(@ DATE 2018-10-12)` is `TRUE`, because `2018-10-12` is a `DATE` literal.
+ `(@ DATE '2018-10-12')` is `FALSE`, because `'2018-10-12'` is a `STRING` literal.


## API

This repository provides a lightweight implementation of a PBG compiler and evaluator. It can be incorporated into an existing project by including `pbg.h`. Documentation of each API function is provided in `pbg.h` but is partially reproduced here for visibility. The library reserves the `pbg_` and `PBG_` prefixes.

### literals & operators

Both literal and operator types, as represented within the library, are enums with their type name prefixed by either `PBG_LT_` or `PBG_OP_`, respectively. So the `NUMBER` literal is referred to as `PBG_LT_NUMBER`, and the `OR` operator is referred to as `PBG_OP_OR`. Operators are named according to the abbreviations listed [above](#formal-defintiion).

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
 * the type to zero. This is useful for creating a `TRUE`, `FALSE`, or `NULL` field. */
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

### example

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
