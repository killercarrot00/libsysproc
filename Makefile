all: libsysproc.so

libprocesshider.so: processhider.c
	gcc -Wall -fPIC -shared -o libsysproc.so libsysproc.c -ldl

.PHONY clean:
	rm -f libsysproc.so
