.PHONY: all test build clean

BUILD_TYPE?=Release

CMAKE_OPTS = -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

clean:
	rm -rf ./build

build:
	echo building $(BUILD_TYPE)
	mkdir -p build \
	&& cd build \
	&& cmake .. $(CMAKE_OPTS) \
	&& cmake --build .

test: build
	./build/tests/wavio_unit_tests


all: clean build
