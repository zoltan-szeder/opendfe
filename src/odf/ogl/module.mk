ODF_OGL_SOURCES := $(wildcard src/odf/ogl/*.c)
ODF_OGL_OBJECTS := $(ODF_OGL_SOURCES:.c=.o)

ODF_OGL_REQUIREMENTS := -lglfw -lGLEW -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
ODF_OGL_SO := lib/libodf-ogl.so

ODF_OBJECTS_ALL += $(ODF_OGL_OBJECTS)
ODF_SO_ALL += $(ODF_OGL_SO)

$(ODF_OGL_SO): $(ODF_OGL_OBJECTS) | $(ODF_SYS_SO)
	$(CC) $(SOFLAGS) $(LIBS) $(LINK_ARGS)


ODF_OGL_SO_DEPS = $(ODF_OGL_SO)
REQUIREMENTS += $(ODF_OGL_REQUIREMENTS)
