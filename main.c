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

int main(int argc, char* argv[]) {
    if(argc != 2) {err(); return 1;}
    int opt;
    char* inputFileName = NULL;
    char* outputFileName = NULL;
    char line[BLEN];
    char table[ALEN][ALEN];
    uint32_t frequencies[ALEN] = {0};
    FILE* inputFile;
    FILE* outputFile;
    uint8_t pad;
    int idx=0;
    int num;
    Tree htree;
    while((opt = getopt(argc, argv, "cdvh")) != -1) {
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
                while(fgets(line, BLEN, inputFile)) 
                   adaptFreq(line, frequencies); 

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
                ffdecode(inputFile, outputFile, htree, pad);
                printf("\nFile %s decompressed properly", inputFileName);
                break;


            case 'v':
                inputFileName = "input.txt"; 
                if((inputFile = fopen(inputFileName, "r")) == NULL) {
                   err(); 
                   return 1;
                }
                inputFileName = "input.txt";
                printf("Information for %s\n", inputFileName);
                printf("Metadata information:");
                printf("\n |- Fixed pad number: 1 byte");
                printf("\n |- Fixed magic number: 7 bytes");
                printf("\n |- Fixed length of frequency array: 128 bytes");

                while(fgets(line, BLEN, inputFile))
                    adaptFreq(line, frequencies);
            
                for(int i=0; i<ALEN; i++)
                    if(frequencies[i] > 0)
                        num++;
                printf("\n\nFile data information:");
                printf("\n |- Theoretical minimum average bits (entropy):"
                        " %f", entropy(frequencies));
                printf("\n |- Number of distinct ASCII characters: %d", num);
                htree = huffmanTree(frequencies, table);
                printf("\nCodes:\n");
                for(int i=0; i<ALEN; ++i)
                    if(frequencies[i] > 0)
                        printf("|- %c: %s\n", (char)i,encode((char*)&i, table));
                break;


            case 'h':
                printf("\n'Information: the negative reciprocal value of probability.'");
                printf("\n-Claude E. Shannon.");
                break;
            default:
                return 1;
        }
    }
    return 0;
}
