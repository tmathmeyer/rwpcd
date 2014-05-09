#
# Ted Meyer
# make file for rwpcd
# random wallpaper changing daemon
# GPL v2
#
# 2014-05-07
#



rwpcd: changer.c config.h flags.o
	gcc -Wall changer.c flags.o -o rwpcd

flags.o: flags.c flags.h
	gcc -c flags.c

install: rwpcd
	cp rwpcd /usr/local/bin/rwpcd

clean:
	rm *.o rwpcd
