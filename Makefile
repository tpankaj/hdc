.PHONY: build init clean

all: build

build:
	mkdir -p build && cd build && cmake .. && make

init:
	git submodule update --init --recursive

clean:
	rm -rf build lib/*~
