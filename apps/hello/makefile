hello: app.o ../api/libcosp.a
	ld -T ../app.ld -o hello app.o -L../api -lcosp

app.o: app.asm
	nasm app.asm -f elf32 -o app.o

../api/libcosp.a: ../api/makefile
	make -f makefile -C ../api/