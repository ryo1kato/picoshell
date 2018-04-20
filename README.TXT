PicoShell - A very small C library for shell-like CLI with emacs-like keybind

                                            Ryoichi KATO <ryo1kato@gmail.com>

Introduction

    PicoShell is a tiny C library for shell-like CLI with emacs-like line-edit
    and command history support, aimed for CLI of bootloader or tiny, resource
    poor device like PICs or Arduino.

    PicoShell is implemented with using just a few external C functions
    listed below, which are similar or same as the standard C library
    APIs like getchar(), putchar(), memcpy() etc.

    So even in very poor programing environment without standard C library,
    this library should work with little effort to implement these handful
    functions.


Compiling

    Just type 'make' to comple a sample on a Unix machine (Tested on
    Linux and MacOSX)
    There's no API documentation (yet). Please see "sample" directory
    for usage. Note that this library is originally developed as 'msh'
    and function names and macro definitions are still using it as prefix.

    You can configure keybind used for line editor, meta charactors, and
    command line history behavior, by editing include/picoshell_config.h.


Arduino

    'make arduino' to generate a stub project directory 'picoshell_arduino'.
    Open 'picoshell_arduino.ino' file in Arduino IDE, build and upload.
    You probably want to disable some of the features to save ROM / RAM
    by editing picoshell_config.h file.


External Functions

    PicoShell uses the following functions.

        <stdio.h>
            getchar (as pico_getchar)
            putchar (as pico_putchar)
            puts    (as pico_puts)

        <string.h>
            memset
            strcpy
            strlen
            strcpy
            strcmp

    getchar(), putchar(), and puts() should implemented slightly differently
    from standard C specification, and for that reason the code is referring
    the functions with "pico_" prefix so that easier to adjust / wrap.

        * All IO functions (getchar, putchar, and puts) should NOT buffered.
        * puts() should NOT emit trailing newline.


