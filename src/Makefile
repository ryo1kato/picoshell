all: lib
SOURCES = lineedit.c commands.c history.c parse.c
include ../default.mk


lib: ../lib$(NAME).a

../lib$(NAME).a: $(OBJECTS)
	$(AR) cruv $@ $^

clean::
	rm -f ../lib$(NAME).a

