ODF_SYS_SOURCES := $(wildcard src/odf/sys/*.c)
ODF_SYS_OBJECTS := $(ODF_SYS_SOURCES:.c=.o)

ODF_SYS_SO := lib/libodf-sys.so

ODF_OBJECTS_ALL += $(ODF_SYS_OBJECTS)
ODF_SO_ALL += $(ODF_SYS_SO)

$(ODF_SYS_SO): $(ODF_SYS_OBJECTS)
	mkdir -p lib
	$(CC) $(SOFLAGS) $(LINK_ARGS)


ODF_SYS_SO_DEPS = $(ODF_SYS_SO)
TESTS += $(wildcard tests/odf/sys/test_*.c)

tests/odf/sys/test_%.out: tests/odf/sys/test_%.o $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS) | $(ODF_SYS_SO_DEPS)
	$(LD) -lcmocka $(LINK_ARGS)
	$(MC) $(MCFLAGS) ./$@
