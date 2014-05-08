linedelaybuffer: linedelaybuffer.cpp
	g++ linedelaybuffer.cpp -Wall -Wextra -O3 -g -o linedelaybuffer

.PHONY: install

install: linedelaybuffer
	cp linedelaybuffer /usr/local/bin/
