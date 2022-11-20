CFLAGS := -std=c11 -fPIC -rdynamic -g -O2 -Wall -I include -I tests/include/
CPPFLAGS :=

CXXFLAGS := -std=c++11 -fPIC -rdynamic -g -O2 -Wall -I include -I tests/include/

SOFLAGS := -shared
LD := gcc
LDFLAGS := -L .
LINK_ARGS = -o $@ $(LDFLAGS) $(addprefix -l:,$|) $^

#MC := valgrind
#MCFLAGS := --leak-check=full --suppressions=./valgrind.supp --error-exitcode=1 -q


ODF_SOURCES := $(wildcard src/odf/*.c)
ODF_OBJECTS := $(ODF_SOURCES:.c=.o)
ODF_MAIN := "bin/dark"

ODF_SO_ALL =
ODF_OBJECTS_ALL =

MOCKS := $(wildcard tests/mocks/*.c)
MOCK_OBJECTS := $(MOCKS:.c=.o)
TEST_TOOLS := $(wildcard tests/lib/*.c)
TEST_TOOL_OBJECTS := $(TEST_TOOLS:.c=.o)

TESTS =
REQUIREMENTS =

include src/odf/sys/module.mk
include src/odf/math/module.mk
include src/odf/res/module.mk
include src/odf/ogl/module.mk

TESTS += $(wildcard tests/test_*.c)
TEST_OBJECTS = $(TESTS:.c=.o)
TEST_OUTPUTS = $(TESTS:.c=.out)

SOURCES := $(ODF_SOURCES) $(ODF_SYS_SOURCES) $(ODF_OGL_SOURCES) $(ODF_RES_SOURCES)
OBJECTS := $(SOURCES:.c=.o)
MAIN_EXEC := dark.o
LIB_OBJECTS := $(filter-out $(MAIN_OBJECT),$(OBJECTS))

TOOLS := $(wildcard tools/*.c)
TOOL_OBJECTS := $(TOOLS:.c=.o)
TOOL_OUTPUTS := $(TOOLS:.c=.out)


depend: .depend

.depend: $(SOURCES)
	rm -f "$@"
	$(CC) $(CFLAGS) -MM $^ >> "$@"

include .depend

test: $(TEST_OUTPUTS)

assemble: $(ODF_MAIN)


$(ODF_MAIN): $(ODF_OBJECTS) | $(ODF_SO_ALL)
	$(LD) $(LINK_ARGS) $(REQUIREMENTS)

compile: $(OBJECTS)

assemble-tools: $(TOOL_OUTPUTS)

tools/%.out: tools/%.o | $(ODF_SO_ALL)
	$(LD) $(LINK_ARGS) $(REQUIREMENTS)

compile-tools: $(TOOL_OBJECTS)


tests/test_%.out: tests/test_%.o $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS) | $(ODF_SO_ALL)
	$(LD) $(LINK_ARGS)
	$(MC) $(MCFLAGS) ./$@

clean:
	rm -f $(ODF_SO_ALL) $(TEST_OUTPUTS) $(TEST_OBJECTS) $(ODF_OBJECTS_ALL) $(TOOL_OUTPUTS) $(TOOL_OBJECTS) .depend

.PHONY: test clean compile
.PRECIOUS: $(OBJECTS) $(TEST_OBJECTS) $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS) $(TEST_OUTPUTS)
