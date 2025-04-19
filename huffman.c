#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define ALEN 26 

typedef struct node* Tree;

struct node {
    int value;
    char character;
    struct node* left;
    struct node* right;
};
int frequencies[ALEN] = {
    8,  
    2,  
    3,  
    4,  
    13, 
    2,  
    2,  
    6,  
    7,  
    1,  
    1,  
    4,  
    2,  
    7,  
    8, 
    2, 
    1, 
    6,  
    6,
    9,
    3, 
    1,
    2,
    1, 
    2, 
    1   
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
Tree huffmanTree(int f[]) {
    Tree forest[ALEN];
    //initialization of tree
    for(int i=0; i<ALEN; i++) {
        forest[i] = newTree(NULL, NULL, 'a'+i, f[i]);
    }
    
    //creating minma and finding
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
    return forest[*pos];
}

//the encoding algorithm searches for the leaf with same char
void runEncoder(Tree t, int depth, char bits[], char table[][ALEN]) {
    if(!t->left && !t->right) {
        bits[depth] = '\0';
        strcpy(table[t->character-'a'], bits);
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

void encode(Tree t, char table[][ALEN]) {
    char bits[ALEN];
    runEncoder(t, 0, bits, table);
}

void printTable(char table[][ALEN]) {
    for(int i=0; i<ALEN; ++i) 
        printf("%c: %s\n", 'a'+i, table[i]);
}
 
    

int main(void) {
    char table[ALEN][ALEN];
    Tree huffmanAlphabet = huffmanTree(frequencies);
    //encoding function
    encode(huffmanAlphabet, table);

    printTable(table);
    return 0;
}
