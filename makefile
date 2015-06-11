.PHONY : all install

all : build/daemon build/parser build/libyauj.so

build/libyauj.so : src/interpreter.cpp src/function.cpp
	g++ src/interpreter.cpp src/function.cpp src/vjudge_hack.cpp -std=c++11 -fPIC -shared -lboost_regex -ljsoncpp -lcurl -o build/libyauj.so -O2

build/daemon : src/daemon.cpp build/abstractstubserver.h src/config_daemon.h
	g++ src/daemon.cpp -o build/daemon -pthread -ljsoncpp -lmicrohttpd -ljsonrpccpp-common -ljsonrpccpp-server -Isrc -Ibuild -O2

build/abstractstubserver.h : src/spec.json
	jsonrpcstub src/spec.json --cpp-server=AbstractStubServer --cpp-server-file=build/abstractstubserver.h

build/parser : build/lex.yy.c build/parser.tab.c src/mystr.c build/parser.tab.h src/mystr.h
	gcc build/lex.yy.c build/parser.tab.c src/mystr.c -o build/parser -Isrc -Ibuild -O2

build/lex.yy.c : src/parser.l
	flex -o build/lex.yy.c src/parser.l

build/parser.tab.c build/parser.tab.h : src/parser.y
	bison -d src/parser.y
	mv parser.tab.c parser.tab.h build/


install : build/daemon distribute.makefile build/parser
	cp build/daemon /usr/bin/yauj_daemon
	cp distribute.makefile /home/judge/resource/makefile
	cp build/libyauj.so /usr/lib/
	cp -r src /home/judge/resource/
	cp build/parser /usr/bin/yauj_parser
