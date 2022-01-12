SFML = `pkg-config --libs --cflags sfml-graphics`
BOOST = -lboost_filesystem -lboost_thread
DEBUG = -g -fsanitize=address 

STD = c++20
CC = clang++

CONF = -O3 -std=$(STD) 
LINK = -pthread -march=native $(BOOST) $(SFML)

OUT = dither 

build: $(OUT)
	 
headers.hpp.gch: headers.hpp Makefile
	$(CC) $< $(CONF)

test.o: test.cc headers.hpp.gch
	$(CC) $< $(CONF) -c

$(OUT): test.o headers.hpp.gch
	$(CC) $< -o $(OUT) $(LINK)

profile: test.cc
	$(CC) $< -pg $(CONF) $(DEBUG) -c
	$(CC) test.o -o $(OUT) $(LINK)


gen: test.cc
ifeq ($(CC),g++)
	$(CC) $< -fprofile-generate $(CONF) -o $(OUT) $(LINK)
endif
ifeq ($(CC),clang++)
	$(CC) $< -fprofile-instr-generate="code-%p.profraw" -o $(OUT) $(CONF) $(LINK)
endif


use: test.cc
ifeq ($(CC),g++)
	$(CC) -fprofile-use $< $(CONF) -o $(OUT) $(LINK)
endif
ifeq ($(CC),clang++)
	llvm-profdata merge -output=default.profdata code-*.profraw
	$(CC) -fprofile-instr-use test.cc -o $(OUT) $(CONF) $(LINK)
endif


install: build
	cp $(OUT) /usr/bin/
