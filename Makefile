CC := gcc
CFLAGS := -std=c11 -fPIC -rdynamic -g -O2 -Wall -I include -I tests/include/
SOFLAGS := -shared
LIBS := -lglfw -lGLEW -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
LD := gcc
LDFLAGS := -L .

MC := valgrind
MCFLAGS := --leak-check=full --suppressions=./valgrind.supp --error-exitcode=1 -q

ODF_RES_SOURCES := $(wildcard src/odf/res/*.c)
ODF_RES_OBJECTS := $(ODF_RES_SOURCES:.c=.o)
ODF_OGL_SOURCES := $(wildcard src/odf/ogl/*.c)
ODF_OGL_OBJECTS := $(ODF_OGL_SOURCES:.c=.o)
ODF_SYS_SOURCES := $(wildcard src/odf/sys/*.c)
ODF_SYS_OBJECTS := $(ODF_SYS_SOURCES:.c=.o)

ODF_SOURCES := $(wildcard src/odf/*.c)
ODF_OBJECTS := $(ODF_SOURCES:.c=.o)
ODF_MAIN := "bin/dark"

ODF_RES_SO := lib/libodf-res.so
ODF_OGL_SO := lib/libodf-ogl.so
ODF_SYS_SO := lib/libodf-sys.so
ODF_SO_ALL := $(ODF_SYS_SO) $(ODF_RES_SO) $(ODF_OGL_SO)

SOURCES := $(ODF_SOURCES) $(ODF_SYS_SOURCES) $(ODF_OGL_SOURCES) $(ODF_RES_SOURCES)
OBJECTS := $(SOURCES:.c=.o)
MAIN_EXEC := dark.o
LIB_OBJECTS := $(filter-out $(MAIN_OBJECT),$(OBJECTS))

TESTS := $(wildcard tests/test_*.c)
TEST_OBJECTS := $(TESTS:.c=.o)
TEST_OUTPUTS := $(TESTS:.c=.out)

TOOLS := $(wildcard tools/*.c)
TOOL_OBJECTS := $(TOOLS:.c=.o)
TOOL_OUTPUTS := $(TOOLS:.c=.out)

MOCKS := $(wildcard tests/mocks/*.c)
MOCK_OBJECTS := $(MOCKS:.c=.o)
TEST_TOOLS := $(wildcard tests/lib/*.c)
TEST_TOOL_OBJECTS := $(TEST_TOOLS:.c=.o)

depend: .depend

.depend: $(SOURCES)
	rm -f "$@"
	$(CC) $(CFLAGS) -MM $^ >> "$@"

include .depend

test: $(TEST_OUTPUTS)

assemble: $(ODF_MAIN)

LINK_ARGS = -o $@ $(LDFLAGS) $(addprefix -l:,$|) $^

$(ODF_MAIN): $(ODF_OBJECTS) | $(ODF_SO_ALL)
	$(LD) $(LINK_ARGS) -lm

$(ODF_SYS_SO): $(ODF_SYS_OBJECTS)
	$(CC) $(SOFLAGS) $(LINK_ARGS)

$(ODF_RES_SO): $(ODF_RES_OBJECTS) | $(ODF_SYS_SO)
	$(CC) $(SOFLAGS) $(LINK_ARGS)

$(ODF_OGL_SO): $(ODF_OGL_OBJECTS) | $(ODF_SYS_SO)
	$(CC) $(SOFLAGS) $(LIBS) $(LINK_ARGS)

compile: $(OBJECTS)

assemble-tools: $(TOOL_OUTPUTS)

tools/%.out: tools/%.o | $(ODF_SO_ALL)
	$(LD) $(LINK_ARGS)

compile-tools: $(TOOL_OBJECTS)


tests/test_%.out: tests/test_%.o $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS) | $(ODF_SYS_SO) $(ODF_RES_SO)
	$(LD) $(LINK_ARGS)
	$(MC) $(MCFLAGS) ./$@

clean:
	rm -f $(TEST_OUTPUTS) $(TEST_OBJECTS) $(OBJECTS) $(TOOL_OUTPUTS) $(TOOL_OBJECTS) .depend

.PHONY: test clean compile
.PRECIOUS: $(OBJECTS) $(TEST_OBJECTS) $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS) $(TEST_OUTPUTS)
