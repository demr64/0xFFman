#include "huffman.h"
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#define BLEN 1024

void printBinary(char c);
void charToBinary(char c, char ans[]);
void writeMetadata(FILE* outputFile, uint32_t frequencies[]);
int checkMagicFF(FILE* inputFile);
int ffencode(FILE*, FILE*, char table[][ALEN]);
int ffdecode(FILE*, FILE*, Tree, uint8_t, int maxCode);
