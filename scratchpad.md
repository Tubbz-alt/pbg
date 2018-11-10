goals:
-	small, portable CLI data management tool for personal use. driven by boolean expressions.
-	simple, portable, powerful CLI data management tool for personal use
	-	fast enough, memory conscious enough
	-	highly flexible, portable, self-contained data format
		-	JSON to store transaction data
		-	ability to relatively link attachments
	-	simple, portable, powerful code
		-	we'll use C
			-	widely, widely supported
			-	low-level enough to build standalone executable
			-	fast, memory efficient
			-	slower to develop in, but robust
		-	we'll use jsmn to parse JSON in C
			-	MIT licensed
			-	two files!
			-	very little overhead, no unnecessary copies!


basic functions:
-	create new transactions
-	modify or delete existing transactions
-	generate summary CSVs based on transaction tags

PBN (Prefix Boolean Notation)
-	not whitespace sensitive!
-	uses prefix notation for everything!


select tyler "(|,(>=,[date],2018-10-01),(<,[date],2018-11-01))" | dump
->	dumps all records between Oct 1 and Nov 1, left inclusive.


EXP
	=	(!,EXP)                # Inverts truth value of EXP.
	=	(&,EXP,EXP[,EXP]*)     # Boolean AND of each EXP.
	=	(|,EXP,EXP[,EXP]*)     # Boolean OR of each EXP.
	=	(=,ANY,ANY[,ANY]*)     # TRUE only if all ANY are equal.
	=	(!=,ANY,ANY)           # TRUE only if ANY are not equal.
	=	(<,VALUE,VALUE)        # TRUE only if left VALUE is less than right VALUE.
	=	(>,VALUE,VALUE)        # TRUE only if left VALUE is greater than right VALUE.
	=	(<=,VALUE,VALUE)       # TRUE only if left VALUE is less than or equal to right VALUE.
	=	(>=,VALUE,VALUE)       # TRUE only if left VALUE is greater than or equal to right VALUE.
	=	(?,[KEY])              # TRUE only if KEY is a defined column.
	=	TRUE                   # The TRUE expression.
	=	FALSE                  # The FALSE expression.

ANY
	=	VALUE
	=	STRING

VALUE
	=	NUMBER
	=	DATE

DATE
	=	YYYY
	=	YYYY-MM
	=	YYYY-MM-DD
	=	YYYY-MM-DD@hh:mm
	=	YYYY-MM-DD@hh:mm:ss
	=	[KEY]

NUMBER
	=	[+/-]((0/(1-9))[0-9]*)[.(0-9)[0-9]*][(e/E)[+/-](0-9)[0-9]*]
	=	[KEY]

STRING
	=	'.*'
	=	[KEY]

[KEY]
	=	Resolved via column dictionary lookup.


data types:
	NUMBER
	STRING
	DATE

select(db, boolexp) = [record]
	db       name of database to search
	boolexp  a boolean expression built with values, operators, and joins
	(values)
		*name      name of pair in record
		STRING     a string literal
		NUMBER     a number literal
		DATE       a date literal
	(op)
		a=b     a equals b                  NUMBER, DATE, and STRING
		a<b     a less than b               NUMBER or DATE
		k?      name k exists in record
	(join)
		A&B    boolexp A AND boolexp B
		A|B    boolexp A OR boolexp B
		()      grouping
		!       negation
	record	absolute path to a record with matching (name,value) pairs

dump([record], [name]) = file
	record	absolute path to a record
	name	name of a pair
	file	formatted CSV matrix with name columns and a row for each record

summarize(db) = output
	db   	name of database to search
	output	list of key names used throughout database and number of occurrences
