makelib: flags.o
	ar cr libflag.a flags.o

flags.o: flags.c flags.h
	gcc -c -Wall flags.c

install: makelib
	cp libflag.a /usr/lib/libflags.a

clean:
	rm libflag.a flags.o
