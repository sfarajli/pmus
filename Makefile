.POSIX:

PREFIX ?= ~/.local

pmus: pmus.c
	gcc -lX11 $< -o $@

install: 
	cp -f pmus ${DESTDIR}${PREFIX}/bin

clean:
	rm -f pmus

.PHONY: install clean
