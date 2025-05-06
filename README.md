# 0xFFman
This project is about a file compressor/decompressor using the huffman coding scheme in C.

### what is huffman coding and project information
Huffman coding is an algortihm that allows to compress data based on an alphabet's letter frequency, most used letters will have assigned an encoding value in bits that is small. The algorithm works by constructing a forest of binary trees, from which we gradually build the final tree, which will allow us to encode/decode files.
```
huffman.exe -c
```
This command will compress the contents of "input.txt"
```
huffman.exe -d
```
This one will allow to decompress the contents of "compressed.bin"
there is also a command to visualize some statistics about the input.txt and compressed.bin
```
huffman.exe -v
```
