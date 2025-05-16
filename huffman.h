typedef struct node* Tree;
#define ALEN 128

Tree newTree(Tree a, Tree b, char chr, int freq);
Tree minimumNode(Tree forest[], int n, int*pos);
Tree compress(Tree left, Tree right);
void printForest(Tree f[]);
Tree huffmanTree(int f[], char table[][ALEN]);
char* encode(char* text, char table[][ALEN]);
void runEncoder(Tree t, int depth, char bits[], char table[][ALEN]);
char* decode(Tree t, char* text, int*changedBits, int pad);
int runDecoder(Tree t, char *text, int* i);
double entropy(int frequencies[]);
void printTable(char table[][ALEN]);
void adaptFreq(char, int frequencies[]);
int maxCode(char table[][ALEN]);
