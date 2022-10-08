ODF_RES_SOURCES := $(wildcard src/odf/res/*.c)
ODF_RES_OBJECTS := $(ODF_RES_SOURCES:.c=.o)

ODF_RES_SO := lib/libodf-res.so

ODF_SO_ALL += $(ODF_RES_SO)

$(ODF_RES_SO): $(ODF_RES_OBJECTS) | $(ODF_SYS_SO)
	$(CC) $(SOFLAGS) $(LINK_ARGS)



TESTS += $(wildcard tests/odf/res/test_*.c)

tests/odf/res/test_%.out: tests/odf/res/test_%.o $(MOCK_OBJECTS) $(TEST_TOOL_OBJECTS) | $(ODF_SO_ALL)
	$(LD) $(LINK_ARGS)
	$(MC) $(MCFLAGS) ./$@
