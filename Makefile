CC = gcc
CFLAGS = -rdynamic -g -O2 -Wall -I include -I tests/include/
LIBS = -lglfw -lGLEW -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
LD = gcc
LDFLAGS = 

MC = valgrind
MCFLAGS = --leak-check=full --error-exitcode=1 -q

SOURCES = $(wildcard src/**/*.c) $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

TESTS = $(wildcard tests/test_*.c)
TEST_OBJECTS = $(TESTS:.c=.o)
TEST_OUTPUTS = $(TESTS:.c=.out)

TOOLS = $(wildcard tools/*.c)
TOOL_OBJECTS = $(TOOLS:.c=.o)
TOOL_OUTPUTS = $(TOOLS:.c=.out)

MOCKS = $(wildcard tests/mocks/*.c)
MOCK_OBJECTS = $(MOCKS:.c=.o)
TEST_TOOLS = $(wildcard tests/lib/*.c)
TEST_TOOL_OBJECTS = $(TEST_TOOLS:.c=.o)

test: $(TEST_OUTPUTS)

assemble: compile
	$(LD) -o dark.out $(OBJECTS) $(LDFLAGS) $(LIBS)

compile: $(OBJECTS)

assemble-tools: $(TOOL_OUTPUTS)

tools/%.out: $(OBJECTS) $(TOOL_OBJECTS)
	$(LD) -o $@ $(filter-out src/dark.o,$^) $(LDFLAGS) $(LIBS)

compile-tools: $(TOOL_OBJECTS)


tests/test_%.out: src/%.o tests/test_%.o $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS)
	$(LD) -o $@ $(filter-out tests/mocks/mock_$(subst src/,,$<),$^) $(LDFLAGS) $(LIBS)
	$(MC) $(MCFLAGS) ./$@

clean:
	rm -f $(TEST_OUTPUTS) $(TEST_OBJECTS) $(OBJECTS)

.PHONY: test clean compile
.PRECIOUS: $(OBJECTS) $(TEST_OBJECTS) $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS)
