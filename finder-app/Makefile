COMPILE_TYPE :=$(CROSS_COMPILE)gcc

all: writer.c
	aarch64-none-linux-gnu-gcc writer.c -o writer

compile: writer.c
	$(COMPILE_TYPE) writer.c -o writer

clean:
	rm -f writer
