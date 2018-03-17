.PHONY: build clean
build: yhttpd
yhttpd: ylib.o
	$(CC) $@.c ylib.o -o $@
clean:
	-rm ylib.o yhttpd
