CC=g++
# CC=clang
CFLAGS=-Wall -Wextra -Wconversion -g -std=gnu++11
CPPEXT=cpp
HEXT=h
OBJEXT=o
SRCDIR=src
TARGET=bin/run
BIN=bin
BUILDDIR=build
HEADERDIR=include
INCLUDE=-I./$(HEADERDIR)

SOURCES=$(shell find $(SRCDIR) -type f -name *.$(CPPEXT))
HEADERS=$(shell find $(HEADERDIR) -type f -name *.$(HEXT)) 
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(CPPEXT)=.$(OBJEXT)))

all: $(OBJECTS) $(HEADERS)
	@echo "Sources: $(shell find $(SRCDIR) -type f -name *.$(CPPEXT))"
	$(CC) $(INCLUDE) $(CFLAGS) $(SOURCES) -o $(TARGET)

clang: $(OBJECTS) $(HEADERS)
	@echo "Sources: $(shell find $(SRCDIR) -type f -name *.$(CPPEXT))"
	clang $(INCLUDE) $(CFLAGS) $(SOURCES) -o $(TARGET)

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(CPPEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p bin
	@echo "$(CC) $(CFLAGS) $(INCLUDE) $(SOURCES) -c -o $@"
	-$(CC) $(INCLUDE) -c $(CFLAGS) $< -o $@

clean:
	rm -fr $(BUILDDIR) $(BIN)

#.c.o: $<
#	-gcc -c $(CFLAGS) $(DEBUG) -g $< 2> $(@:.o=.err)
#	cat $*.err
#

.PHONY: clean
