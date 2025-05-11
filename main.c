#include "huffman.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BLEN 1024


void err() {
    fprintf(stderr, "Correct usage:\nffman.exe -e <input file> <output file>\n"
           "ffman.exe -d <input file> <output file>\n");
    fprintf(stderr, "Did you mean something? do ffman.exe -h for help");
}
void printBinary(char c) {
    for (int i = 7; i >= 0; i--) {
        putchar((c & (1 << i)) ? '1' : '0');
    }
    putchar('\n');
}

void charToBinary(char c, char ans[]) {
    for (int i = 7; i >= 0; i--) {
        ans[i] = (c >> i) & 1;
    }
    ans[8] = '\0';
}

void writeMetadata(FILE* outputFile, uint32_t frequencies[]) {
    for(int i=0; i<ALEN; ++i) {
        uint32_t val = frequencies[i];
        fwrite(&val, sizeof(uint32_t), 1, outputFile);
    }
}

int checkMagicFF(FILE* inputFile) {
    uint8_t magic[] = {'0', 'x', 'F','F','M','A','N'};
    uint8_t val;
    for(int i=0;i<7; ++i) {
        fread(&val, sizeof(uint8_t), 1, inputFile);
        if(magic[i] != val) 
            return 0;
    } 
    return 1;
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
    uint8_t padding;
    int idx=0;
    int num = 0;
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
                //create tree and table
                while(fgets(line, BLEN, inputFile)) 
                   adaptFreq(line, frequencies); 

                htree = huffmanTree(frequencies, table);
                rewind(inputFile);
                
                fwrite("-0xFFMAN", 1, 8, outputFile);
                writeMetadata(outputFile, frequencies);
                uint8_t byte = 0;
                int i =0;

                //TODO: FIX FGETS FOR FULL LINE
                while(fgets(line, BLEN, inputFile)) {
                    char* temp = encode(line, table);
                    for(i=0; i<strlen(temp); ++i) {
                        if((idx%8 == 0 && idx!= 0)) {
                            fwrite(&byte, sizeof(uint8_t), 1, outputFile);
                            byte = 0;
                        }
                        byte <<= 1;
                        if(temp[i] == '1') byte |= 1;
                        idx++;
                    }
                    free(temp);
                }
                padding = idx%8;
                if(padding != 0) {
                    byte <<= padding;
                    fwrite(&byte, sizeof(uint8_t), 1, outputFile);
                    rewind(outputFile);
                    fwrite((char*)&padding, 1, 1, outputFile);
                    printBinary(byte);
                }
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

                fread(&padding, sizeof(uint8_t), 1, inputFile);

                if(!checkMagicFF(inputFile))
                    fprintf(stderr, "\nERROR: file magic number doesn't match");

                for(int i=0; i<ALEN; ++i) {
                    uint32_t val;
                    fread(&val, sizeof(uint32_t), 1, inputFile);
                    frequencies[i] = val;
                }
                uint8_t flag = 0;
                char buffer[9] = {0};
                int next;
                int decoded=7;
                int j = 0;

                htree = huffmanTree(frequencies, table);
                while(fread(&byte, sizeof(uint8_t), 1, inputFile) == 1) {
                    next = getc(inputFile);
                    if(next == EOF) flag = 1;
                    ungetc(next, inputFile);

                    for(int i=0; i<8; ++i) {
                        int bit = (byte >> (7-i))&1;
                        buffer[idx++] = bit+'0';
                        if(idx == 8) {
                            if(flag) 
                                printf("%s", decode(htree, buffer, &decoded, padding));
                            else 
                                printf("%s", decode(htree, buffer, &decoded, 0));
                            if(decoded != 7) {
                                //must insert remaining chars in buffer 
                                for(int j=0; j<7-decoded; ++j)
                                    buffer[j] = buffer[7-(7-decoded)+1+j];
                                idx = 7-decoded;
                            }else idx = 0;
                            
                        }
                    }
                }
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
                printf("\n |- Fixed padding number: 1 byte");
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
