SFML = `pkg-config --libs --cflags sfml-graphics`
BOOST = -lboost_filesystem -lboost_thread
DEBUG = -g -fsanitize=address 

STD = c++20
CC = clang++   
CONF = -O3 -fno-pie -std=$(STD) -fPIC -pthread -march=native

build:
	$(CC) test.cc -o un $(CONF) $(SFML) $(BOOST)

profile:
	$(CC) -pg test.cc -o un $(DEBUG) $(CONF) $(SFML) $(BOOST)


gen:
	$(CC) -fprofile-generate test.cc -o un $(CONF) $(SFML) $(BOOST)

	
use:
	$(CC) -fprofile-use test.cc -o un $(CONF) $(SFML) $(BOOST)


gen_clang:
	$(CC) -fprofile-instr-generate="code-%p.profraw"  test.cc -o un $(CONF) $(SFML) $(BOOST)


use_clang:
	llvm-profdata merge -output=default.profdata code-*.profraw
	$(CC) -fprofile-instr-use test.cc -o un $(CONF) $(SFML) $(BOOST)

install: build
	cp un /usr/bin/
