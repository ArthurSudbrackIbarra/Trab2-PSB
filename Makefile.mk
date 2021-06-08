# Makefile para Windows

PROG = seamcarving.exe
FONTES = main.c lib/SOIL/SOIL.c lib/SOIL/image_DXT.c lib/SOIL/image_helper.c lib/SOIL/stb_image_aug.c 
OBJETOS = $(FONTES:.c=.o)
CFLAGS = -O3 -g -Iinclude # -Wall -g  # Todas as warnings, infos de debug
LDFLAGS = -Llib -lfreeglut -lopengl32 -lglu32 -lm
CC = gcc

$(PROG): $(OBJETOS)
	gcc $(CFLAGS) $(OBJETOS) -o $@ $(LDFLAGS)

clean:
	-@ del $(OBJETOS) $(PROG)
