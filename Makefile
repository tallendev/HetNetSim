CC=g++
# CC=clang
CFLAGS=-Wall -Wextra -Wconversion -g #$(AA)
CPPEXT=cpp
HEXT=h
OBJEXT=o
SRCDIR=src
TARGET=bin/run
BIN=bin
BUILDDIR=build

SOURCES=$(shell find $(SRCDIR) -type f -name *.$(CPPEXT))
HEADERS=$(shell find $(SRCDIR) -type f -name *.$(HEXT)) 
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(CPPEXT)=.$(OBJEXT)))

all: $(OBJECTS) $(HEADERS)
	@echo "Sources: $(shell find $(SRCDIR) -type f -name *.$(CPPEXT))"
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(CPPEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p bin
	@echo "$(CC) $(CFLAGS) $(SOURCES) -c -o $@ $<"
	@$(CC) $(CFLAGS) $(SOURCES) -c -o $@ $<

clean:
	rm -fr $(BUILDDIR) $(BIN)

#.c.o: $<
#	-gcc -c $(CFLAGS) $(DEBUG) -g $< 2> $(@:.o=.err)
#	cat $*.err
#

.PHONY: clean
