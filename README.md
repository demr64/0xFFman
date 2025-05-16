# 0x7Fman
This project is about a file compressor/decompressor using the Huffman coding scheme in C.

### what is huffman coding and project information
Huffman coding is an algorithm that allows to compress data based on an alphabet's letter frequency, most used letters will have assigned an encoding value in bits that is small. 
The algorithm works by constructing a forest of binary trees, from which we gradually buil
the final tree, which will allow us to encode/decode files.
The program uses a magic number to check wether an input file was properly compressed by
an encoder in the past. This project only supports ASCII character texts.
A codec named ffcodec is used along the main.c to parse bitsreams in I/O files.
The project name comes from the hexadecimal 0x7F which is 127 in decimal, representing
the maximum ascii character that is in the ascii table when using C.

### manual
This project uses a custom Makefile, use the command
```
make
```
in a terminal opened inside the folder, so that everything gets properly linked.
Your input files shall use a .txt format and outputfiles in .bin, or equivalent format.
First create your input file, your output files will be created if not existent.
Here's the list of all commands:
```
ffman.exe -c <input file> <output file>
```
this command will compress the contents of "input.txt"
```
ffman.exe -d <input file> <output file>
```
this one will allow to decompress the contents of "compressed.bin"
there is also a command to visualize some statistics about the input.txt and compressed.bin files
```
ffman.exe -v <input file> 
```
