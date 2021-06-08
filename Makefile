# Makefile para Linux e macOS

PROG = seamcarving
FONTES = main.c lib/SOIL/SOIL.c lib/SOIL/image_DXT.c lib/SOIL/image_helper.c lib/SOIL/stb_image_aug.c 
OBJETOS = $(FONTES:.c=.o)
CFLAGS = -g -O3 -Iinclude -DGL_SILENCE_DEPRECATION # -Wall -g  # Todas as warnings, infos de debug

UNAME = `uname`

all: $(TARGET)
	-@make $(UNAME)

Darwin: $(OBJETOS)
	gcc $(OBJETOS) -O3 -Wno-deprecated -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)

Linux: $(OBJETOS)
	gcc $(OBJETOS) -O3 -lGL -lGLU -lglut -lm -o $(PROG)

clean:
	-@ rm -f $(OBJETOS) $(PROG)
