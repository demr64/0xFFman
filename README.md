# 0xFFman
This project is about a file compressor/decompressor using the Huffman coding scheme in C.

### what is huffman coding and project information
Huffman coding is an algortihm that allows to compress data based on an alphabet's letter frequency, most used letters will have assigned an encoding value in bits that is small. The algorithm works by constructing a forest of binary trees, from which we gradually build the final tree, which will allow us to encode/decode files.
The program uses a magic number to check wether an input file was properly compressed by an encoder in the past.

### manual
This project uses a custom Makefile, use the command
```
make
```
in a terminal opened inside the folder, so that everything gets properly linked.
Here's the list of all commands:
```
ffman.exe -c
```
this command will compress the contents of "input.txt"
```
ffman.exe -d
```
this one will allow to decompress the contents of "compressed.bin"
there is also a command to visualize some statistics about the input.txt and compressed.bin
```
ffman.exe -v
```
the help command is
```
ffman.exe -h
```
