ODF_SYS_SOURCES := $(wildcard src/odf/sys/*.c)
ODF_SYS_OBJECTS := $(ODF_SYS_SOURCES:.c=.o)

ODF_SYS_SO := lib/libodf-sys.so

ODF_SO_ALL += $(ODF_SYS_SO)

$(ODF_SYS_SO): $(ODF_SYS_OBJECTS)
	$(CC) $(SOFLAGS) $(LINK_ARGS)


TESTS += $(wildcard tests/odf/sys/test_*.c)

tests/odf/sys/test_%.out: tests/odf/sys/test_%.o $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS) | $(ODF_SO_ALL)
	$(LD) $(LINK_ARGS)
	$(MC) $(MCFLAGS) ./$@
