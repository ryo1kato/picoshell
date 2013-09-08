MSH - Minimal SHell

                                         Ryoichi KATO <ryo1kato@gmail.com>

Introduction

    MSH is a tiny shell environment aimed for bootloader, which includes
    command-line editor (emacs-like, with history functionality) and parser.

    MSH is implemented with using very limited external functions listed below,
    which are somehow like a subset of standard C library function.
    (NOTE: MSH need your getchar(), putchar() and puts() implementations are
     UNBUFFERD.  Also, puts() requred to be not emit trailing newline character)


Compiling

    Just type 'make' to comple a sample.  You have to write your own
    Makefile to incorporate MSH into your software.


Testing

    Not implemented yet. (OK, I know what 'test first' is...)


External Functions

    MSH uses the following functions. As stated above, getchar(),
    putchar(), puts() should implemented slightly differently from
    standard C specification.

    (A) All IO functions (getchar, putchar, and puts) should NOT buffered
    (B) puts() should NOT emit trailing newline.

        <stdio.h>
            getchar
            putchar
            puts

        <string.h>
            memset
            strcpy
            strlen
            strcpy
            strcmp

