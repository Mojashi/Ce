CCC = g++
LEX = flex
YACC = bison

TARGET =Ce.exe
OUTDIR =
DEBUGDIR=./Debug/
RELEASEDIR=./Release/
CDEBUGFLAGS= -g3 -DDEBUG -DYYDEBUG -fprofile-arcs -ftest-coverage 
CRELEASEFLAGS = -O3
CFLAGS = -std=c++14 -Wall
LEXFLAGS = -Wno-unused

LEXDEBUG = -d
YACCDEBUG = -vtd --graph --debug
LEXOPTION = 
YACCOPTION = 

OBJS = $(OUTDIR)main.o $(OUTDIR)ast.o $(OUTDIR)parse.tab.o $(OUTDIR)lex.yy.o

.PHONY: Debug
Debug: CFLAGS+=$(CDEBUGFLAGS)
Debug: OUTDIR=$(DEBUGDIR)
Debug: YACCOPTION=$(YACCDEBUG)
Debug: LEXOPTION=$(LEXDEBUG)
Debug: all

.PHONY	: Release
Release	: CFLAGS+=$(CRELEASEFLAGS)
Release	: OUTDIR=$(RELEASEDIR)
Release	: $(TARGET)

all : $(TARGET)
$(TARGET) : $(OBJS)
	$(CCC) $(CFLAGS) -o $(OUTDIR)Ce $(OBJS)

main.o: main.cpp parse.y scan.l parse.tab.c lex.yy.c graphviz.h CNF.h
	$(CCC) $(CFLAGS) -c main.cpp -o $(OUTDIR)main.o

parse.tab.c: parse.y ast.h
	$(YACC) $(YACCOPTION) parse.y

ast.o: ast.cpp ast.h
	$(CCC) $(CFLAGS) -c ast.cpp -o $(OUTDIR)ast.o

parse.tab.o: parse.tab.c
	$(CCC) $(CFLAGS) -c parse.tab.c -o $(OUTDIR)parse.tab.o

lex.yy.c: scan.l parse.tab.o
	$(LEX) $(LEXOPTION) scan.l

lex.yy.o: lex.yy.c
	$(CCC) $(CFLAGS) $(LEXFLAGS) -c lex.yy.c -o $(OUTDIR)lex.yy.o
	

clean:
	rm -f */$(TARGET) */*.o parse.tab.c lex.yy.c
	rm -f parse.tab.h
	rm -f parse.output
	rm -f */*.gcda */*.gcno */*.bb */*.bbg */*.da */*.gcov
	rm -f */*.dot *.dot