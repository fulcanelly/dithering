SFML = `pkg-config --libs --cflags sfml-graphics`
BOOST = -lboost_filesystem -lboost_thread

CC = mold -run g++  
CONF = -fno-pie -std=c++20 -fPIC -O3 -pthread
DEBUG = -g -fsanitize=address 

build:
	$(CC) test.cc -o un $(CONF) $(SFML) $(BOOST)



gen:
	$(CC) -fprofile-generate test.cc -o un $(CONF) $(SFML) $(BOOST)

	
use:
	$(CC) -fprofile-use test.cc -o un $(CONF) $(SFML) $(BOOST)


install: build
	cp un /usr/bin/
