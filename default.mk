NAME = picoshell
TOPDIR  ?= ..

CC       = gcc
CFLAGS   = -Wall -Werror -Wfatal-errors -O2 -Wcast-align
INCLUDES = -I$(TOPDIR)/include

OBJECTS  = $(SOURCES:%.c=%.o)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean::
	rm -f $(OBJECTS)
