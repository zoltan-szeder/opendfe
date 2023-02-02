ODF_CTRL_SOURCES := $(wildcard src/odf/ctrl/*.c)
ODF_CTRL_OBJECTS := $(ODF_CTRL_SOURCES:.c=.o)

ODF_CTRL_SO := lib/libodf-ctrl.so

ODF_OBJECTS_ALL += $(ODF_CTRL_OBJECTS)
ODF_SO_ALL += $(ODF_CTRL_SO)

$(ODF_CTRL_SO): $(ODF_CTRL_OBJECTS)
	$(CC) $(SOFLAGS) $(LINK_ARGS)


ODF_CTRL_SO_DEPS = $(ODF_CTRL_SO)
TESTS += $(wildcard tests/odf/ctrl/test_*.c)

tests/odf/ctrl/test_%.out: tests/odf/ctrl/test_%.o $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS) | $(ODF_CTRL_SO_DEPS)
	$(LD) $(LINK_ARGS)
	$(MC) $(MCFLAGS) ./$@
