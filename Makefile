CC = gcc
CCFLAGS = -Wall -Wextra  -Wc++-compat
ASM = nasm
ASMFLAGS = -felf64 -l listing.list
OBJS_ASM = main.c quadratic.o
OBJ_NAME_ASM = quadratic

LINKER_FLAGS =  -L/usr/lib/x86_64-linux-gnu -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2_ttf -lm -D_REENTRANT -I/usr/include/SDL2



asm: $(OBJS_ASM)
		$(CC) $(OBJS_ASM) $(CCFLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME_ASM)

quadratic.o: quadratic.s
		$(ASM) $(ASMFLAGS) quadratic.s

clean:
		rm -f *.o $(OBJ_NAME_ASM) listing.list
