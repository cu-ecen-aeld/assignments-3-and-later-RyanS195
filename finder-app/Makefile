COMPILE_TYPE :=$(CROSS_COMPILE)gcc
CC           ?= gcc

all: writer.c
	$(CC) writer.c -o writer

compile: writer.c
	$(COMPILE_TYPE) writer.c -o writer

clean:
	rm -f writer
