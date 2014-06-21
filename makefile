all: linedelaybuffer

VERSION_STRING := $(shell git describe --always --dirty=-m 2>/dev/null || date "+%F %T %z" 2>/dev/null)
ifdef VERSION_STRING
CVFLAGS := -DVERSION_STRING='"${VERSION_STRING}"'
endif

linedelaybuffer: linedelaybuffer.cpp
	g++ linedelaybuffer.cpp -Wall -Wextra -O3 -g -o linedelaybuffer ${CVFLAGS}

.PHONY: install all clean

clean:
	rm -f linedelaybuffer linedelaybuffer.1

install: linedelaybuffer
	cp linedelaybuffer /usr/local/bin/

HELP2MANOK := $(shell help2man --version 2>/dev/null)
ifdef HELP2MANOK
all: linedelaybuffer.1

linedelaybuffer.1: linedelaybuffer
	help2man -s 1 -N ./linedelaybuffer -n "Line Delay Buffer" -o linedelaybuffer.1

install: install-man

.PHONY: install-man

install-man: linedelaybuffer.1
	cp linedelaybuffer.1 /usr/local/share/man/man1/
	-mandb -pq

else
$(shell echo "Install help2man for man page generation" >&2)
endif
