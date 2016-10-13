test.exe:test.o
	g++ -rdynamic test.o -o test.exe

test.o:../src/log.h ../src/stacktrace.h
	g++ -g -c test.cc -o test.o
