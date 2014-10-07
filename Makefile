
all: build/createAndReadPipe.exe

build:
	mkdir -p $@

build/createAndReadPipe.exe: build/createAndReadPipe.o | build
	gcc -s -o $@ $^

build/createAndReadPipe.o: createAndReadPipe.c | build
	gcc -c $^ -o $@

clean:
	rm -rf build
