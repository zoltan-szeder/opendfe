ODF_CTRL_GLFW_SOURCES := $(wildcard src/odf/ctrl/glfw/*.c)
ODF_CTRL_GLFW_OBJECTS := $(ODF_CTRL_GLFW_SOURCES:.c=.o)

ODF_CTRL_GLFW_SO := lib/libodf-ctrl-glfw.so

ODF_OBJECTS_ALL += $(ODF_CTRL_GLFW_OBJECTS)
ODF_SO_ALL += $(ODF_CTRL_GLFW_SO)

$(ODF_CTRL_GLFW_SO): $(ODF_CTRL_GLFW_OBJECTS) | $(ODF_SYS_SO) $(ODF_CTRL_SO)
	$(CC) $(SOFLAGS) $(LINK_ARGS)


ODF_CTRL_GLFW_SO_DEPS = $(ODF_CTRL_GLFW_SO) $(ODF_SYS_SO_DEPS) $(ODF_CTRL_SO_DEPS)
TESTS += $(wildcard tests/odf/ctrl/glfw/test_*.c)

tests/odf/ctrl/glfw/test_%.out: tests/odf/ctrl/glfw/test_%.o $(TEST_TOOL_OBJECTS) | $(ODF_CTRL_GLFW_SO_DEPS)
	$(LD) -lcmocka -lglfw $(LINK_ARGS)
	$(MC) $(MCFLAGS) ./$@
