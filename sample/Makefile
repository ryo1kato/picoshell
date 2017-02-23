all: exe
SOURCES = $(wildcard *.c)
OBJECTS = $(.c:.o $(SOURCES))
include ../default.mk

exe: $(NAME)
$(NAME): $(OBJECTS) ../lib$(NAME).a
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $(filter %.o,$^) -l$(NAME) -L..

../lib$(NAME).a:
	$(MAKE) -C ../src

clean::
	$(MAKE) -C ../src clean
	rm -f $(NAME)

