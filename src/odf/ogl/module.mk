ODF_OGL_SOURCES := $(wildcard src/odf/ogl/*.c)
ODF_OGL_OBJECTS := $(ODF_OGL_SOURCES:.c=.o)

ODF_OGL_SO := lib/libodf-ogl.so

ODF_SO_ALL += $(ODF_OGL_SO)

$(ODF_OGL_SO): $(ODF_OGL_OBJECTS) | $(ODF_SYS_SO)
	$(CC) $(SOFLAGS) $(LIBS) $(LINK_ARGS)
