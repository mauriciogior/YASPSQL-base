all: Node.o Table.o Database.o Shell.o sgbd.o
	@g++ -g Node.o Table.o Database.o Shell.o sgbd.o -Wall -std=c++0x -o sgbd

run:
	@./sgbd

Node.o:
	@g++ -g -c -std=c++0x Node.cc

Table.o:
	@g++ -g -c -std=c++0x Table.cc

Database.o:
	@g++ -g -c Database.cc

Shell.o:
	@g++ -g -c Shell.cc

sgbd.o:
	@g++ -g -c sgbd.cc

zip:
	@zip -r t1.zip Makefile Database.cc Database.cc Node.cc Node.h sgbd.cc sgbd.h Shell.cc Shell.h Table.cc Table.h Struct.h
