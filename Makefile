PREFIX = /usr/local/bin
DESTDIR = 
PACKAGE_NAME = morse-iled
CC = gcc
CFLAGS = -Wall

all:
	mkdir -p build
	$(CC) $(CFLAGS) -o ./build/$(PACKAGE_NAME) main.c

clean:
	rm -Rf ./build

install:
	install -m 755 ./build/$(PACKAGE_NAME) $(DESTDIR)$(PREFIX)

uninstall:
	rm $(DESTDIR)$(PREFIX)/$(PACKAGE_NAME)
