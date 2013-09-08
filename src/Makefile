SOURCES = lineedit.c commands.c history.c parse.c
all: ../libmsh.a

include ../default.mk

../libmsh.a: $(OBJECTS)
	$(AR) cruv $@ $^

clean::
	rm -f ../libmsh.a

