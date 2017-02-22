all: exe
SOURCES = $(wildcard *.c)
include ../default.mk

exe: $(NAME)
$(NAME): main.o io_unix.o ../lib$(NAME).a
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(filter %.o,$^) -l$(NAME) -L..

../lib$(NAME).a:
	$(MAKE) -C ../src

clean::
	$(MAKE) -C ../src clean
	rm -f $(NAME)

