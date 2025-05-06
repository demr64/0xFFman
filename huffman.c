#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "huffman.h"

struct node {
    int value;
    char character;
    struct node* left;
    struct node* right;
};


Tree newTree(Tree a, Tree b, char chr, int freq) {
    Tree t = malloc(sizeof(struct node));
    t->value = freq;
    t->character = chr;
    t->left = a;
    t->right = b;
    return t;
}

//finds minimum root node in a forest
Tree minimumNode(Tree forest[], int n, int*pos) {
    int minima = 1000;
    Tree minNode;
    for(int i=0; i<n; ++i) {
        if(forest[i] == NULL) continue;
        if(forest[i]->value < minima) { 
            minima = forest[i]->value;
            minNode = forest[i];
            *pos = i;
        }
    }
    return minNode; 
}

Tree compress(Tree left, Tree right) {
    Tree parent = newTree(left, right, ' ', left->value+right->value);
    return parent;
}

//prints current forest status
void printForest(Tree f[]) {
    for(int i=0; i<ALEN; ++i) {
        if(f[i] == NULL) continue;
        printf("%d %c |", f[i]->value, f[i]->character);
    }
    printf("\n");
}

//creates forest, which is then compressed in a single tree
Tree huffmanTree(int f[], char table[][ALEN]) {
    Tree forest[ALEN];
    //initialization of tree
    for(int i=0; i<ALEN; i++) 
        forest[i] = newTree(NULL, NULL, (char)i, f[i]);
    
    //creating minma and finding
    Tree htree;
    Tree left, right;
    int *pos = malloc(sizeof(int));
    *pos = 0;
    for(int i=0; i<ALEN-1; ++i) {
        left = minimumNode(forest, ALEN, pos);
        forest[*pos] = NULL;
        right = minimumNode(forest, ALEN, pos);
        forest[*pos] = NULL;
        //compress
        forest[*pos] = compress(left, right);
    }
    //create the table of values to send
    char bits[ALEN];
    runEncoder(forest[*pos], 0, bits, table);
    htree = forest[*pos];
    return htree;
}

//the encoding algorithm searches for the leaf with same char
void runEncoder(Tree t, int depth, char bits[], char table[][ALEN]) {
    if(!t->left && !t->right) {
        bits[depth] = '\0';
        strcpy(table[(int)t->character], bits);
        return;
    }
    if(t->left != NULL)  {
        bits[depth] = '0';
        runEncoder(t->left, depth+1, bits, table);
    }
    if(t->right != NULL) {
        bits[depth] = '1';
        runEncoder(t->right, depth+1, bits, table);
    }
}
//main encoder function
char* encode(char* text, char table[][ALEN]) {
    static char encodedString[ALEN*ALEN];
    strcpy(encodedString, "");
    for(int i=0; i<strlen(text); i++) 
       strcat(encodedString, table[(int)text[i]]); 
    return encodedString;
}

char runDecoder(Tree t, char *text, int* i) {
    if(!t->left && !t->right) 
        return t->character;
    if(text[*i] == '0') {
        (*i)++;
        return runDecoder(t->left, text, i);
    }
    else if(text[*i] == '1') {
        (*i)++;
        return runDecoder(t->right, text, i);
    }
}

char* decode(Tree t, char* text) {
    int* i= malloc(sizeof(int));
    *i = 0;
    int j=0;
    static char decodedString[ALEN*ALEN];
    while(text[*i] == '1' || text[*i] == '0') {
        decodedString[j] = runDecoder(t, text,  i);
        j++;
    }
    return decodedString;
}
void adaptFreq(char* line, int frequencies[]) {
    char *ptr = line;
    while(*ptr != '\n') {
        frequencies[(int)*ptr]++;
        ptr++;
    }
    frequencies[(int)'\n']++;
}
double entropy(int frequencies[]) {
    double y=0.0;
    int total = 0;
    for(int i=0; i<ALEN; i++)
        total+=frequencies[i];
    for(int i=0; i<ALEN; ++i) 
        if(frequencies[i] != 0)
            y += -((double)frequencies[i]/total)*log2(((double)frequencies[i])/total);
    return y;
} 
void printTable(char table[][ALEN]) {
    for(int i=0; i<ALEN; ++i) 
        printf("%c: %s\n", i, table[i]);
}
 
