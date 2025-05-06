link: huffman.o main.o
	gcc huffman.o main.o -o ffman.exe

main.o:
	gcc -c main.c

huffman.o:
	gcc -c huffman.c

clean:
	rm -f *.exe *.o
