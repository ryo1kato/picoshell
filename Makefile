all:
	$(MAKE) -C sample

clean:
	$(MAKE) -C sample clean

cleanall: clean
	rm -rf picoshell_arduino

arduino:
	mkdir -p picoshell_arduino
	cat src/*.c > picoshell_arduino/picoshell.cpp
	cp include/*.h src/*.h picoshell_arduino
	cp sample/sample.c picoshell_arduino/sample.cpp
	cp sample/io_arduino.ino picoshell_arduino/picoshell_arduino.ino
