#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define FREQLEN 26
#define ALEN 26 

typedef struct node* Tree;

struct node {
    int value;
    char character;
    struct node* left;
    struct node* right;
};
int frequencies[26] = {
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
}

Tree minimumNode(Tree forest[], int n, int*pos) {
    int minima = 105;
    Tree minNode = NULL;
    for(int i=0; i<n; ++i) {
        if(forest[i] == NULL) continue;
        if(forest[i]->value < minima) { 
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

void printForest(Tree f[]) {
    for(int i=0; i<ALEN; ++i) {
        if(f[i] == NULL) continue;
        printf("%d %c |", f[i]->value, f[i]->character);
    }
}

Tree huffmanTree(int f[]) {
    Tree forest[ALEN];
    //initialization of tree
    for(int i=0; i<ALEN; i++) {
        forest[i] = newTree(NULL, NULL, 'a'+i, f[i]);
    }
    
    //creating minma and finding
    int *pos = malloc(sizeof(int));
    for(int i=0; i<ALEN-1; ++i) {
        Tree left = minimumNode(forest, ALEN, pos);
        forest[*pos] = NULL;
            
        Tree right = minimumNode(forest, ALEN, pos);
        forest[*pos] = NULL;
        //compress the two nodes in one
        forest[*pos] = compress(left, right);
    }
    return forest[*pos];
}
/*
void encode(Tree t, char string[], char table[]) {
    if(t->left == NULL && t->right == NULL) 
        strcpy(table[t->character-'a'], string);
}
*/

int main(void) {
    Tree huffmanAlphabet = huffmanTree(frequencies);
    return 0;
}
