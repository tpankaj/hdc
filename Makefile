.PHONY: build test init clean

all: build

build:
	mkdir -p build && cd build && cmake .. && make

test: build
	cd build/test && ctest

init:
	git submodule update --init --recursive

clean:
	rm -rf build lib/*~
