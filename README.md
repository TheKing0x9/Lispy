# Lispy

Lispy is a tiny Lisp like language written in less than 1000 semicolons.

## Features

At its current stage, Lispy features : 

- Single Line Comments
- Integer Arithmetic
- S Expressions
- Q Expressions
- Lists
- Conditions
- Variables
- Functions
- Lambdas
- Basic String support

## Syntax

### Single Line Comments

Lispy supports single lined comments starting with a hash (```#```)

```
# This is a single lined comment
```

### Arithmetic : 

Lispy supports addition, subtraction, multiplication, division and modulus operations
in the Prefix (Polish) notation.
All numeric values are represented as long integers currently.

```
+ 5 5      # 10
- 10 2     # 8
* 6 7      # 42
/ 4 1      # 4
% 6 4      # 2
```

### S-Expressions : 
  
S Expressions are special expressions that are evaluated as soon as they are encountered.

```
(+ 5 5 ( / 4 5))
```

### Q-Expressions

Q Expressions are yet another special expressions that are kept in the memory as is
without any evaluation. They are used to define variables, functions and represent lists.

```
# This expression won't be evaluated 
{ + {+ 1 2} 2}
```

### Lists

Lispy supports Lists using Q Expressions

```
{1 2 3}        # a simple list
{1 3 { 1 3 3}} # a compound list
```

#### List functions

- Head : Returns a list containing the first element of the list.
  ```
  head { 1 2 3 } # returns {1}
  ```
  
- Tail : Returns a new list with first element removed
  ```
  tail { 1 2 3 } # returns { 2 3 }
  ```
  
- Eval : Evaluates a list as if it were an S Expression
  ```
  eval { + 1 2 3 } # returns 6
  ```
  
### Conditionals

Lispy supports the tradional if statement using Q-Expressions. It accepts three 
arguments : The condition, true statement and the false statement. 
Lispy currently does not have support for boolean values and rather uses the C Style 0
for false and 1 for true

```
if ( >= 1 2 ) { print "True" } { print "False" }
```
  
### Variables 

Variables are an integral part of Lispy. Almost everything you can define can be assigned
to a variable, functions, other variables, Lists, Numbers and Strings

```
def { x }    100            #
def { name } "myname"
def { foo }  { 1 2 3 }
def { bar }  foo
```

Variables can be declared using two syntaxes

#### Local Definition

This limits the scope of the function to just the current block it is in. These variables
are defined using the syntax 

```
def { x } 100
```

#### Global Definition

This declaration initialzes the variable at the global scope

```
 = { x } 100
```

#### Changing variable values

In Lispy, values of a variable can be mutatted by just overwriting the old value of the
variable. 

> *Note* : This is currently under consideration and may change in the near future

### Functions and Lambdas  

Lispy has a unique and fun syntax for declaring functions. The function name, arguments
and body are written using Q-Expression, which are wrapped around an S Expression.

The functions return whatever result it's body produces.

```
def { inc } (\{ x } { + x 1 }) 

inc 1 # returns 2
```

This unique representation of functions allows Lispy to do an amazing thing - assign
functions to variables. This statement is perfectly acceptable in Lispy

```
def { x } inc 

x 1 # prints 2
```

## Coming Soon

Lispy will soon be updated with more cool features such as

- Double Precision floating point numbers
- A Standard Library
- File Interface Functions
- String Manipulation Functions
- Performance improvements

## Credits

Lispy was made based on [Buid Your Own Lisp](http://www.buildyourownlisp.com).
A huge thanks to the author of the book for making such an amazing book available
for free :heart:
