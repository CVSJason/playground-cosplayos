OBJECTS = api.o

libcosp.a: api.asm $(OBJECTS)
	ar rcs libcosp.a $(OBJECTS)

api.o: api.asm
	nasm api.asm -f elf32 -o api.o