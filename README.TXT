PicoShell - A very small C library for shell-like CLI with emacs-like keybind

                                            Ryoichi KATO <ryo1kato@gmail.com>

Introduction

    PicoShell is a tiny C library for shell-like CLI with emacs-like line-edit
    and command history support, aimed for CLI of bootloader or tiny, resource
    poor device like PICs.

    PicoShell is implemented with using just a few external C functions
    listed below, which are similar or same as the standard C library
    APIs like getchar(), putchar(), memcpy() etc.

    So even in very poor programing environment without standard C library,
    this library should work with little effort to implement these handful
    functions.


Compiling

    Just type 'make' to comple a sample. (of course you need GCC)
    There's no API documentation (yet). Please see sample/ directory
    for usage. Note that this library is originally developed as 'msh'
    and function names and macro definitions are still using this prefix.

    You can configure keybind used for line editor, meta charactors, and
    command line history behavior, by editing include/picoshell_config.h.


External Functions

    PicoShell uses the following functions. As stated above, getchar(),
    putchar(), puts() should implemented slightly differently from
    standard C specification:

        * All IO functions (getchar, putchar, and puts) should NOT buffered
        * puts() should NOT emit trailing newline.

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


