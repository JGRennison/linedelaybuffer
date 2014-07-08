prefix ?= /usr/local

all: linedelaybuffer

VERSION_STRING := $(shell cat version 2>/dev/null || git describe --tags --always --dirty=-m 2>/dev/null || date "+%F %T %z" 2>/dev/null)
ifdef VERSION_STRING
CVFLAGS := -DVERSION_STRING='"${VERSION_STRING}"'
endif

linedelaybuffer: linedelaybuffer.cpp
	g++ linedelaybuffer.cpp -Wall -Wextra -O3 -g -o linedelaybuffer ${CVFLAGS}

.PHONY: install all clean dumpversion

dumpversion:
	@echo $(VERSION_STRING)

clean:
	rm -f linedelaybuffer linedelaybuffer.1

install: linedelaybuffer
	install -D -m 755 linedelaybuffer $(DESTDIR)$(prefix)/bin/linedelaybuffer

uninstall:
	rm -f $(DESTDIR)$(prefix)/bin/linedelaybuffer $(DESTDIR)$(prefix)/share/man/man1/linedelaybuffer.1

HELP2MANOK := $(shell help2man --version 2>/dev/null)
ifdef HELP2MANOK
all: linedelaybuffer.1

linedelaybuffer.1: linedelaybuffer
	help2man -s 1 -N ./linedelaybuffer -n "Line Delay Buffer" -o linedelaybuffer.1

install: install-man

.PHONY: install-man

install-man: linedelaybuffer.1
	install -D -m 644 linedelaybuffer.1 $(DESTDIR)$(prefix)/share/man/man1/linedelaybuffer.1
	-mandb -pq

else
$(shell echo "Install help2man for man page generation" >&2)
endif
