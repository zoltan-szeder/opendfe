ODF_MATH_SOURCES := $(wildcard src/odf/math/*.cpp)
ODF_MATH_OBJECTS := $(ODF_MATH_SOURCES:.cpp=.o)

ODF_MATH_REQUIREMENTS := -lm
ODF_MATH_SO := lib/libodf-math.so

ODF_OBJECTS_ALL += $(ODF_MATH_OBJECTS)
ODF_SO_ALL += $(ODF_MATH_SO)

$(ODF_MATH_SO): $(ODF_MATH_OBJECTS)
	$(CC) $(SOFLAGS) $(LINK_ARGS)


ODF_MATH_SO_DEPS = $(ODF_MATH_SO)
TESTS += $(wildcard tests/odf/math/test_*.c)
REQUIREMENTS += $(ODF_MATH_REQUIREMENTS)

tests/odf/math/test_%.out: tests/odf/math/test_%.o $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS) | $(ODF_MATH_SO_DEPS)
	$(LD) -lcmocka $(ODF_MATH_REQUIREMENTS) $(LINK_ARGS)
	$(MC) $(MCFLAGS) ./$@
