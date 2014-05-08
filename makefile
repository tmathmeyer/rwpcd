#
# Ted Meyer
# config file for rwpcd
# random wallpaper changing daemon
# GPL v2
#
# 2014-05-07
#



rwpcd: changer.c config.h
	gcc -Wall changer.c -o rwpcd -lflags

install: rwpcd
	echo "not yet"

clean:
	rm *.o rwpcd