lpcompiler
==========

Compiler for a toy language called L+.

This is currently a GitHub snapshot of the project hosted somewhere else.

The L+ Compiler (lpc)
=====================

This project was developed by Emilio Pombo and I (Alejandro Segovia) as a mandatory assignment in 2006. The assignment was part of a Compilers course at the Catholic University of Uruguay and consisted in building a compiler for a C-like programming language from scratch.

We believe this project could be useful to anyone attending a compilers course, interested in compilers or just curious about how a compiler might work internally.

The language our compiler takes as input, called L+, supports most constructs from C (variables, branches, loops, functions, recursion and I/O) albeit with some limitations. The language does sport some additions instead. The L+ compiler generates Java Bytecode. L+ programs can be run on any platform with a Java VM installed.

This program was developed a long time ago, and this was my second largest C++ program at the time. Consequently, the code should not be considered good C++ programming style.

This program is Free Software, licensed under the GPLv3. Feel free to download the code, explore how it works, tweak it and redistribute it to your friends.

The following section presents some examples of the L+ programming language in action. We also have a Programming Guide available in PDF format.

L+ Language
===========

Hello World:

L+ should look familiar to regular users of C-like languages. Here's the mandatory "hello world" example in L+:

```
// hello.lp
void main()
{
        print("Hello, World!");
}
```

Fibonacci:

Here's a more elaborate example that reads a number from stdin and calculates it's corresponding Fibonacci number. This example illustrates some core capabilities of L+, including I/O, functions, recursion, branching and string processing:

```
// Calculate the Fibonacci number for n
int fibo(int n)
{
        if (n == 0)
        {
                return 0;
        }

        if (n == 1)
        {
                return 1;
        }

        return fibo(n-1) + fibo(n-2);
}

// Program fibo.lp
// Read a number, calculate the Fibonacci number for said number
// and print a message with the result.
void main()
{
        print("Enter number to calculate the Fibonacci number for:");
        string nStr = read();

        int f = fibo(toInt(nStr));
        string s = toString(f);

        print("The Fibonacci number for " + nStr + " is: " + s);
}
```

Running this program with an input value of "10" would produce the following output:

```
Enter number to calculate the Fibonacci number for:
10
The Fibonacci number for 10 is: 55.0
```

Platforms & Dependencies
========================

The L+ compiler is written in pure ANSI/ISO C++ and supports GNU/Linux, Mac OS X and Microsoft Windows. The build system is UNIX-based (using Makefiles), so other UNICES should work fine too.

The compiler's build requirements are only GNU Flex (2.5.35), Bison (2.4), a C++ development environment and make.

Compiled programs require a Java VM to execute.

Since 2006
==========

The codebase is mostly the untouched assignment we turned in in 2006. I had to perform some minor changes (specially to the build and deployment system) because I basically didn't know how to write sane Makefiles back in 2006.

I've also translated most messages from Spanish to English, but code comments and code examples remain in their original language. I might translate these as well at some point. Other than that, all compiler activity is reported in English, so don't be afraid to dig in ;)

Special Thanks
==============

We want to thank Leonardo Val and Matías Hernandez, who were our professors and designed the assignment this program was developed to fullfil.
