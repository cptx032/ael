# ael
Ael is a simple experimental ultra-minimalistic1 script language. Its made for my games scriptings.
Ael is not for fast processing because its **slow** in **for** operations (but you
can write an optimized version of **for** command as well :) ).

Ael is a minimized version of Tcl's syntax, but in implementation is very different.

## compilation
```bash
make
```

## Example
```bash
set 10 \n.
set my_function {
	trace Hello World! \n.
}.
run my_function.

set __ael_default_function set.
name John.
trace name.
```

Ael has not **functions**, instead you can set a variable with ael code and interprets it in
execution-time using the **run** command, like **exec** function in Python.

## Syntax

A word is just a non-blank string of characters or a string of characters inside brackets.
So **"price"** is one "word", **"{john merric}"** is one "word", but **"john meric"** are two
words. A "phrase" is a set of words with a "." (dot) character in end. So, all that syntax is summarized
as follows:

```<command> arg1 arg2 (...).```

For example:
```trace Hello World!.``` is interpreted like:

    command: trace
    arg1: Hello
    arg2: World!


Note that "World!" is a valid "word" even using the "!" character. Any non-blank character is valid.

### Commands

Ael has "commands" that are implemented in c++ and is available in ael interpreter. The "command" is the
first "word/token" of a "phrase". When ael does not find the command in the command dictionary it call the
function stored in the **__ael_default_function** variable. So, take this code:

```bash
john mary jane.
```

If ael interpreter does not find "john" command it will executes:

```bash
<__ael_default_function> john mary jane.
```

This variable, by default, is set to **run**. So you can do things like:

```
set my_function {trace ok.}.
my_function.
```

That is the same of:

```bash
set my_function {trace ok.}.
run my_function.
```

Inside brackets you can have **any** character, even new lines. This is useful to create "functions" as
you already noticed:

```bash
set func {
	trace ok.
}.
```

Ael has not "comments", instead it have a command for that. The command is **"#"**, thats why you
should include a dot in end of "commentaries", taking care to not put any dot in the middle:
```bash
# this is a valid commentary.
# this is not a valid commentary. because it have a dot inside.
# {but this is a valid commentary. because the commentary have only one argument even with a dot inside}.
# {
	Like any command you can use
			multiple lines
}.
```

## Details of C++ implementation
All commands are stored in a hashtable pointing to a function pointer. All "normal" variable are stored
in a hashtable too, like std::strings.
The process of interpretation basicaly is split a string in tokens/words calling the functions in
function hashtable.

## Embedding
Is simple like that:

```c++
#include <ael/ael.h>
int main() {
	aelinterpreter ael_interpreter
	// loads all default commands:
	load_main_ael_functions(ael_interpreter);
	std::string my_ael_code = "...";
	phrase ael_phrase;
	// stores in "phrase" the words
	ael_interpreter.to_tokens(ael_phrase);
	// interprets the phrase
	ael_interpreter.interprets(ael_phrase);
	return 0;
}
```

## Implementations
Currently Ael have interpreters in:
+ c++
+ python
+ java
+ javascript

But the "main" interpreter is writen in c++. The others languages are not well documented still :(1

So you can create an API and make it available to AEL, and write code once.

## Performance
For simple scripts Ael is more fast than many languages, being slower than c++ only. But complex scripts
with many conditionals and loops Ael gets very slow, so its not ideal to math/physics processing or things like that.

Use ael at your own risk. Enjoy! :)
