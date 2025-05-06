#include "huffman.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BLEN 1024


void err() {
    fprintf(stderr, "\nUsage:\nffman.exe -e <input file> <output file>\n"
           "ffman.exe -d <input file> <output file>\n");
}


int main(int argc, char* argv[]) {
    int opt;
    char* inputFileName = NULL;
    char* outputFileName = NULL;
    char line[BLEN];
    char table[ALEN][ALEN];
    uint32_t frequencies[ALEN] = {0};
    Tree htree;
    while((opt = getopt(argc, argv, "cdvh")) != -1) {
        FILE* inputFile;
        FILE* outputFile;
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
                //create tree and table
                while(fgets(line, BLEN, inputFile)) 
                   adaptFreq(line, frequencies); 

                htree = huffmanTree(frequencies, table);
                rewind(inputFile);
                //write magic number
                fwrite("0xFFMAN", 1, 7, outputFile);
                for(int i=0; i<ALEN; ++i) {
                    uint32_t val = frequencies[i];
                    fwrite(&val, sizeof(uint32_t), 1, outputFile);
                }
                fprintf(outputFile, "\n"); 
                while(fgets(line, BLEN, inputFile)) {
                    fprintf(outputFile, encode(line, table));
                }
                printf("File %s compressed properly", inputFileName);
                break;
            case 'd':
                //check magic  
                inputFileName = "compressed.bin";
                if((inputFile = fopen(inputFileName, "rb")) == NULL) {
                    err();
                    return 1;
                }
                //magic number file format
                uint8_t magic[] = {'0', 'x', 'F','F','M','A','N'};

                for(int i=0;i<7; ++i) {
                    uint8_t val = fgetc(inputFile);
                    if(magic[i] != val) {
                        fprintf(stderr, "\nERROR: file magic doesn't match.");
                        return 1;
                    }
                } 
                for(int i=0; i<ALEN; ++i) {
                    uint32_t val;
                    fread(&val, sizeof(uint32_t), 1, inputFile);
                    frequencies[i] = val;
                }
                Tree htree = huffmanTree(frequencies, table);
                while(fgets(line, BLEN, inputFile)) 
                    printf("%s", decode(htree, line));
                printf("File %s decompressed properly", inputFileName);
                break;
            case 'v':
                inputFileName = "input.txt"; 
                if((inputFile = fopen(inputFileName, "r")) == NULL) {
                   err(); 
                   return 1;
                }
                while(fgets(line, BLEN, inputFile)) 
                    adaptFreq(line, frequencies);
                printf("Shannon's entropy of input.txt: %f", entropy(frequencies));
                break;
            case 'h':
                printf("0xFFman github page: ");
                printf("\nhttps://github.com/demr64/0xFFman/blob/main/README.md");
                break;
            default:
                err();
                return 1;
            
        }
    }
    return 0;
}
