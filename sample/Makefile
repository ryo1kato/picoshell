SOURCES = msh.c
all: msh

include ../default.mk


msh: msh.o ../libmsh.a
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< -lmsh -L..

../libmsh.a:
	$(MAKE) -C ../src

clean::
	$(MAKE) -C ../src clean
	rm -f msh

