#include "ffcodec.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


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
int ffencode(FILE* inputFile, FILE* outputFile, char table[][ALEN]) {
    char line[2] = {'0', '\0'};
    uint8_t pad;
    int idx=0;
    uint8_t byte = 0;
    int i = 0;
    while((line[0] = fgetc(inputFile)) != EOF) {
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
    pad = 8-(idx%8);
    if(pad != 8) {
        byte <<= pad;
        fwrite(&byte, sizeof(uint8_t), 1, outputFile);
        rewind(outputFile);
        fwrite((char*)&pad, 1, 1, outputFile);
    }else {
        pad = 0;
        fwrite(&byte, sizeof(uint8_t), 1, outputFile);
        rewind(outputFile);
        fwrite((char*)&pad, 1, 1, outputFile);
    }
    return 1;
}
int ffdecode(FILE* inputFile, FILE* outputFile, Tree htree, uint8_t pad, int maxCode) {
    uint8_t byte = 0;
    int idx=0;
    uint8_t flag = 0;
    maxCode = maxCode <= 8 ? 8 : maxCode;
    char buffer[maxCode+1];
    memset(buffer, 0, (maxCode+1)*sizeof(char));
    int next;
    int decoded=8;
    char line[BLEN];
    while(fread(&byte, sizeof(uint8_t), 1, inputFile) == 1) {
        next = getc(inputFile);
        if(next == EOF) flag = 1;
        ungetc(next, inputFile);
        for(int i=0; i<8; ++i) {
            int bit = (byte >> (7-i))&1;
            buffer[idx++] = bit+'0';
            //if we are at a multiple of 8 decode?
            if(idx == maxCode) {
                if(flag) 
                    printf("%s", decode(htree, buffer, &decoded, pad));
                else
                    printf("%s", decode(htree, buffer, &decoded, 0));
                if(decoded < maxCode && decoded >=0) {
                    //must insert remaining chars in buffer 
                    for(int j=0; j<maxCode-1-decoded; ++j)
                        buffer[j] = buffer[maxCode-1-(maxCode-1-decoded)+1+j];
                    idx = maxCode-1-decoded;
                }else idx = 0;
            }
        }
    }
    return 1;
}
