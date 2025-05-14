#include "huffman.h"
#include "ffcodec.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void err() {
    fprintf(stderr, "Correct usage:\nffman.exe -e <input file> <output file>\n"
           "ffman.exe -d <input file> <output file>\n");
    fprintf(stderr, "Did you mean something? do ffman.exe -h for help");
}

int maxCode(char table[][ALEN]) {
    int max = 0;
    for(int i=0; i<ALEN; ++i) 
        if(strlen(table[i]) > max) 
            max = strlen(table[i]);
    return max;
}

int main(int argc, char* argv[]) {
    if(argc != 2) {err(); return 1;}
    int opt, idx=0, num=0;
    char* inputFileName = NULL;
    char* outputFileName = NULL;
    char ch, table[ALEN][ALEN] = {0};
    uint32_t frequencies[ALEN] = {0};
    FILE* inputFile, *outputFile;
    uint8_t pad;
    Tree htree;
    while((opt = getopt(argc, argv, "cdv")) != -1) {
        switch(opt) {
            case 'c':
                inputFileName = "input.txt";
                outputFileName = "compressed.bin";
                if((inputFile = fopen(inputFileName, "r")) == NULL) {
                   err(); 
                   return 1;
                }if((outputFile = fopen(outputFileName, "wb")) == NULL) {
                    err();
                    return 1;
                }
                while((ch = fgetc(inputFile)) != EOF)
                    adaptFreq(ch, frequencies);

                htree = huffmanTree(frequencies, table);
                rewind(inputFile);

                fwrite("-0xFFMAN", 1, 8, outputFile);
                writeMetadata(outputFile, frequencies);
                ffencode(inputFile, outputFile, table);
                printf("File %s compressed properly", inputFileName);
                break;


            case 'd':
                outputFileName = "output.txt";
                inputFileName = "compressed.bin";
                if((inputFile = fopen(inputFileName, "rb")) == NULL) {
                    err();
                    return 1;
                }
                if((outputFile = fopen(outputFileName, "w")) == NULL) {
                    err();
                    return 1;
                }

                fread(&pad, sizeof(uint8_t), 1, inputFile);

                if(!checkMagicFF(inputFile))
                    fprintf(stderr, "\nERROR: file magic number doesn't match");
                for(int i=0; i<ALEN; ++i) {
                    uint32_t val;
                    fread(&val, sizeof(uint32_t), 1, inputFile);
                    frequencies[i] = val;
                }

                Tree htree = huffmanTree(frequencies, table);
                ffdecode(inputFile, outputFile, htree, pad, maxCode(table));
                printf("\nFile %s decompressed properly", inputFileName);
                break;


            case 'v':
                inputFileName = "input.txt"; 
                if((inputFile = fopen(inputFileName, "r")) == NULL) {
                   err(); 
                   return 1;
                }
                inputFileName = "input.txt";
                printf("\nInformation for %s", inputFileName);
                printf("\nMetadata information:");
                printf("\n |- Metadata block size: 136 bytes");
                printf("\n |- Fixed pad number: 1 byte");
                printf("\n |- Fixed magic number: 7 bytes");
                printf("\n |- Fixed length of frequency array: 128 bytes");
                while((ch = fgetc(inputFile)) != EOF)
                    adaptFreq(ch, frequencies);
                for(int i=0; i<ALEN; i++)
                    if(frequencies[i] > 0)
                        num++;
                htree = huffmanTree(frequencies, table);
                printf("\n\nFile data information:");
                printf("\n |- Theoretical minimum average bits (entropy):"
                        " %f", entropy(frequencies));
                printf("\n |- Number of distinct ASCII characters: %d", num);
                printf("\n |- Maximum code length: %d", maxCode(table));
                printf("\n\n'Information: the negative reciprocal value of probability.'");
                printf("\n-Claude E. Shannon.");
                break;
            default:
                return 1;
        }
    }
    return 0;
}
