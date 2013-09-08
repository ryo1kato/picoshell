all:
	$(MAKE) -C sample

clean:
	$(MAKE) -C sample clean
	rm -f msh
