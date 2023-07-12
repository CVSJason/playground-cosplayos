noodle: app.o ../api/libcosp.a
	ld -T ../app.ld -o noodle app.o -L../api -L../../golibc -lcosp -lgolibc

app.o: app.cpp
	clang++ app.cpp -o app.o -target x86_64-pc-none-gnu -m32 -std=c++17 -c -I ../../golibc

../api/libcosp.a: ../api/makefile
	make -f makefile -C ../api/