COMPILE_TYPE :=$(CROSS_COMPILE)gcc

all: compile

compile: writer.c
	$(COMPILE_TYPE) writer.c -o writer

clean:
	rm -f writer
