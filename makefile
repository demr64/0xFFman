link: huffman.o main.o ffcodec.o
	gcc huffman.o main.o ffcodec.o -o ffman.exe

main.o:
	gcc -c main.c

huffman.o:
	gcc -c huffman.c

ffcodec.o:
	gcc -c ffcodec.c

clean:
	rm -f *.exe *.o
