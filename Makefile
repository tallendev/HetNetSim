CC=g++-4.8
ifeq (clang, $(firstword $(MAKECMDGOALS)))
	CC=clang
endif
CFLAGS= -Wall -Wextra -Wno-unused-label -Wno-literal-suffix -Wno-unused-function -Wno-sign-compare -Wno-unused-parameter -Wno-missing-field-initializers -std=gnu++11
DEBUG= -g -DSERVER_DEBUG
CPPEXT=cpp
HEXT=h
OBJEXT=o
SRCDIR=src
BIN=bin
LIB=lib

TARGET=$(LIB)/solver.so
TEST_TARGET=$(BIN)/run
SWIG=interface.i
SWIGCPP=interface_wrap.$(CPPEXT)
TESTFILE=

BUILDDIR=build
TESTDIR=test
HEADERDIR=include

INCLUDE=-I./$(HEADERDIR)

SOURCES=$(shell ls src/*.cpp)
TEST_SOURCES=src/$(shell find src/ -type f ! -iname '$(SWIGCPP)')
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(CPPEXT)=.$(OBJEXT)))
TEST_OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(TEST_SOURCES:.$(CPPEXT)=.$(OBJEXT)))

all: $(SWIGCPP) $(OBJECTS) $(HEADERS)
	@echo "Sources: $(SOURCES)"
	$(CC) -fPIC -shared $(shell php-config --includes) $(DEBUG) $(INCLUDE) $(CFLAGS) $(SOURCES) -o $(TARGET)

clang: $(SWIGCPP) $(OBJECTS) $(HEADERS)
	@echo "Sources: $(SOURCES)"
	$(CC) -fPIC -shared $(shell php-config --includes) $(DEBUG) $(INCLUDE) $(CFLAGS) $(SOURCES) -o $(TARGET)

test: $(TEST_SOURCES) $(TESTFILE) $(TEST_OBJECTS)
	$(CC) $(TEST_SOURCES) $(TEST_FILE) $(INCLUDE) $(CFLAGS) -o $(TEST_TARGET)

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(CPPEXT)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BIN)
	@mkdir -p $(LIB)
	-$(CC) $(INCLUDE) $(shell php-config --includes) -c $(CFLAGS) $(DEBUG) $< -o $@

install:
	#@cp www/* /var/www/
	@cp lib/solver.so $(shell php-config --extension-dir)

clean:
	rm -rf $(BUILDDIR) $(BIN) $(LIB)



$(SWIGCPP): 
	swig -c++ -php -I./include -o $(SRCDIR)/$(SWIGCPP) $(SRCDIR)/$(SWIG)

.PHONY: clean
