#include "huffman.h"
#include "ffcodec.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void err() {
    fprintf(stderr, "Correct usage:\nffman.exe -c <input file> <output file>\n"
           "ffman.exe -d <input file> <output file>\n"
           "ffman.exe -v <input file>\n");
}

int main(int argc, char* argv[]) {
    if(argc != 3 && argc != 4) {
        err();
        return 1;
    }
    int opt=0, idx=0, num=0;
    char* inputFileName = NULL;
    char* outputFileName = NULL;
    char ch, table[ALEN][ALEN] = {0};
    uint32_t frequencies[ALEN] = {0};
    FILE* inputFile, *outputFile;
    uint8_t pad;
    Tree htree;
    if(!strcmp(argv[1], "-c")) opt=1;
    else if(!strcmp(argv[1], "-d")) opt=2;
    else if(!strcmp(argv[1], "-v")) opt=3;
    else {err(); return 1;}
    inputFileName = argv[2];
    if(opt != 3)
        outputFileName = argv[3];
    switch(opt) {
        case 1:
            if(!(inputFile = fopen(inputFileName, "r"))) {
               err(); 
               return 1;
            }if(!(outputFile = fopen(outputFileName, "wb"))) {
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
            fclose(inputFile);
            fclose(outputFile);
            break;

        case 2:
            if(!(inputFile = fopen(inputFileName, "rb"))) {
                err();
                return 1;
            }
            if(!(outputFile = fopen(outputFileName, "w"))) {
                err();
                return 1;
            }

            fread(&pad, sizeof(uint8_t), 1, inputFile);

            if(!checkMagicFF(inputFile)) {
                fprintf(stderr, "\nERROR: file magic number doesn't match");
                return 1;
            }
            for(int i=0; i<ALEN; ++i) {
                uint32_t val;
                fread(&val, sizeof(uint32_t), 1, inputFile);
                frequencies[i] = val;
            }

            Tree htree = huffmanTree(frequencies, table);
            ffdecode(inputFile, outputFile, htree, pad, maxCode(table));
            printf("\nFile %s decompressed properly", inputFileName);
            fclose(inputFile);
            fclose(outputFile);

            break;

        case 3:
            if(!(inputFile = fopen(inputFileName, "r"))) {
               err(); 
               return 1;
            }
            printf("Information for %s", inputFileName);
            printf("\nMetadata information:");
            printf("\n |- Fixed metadata block size: 136 bytes");

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
            printf("\n |- Compression ratio:"); 
            printf("\n\n'Information: the negative reciprocal value of probability.'");
            printf("\n-Claude E. Shannon.");
            fclose(inputFile);
            break;
        default:
            err();
            return 1;
    }
    return 0;
}
