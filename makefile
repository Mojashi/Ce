CCC = g++
LEX = flex
YACC = bison

TARGET =ce.exe
SOURCEDIR = ./source/
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
PYTHON =python3.8
PYTHONOPTION =  -l$(PYTHON) $(shell $(PYTHON)-config --cflags --ldflags)
NOPYOPTION = -DNO_PYTHON
OBJS = $(OUTDIR)main.o $(OUTDIR)ast.o $(OUTDIR)parse.tab.o $(OUTDIR)lex.yy.o $(OUTDIR)builtIn.o
PYOBJ = $(OUTDIR)formatresult.o

.PHONY: Debug Release NPDebug NPRelease
Debug: CFLAGS+= $(CDEBUGFLAGS)
Debug: OUTDIR+=$(DEBUGDIR)
Debug: YACCOPTION=$(YACCDEBUG)
Debug: LEXOPTION=$(LEXDEBUG)
Debug: OBJS+=$(PYOBJ)
Debug: formatresult.o
Debug: all

Release	: CFLAGS+=$(CRELEASEFLAGS)
Release	: OUTDIR+=$(RELEASEDIR)
Release: YACCOPTION=$(YACCRELEASE)
Release: OBJS += $(PYOBJ)
Release: formatresult.o
Release: all

NPDebug: CFLAGS+=$(CDEBUGFLAGS) $(NOPYOPTION)
NPDebug: OUTDIR+=$(DEBUGDIR)
NPDebug: YACCOPTION=$(YACCDEBUG)
NPDebug: LEXOPTION=$(LEXDEBUG)
NPDebug: PYTHONOPTION=
NPDebug: all

NPRelease	: CFLAGS+=$(CRELEASEFLAGS) $(NOPYOPTION)
NPRelease	: OUTDIR+=$(RELEASEDIR)
NPRelease: YACCOPTION=$(YACCRELEASE)
NPRelease: PYTHONOPTION=
NPRelease: all

all : $(TARGET)
$(TARGET) : $(OBJS)
	$(CCC) $(CFLAGS) -o $(OUTDIR)ce $(OBJS) $(PYTHONOPTION)

main.o: $(SOURCEDIR)main.cpp $(SOURCEDIR)parse.y $(SOURCEDIR)scan.l $(SOURCEDIR)parse.tab.c $(SOURCEDIR)lex.yy.c $(SOURCEDIR)graphviz.h $(SOURCEDIR)CNF.h 
	$(CCC) $(CFLAGS) -c $(SOURCEDIR)main.cpp -o $(OUTDIR)main.o

$(SOURCEDIR)parse.tab.c: $(SOURCEDIR)parse.y $(SOURCEDIR)ast.h
	$(YACC) $(YACCOPTION) $(SOURCEDIR)parse.y -o $(SOURCEDIR)parse.tab.c

ast.o: $(SOURCEDIR)ast.cpp $(SOURCEDIR)ast.h
	$(CCC) $(CFLAGS) -c $(SOURCEDIR)ast.cpp -o $(OUTDIR)ast.o

builtIn.o: $(SOURCEDIR)builtIn.cpp $(SOURCEDIR)builtIn.h
	$(CCC) $(CFLAGS) -c $(SOURCEDIR)builtIn.cpp -o $(OUTDIR)builtIn.o

formatresult.o: $(SOURCEDIR)formatresult.cpp $(SOURCEDIR)formatresult.h
	$(CCC) $(CFLAGS) -c $(SOURCEDIR)formatresult.cpp -o $(OUTDIR)formatresult.o $(PYTHONOPTION)

parse.tab.o: $(SOURCEDIR)parse.tab.c
	$(CCC) $(CFLAGS) -c $(SOURCEDIR)parse.tab.c -o $(OUTDIR)parse.tab.o

$(SOURCEDIR)lex.yy.c: $(SOURCEDIR)scan.l parse.tab.o
	$(LEX) $(LEXOPTION) -o $(SOURCEDIR)lex.yy.c $(SOURCEDIR)scan.l 

lex.yy.o: $(SOURCEDIR)lex.yy.c
	$(CCC) $(CFLAGS) $(LEXFLAGS) -c -o $(OUTDIR)lex.yy.o $(SOURCEDIR)lex.yy.c
	

clean:
	rm -f */$(TARGET) */*.o $(SOURCEDIR)parse.tab.c $(SOURCEDIR)lex.yy.c
	rm -f $(SOURCEDIR)parse.tab.h
	rm -f parse.output
	rm -f */*.gcda */*.gcno */*.bb */*.bbg */*.da */*.gcov
	rm -f */*.dot *.dot *.log result.txt log.txt 