CC=g++-4.8
# CC=clang
CFLAGS= -Wall -Wextra -Wconversion -g -std=gnu++11
CPPEXT=cpp
HEXT=h
OBJEXT=o
SRCDIR=src
TESTDIR=test
TARGET=bin/solver.so
BIN=bin
BUILDDIR=build
HEADERDIR=include
INCLUDE=-I./$(HEADERDIR)

SOURCES = $(shell find $(SRCDIR) -type f -name *.$(CPPEXT))
FEASIBLESOURCES := $(shell find $(SRCDIR)/ -type f ! -name "interface.cpp" -name *.$(CPPEXT))
FEASIBLESOURCES += $(shell find $(TESTDIR)/ -type f -name feasible.$(CPPEXT))
INFEASIBLESOURCES := $(shell find $(SRCDIR)/ -type f ! -name "interface.cpp" -name *.$(CPPEXT))
INFEASIBLESOURCES += $(shell find $(TESTDIR)/ -type f -name infeasible.$(CPPEXT))
HEADERS = $(shell find $(HEADERDIR) -type f -name *.$(HEXT)) 
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(CPPEXT)=.$(OBJEXT)))
FEASIBLEOBJECTS = $(filter-out $(SRCDIR)/interface.$(OBJEXT), $(OBJECTS))
FEASIBLEOBJECTS := $(feasible.o, $(FEASIBLEOBJECTS))
INFEASIBLEOBJECTS = $(filter-out $(SRCDIR)/interface.$(OBJEXT), $(OBJECTS))
INFEASIBLEOBJECTS := $(infeasible.o, $(INFEASIBLEOBJECTS))

all: interface_wrap.cpp $(OBJECTS) $(HEADERS)
	@echo "Sources: $(shell find $(SRCDIR) -type f -name *.$(CPPEXT))"
	$(CC) -fPIC -shared $(shell php-config --includes) $(INCLUDE) $(CFLAGS) $(SOURCES) -o $(TARGET)

testfeasible:
	@echo "Sources: $(FEASIBLESOURCES)"
	@mkdir -p $(BUILDDIR)
	@mkdir -p bin
	$(CC) $(INCLUDE) $(CFLAGS) $(FEASIBLESOURCES) -o $(TARGET)

testinfeasible:
	@echo "Sources: $(INFEASIBLESOURCES)"
	@mkdir -p $(BUILDDIR)
	@mkdir -p bin
	$(CC) $(INCLUDE) $(CFLAGS) $(INFEASIBLESOURCES) -o $(TARGET)

clang: $(OBJECTS) $(HEADERS)
	@echo "Sources: $(shell find $(SRCDIR) -type f -name *.$(CPPEXT))"
	clang $(INCLUDE) $(CFLAGS) $(SOURCES) -o $(TARGET)

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(CPPEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p bin
	@echo "$(CC) $(CFLAGS) $(INCLUDE) $(SOURCES) -c -o $@"
	-$(CC) $(shell php-config --includes) -fPIC $(INCLUDE) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR) $(BIN)

interface_wrap.cpp: 
	swig -c++ -php -I./include -o src/interface.cpp src/interface.i
#.c.o: $<
#	-gcc -c $(CFLAGS) $(DEBUG) -g $< 2> $(@:.o=.err)
#	cat $*.err
#

.PHONY: clean
