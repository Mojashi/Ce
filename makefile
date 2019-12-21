CCC = g++
LEX = flex
YACC = bison

TARGET =ce.exe
OUTDIR =
DEBUGDIR=./Debug/
RELEASEDIR=./Release/
CDEBUGFLAGS= -g3 -DDEBUG -DYYDEBUG -fprofile-arcs -ftest-coverage 
CRELEASEFLAGS = -O3
CFLAGS = -std=c++14 -Wall
LEXFLAGS = -Wno-unused

LEXDEBUG = -d
YACCDEBUG = -vtd --graph --debug
YACCRELEASE = -d
LEXOPTION = 
YACCOPTION =
PYTHONOPTION = -L"C:\Program Files (x86)\Microsoft Visual Studio\Shared\Python37_64\libs" -I"C:\Program Files (x86)\Microsoft Visual Studio\Shared\Python37_64\include" -lpython37
NOPYOPTION = -DNO_PYTHON
OBJS = $(OUTDIR)main.o $(OUTDIR)ast.o $(OUTDIR)parse.tab.o $(OUTDIR)lex.yy.o $(OUTDIR)builtIn.o
PYOBJ = $(OUTDIR)formatresult.o

.PHONY: Debug Release NPDebug NPRelease
Debug: CFLAGS+= $(CDEBUGFLAGS)
Debug: OUTDIR=$(DEBUGDIR)
Debug: YACCOPTION=$(YACCDEBUG)
Debug: LEXOPTION=$(LEXDEBUG)
Debug: OBJS+=$(PYOBJ)
Debug: formatresult.o
Debug: all

Release	: CFLAGS+=$(CRELEASEFLAGS)
Release	: OUTDIR=$(RELEASEDIR)
Release: YACCOPTION=$(YACCRELEASE)
Release: OBJS += $(PYOBJ)
Release: formatresult.o
Release: all

NPDebug: CFLAGS+=$(CDEBUGFLAGS) $(NOPYOPTION)
NPDebug: OUTDIR=$(DEBUGDIR)
NPDebug: YACCOPTION=$(YACCDEBUG)
NPDebug: LEXOPTION=$(LEXDEBUG)
NPDebug: PYTHONOPTION=
NPDebug: all

NPRelease	: CFLAGS+=$(CRELEASEFLAGS) $(NOPYOPTION)
NPRelease	: OUTDIR=$(RELEASEDIR)
NPRelease: YACCOPTION=$(YACCRELEASE)
NPRelease: PYTHONOPTION=
NPRelease: all

all : $(TARGET)
$(TARGET) : $(OBJS)
	$(CCC) $(CFLAGS) -o $(OUTDIR)ce $(OBJS) $(PYTHONOPTION)

main.o: main.cpp parse.y scan.l parse.tab.c lex.yy.c graphviz.h CNF.h 
	$(CCC) $(CFLAGS) -c main.cpp -o $(OUTDIR)main.o

parse.tab.c: parse.y ast.h
	$(YACC) $(YACCOPTION) parse.y

ast.o: ast.cpp ast.h
	$(CCC) $(CFLAGS) -c ast.cpp -o $(OUTDIR)ast.o

builtIn.o: builtIn.cpp builtIn.h
	$(CCC) $(CFLAGS) -c builtIn.cpp -o $(OUTDIR)builtIn.o

formatresult.o: formatresult.cpp formatresult.h
	$(CCC) $(CFLAGS) -c formatresult.cpp -o $(OUTDIR)formatresult.o $(PYTHONOPTION)

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