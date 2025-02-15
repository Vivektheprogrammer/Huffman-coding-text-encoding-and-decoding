#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_TREE_NODES 512
#define MAX_CHAR 256

typedef struct Node {
    char ch;
    int freq;
    struct Node *left, *right;
} Node;

typedef struct {
    Node *array[MAX_TREE_NODES];
    int size;
} MinHeap;

Node *createNode(char ch, int freq, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

void insertMinHeap(MinHeap *heap, Node *node) {
    int i = heap->size++;
    while (i && node->freq < heap->array[(i - 1) / 2]->freq) {
        heap->array[i] = heap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    heap->array[i] = node;
}

Node *extractMin(MinHeap *heap) {
    Node *top = heap->array[0];
    heap->array[0] = heap->array[--heap->size];
    int i = 0, min, left, right;
    while ((left = 2 * i + 1) < heap->size) {
        right = left + 1;
        min = (right < heap->size && heap->array[right]->freq < heap->array[left]->freq) ? right : left;
        if (heap->array[i]->freq <= heap->array[min]->freq) break;
        Node *temp = heap->array[i];
        heap->array[i] = heap->array[min];
        heap->array[min] = temp;
        i = min;
    }
    return top;
}

MinHeap *buildMinHeap(char data[], int freq[], int size) {
    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    heap->size = 0;
    for (int i = 0; i < size; i++)
        insertMinHeap(heap, createNode(data[i], freq[i], NULL, NULL));
    return heap;
}

Node *buildHuffmanTree(char data[], int freq[], int size) {
    MinHeap *heap = buildMinHeap(data, freq, size);
    while (heap->size > 1) {
        Node *left = extractMin(heap);
        Node *right = extractMin(heap);
        insertMinHeap(heap, createNode('\0', left->freq + right->freq, left, right));
    }
    return extractMin(heap);
}

void generateHuffmanCodes(Node *root, char *code, int depth, char huffmanCodes[MAX_CHAR][MAX_CHAR]) {
    if (!root) return;
    if (!root->left && !root->right) {
        code[depth] = '\0';
        strcpy(huffmanCodes[(unsigned char)root->ch], code);
    }
    code[depth] = '0';
    generateHuffmanCodes(root->left, code, depth + 1, huffmanCodes);
    code[depth] = '1';
    generateHuffmanCodes(root->right, code, depth + 1, huffmanCodes);
}

void writeTree(Node *root, FILE *output) {
    if (!root) return;
    if (!root->left && !root->right) {
        fputc('L', output);
        fputc(root->ch, output);
    } else {
        fputc('I', output);
        writeTree(root->left, output);
        writeTree(root->right, output);
    }
}

void compressFile(const char *inputFile, const char *outputFile) {
    FILE *input = fopen(inputFile, "r");
    FILE *output = fopen(outputFile, "wb");
    if (!input || !output) {
        printf("Error opening files!\n");
        return;
    }

    int freq[MAX_CHAR] = {0};
    char ch;
    while ((ch = fgetc(input)) != EOF) {
        freq[(unsigned char)ch]++;
    }

    char data[MAX_CHAR];
    int freqArr[MAX_CHAR], size = 0;
    for (int i = 0; i < MAX_CHAR; i++) {
        if (freq[i]) {
            data[size] = (char)i;
            freqArr[size] = freq[i];
            size++;
        }
    }
    
    Node *root = buildHuffmanTree(data, freqArr, size);
    char huffmanCodes[MAX_CHAR][MAX_CHAR] = {{0}};
    char code[MAX_CHAR];
    generateHuffmanCodes(root, code, 0, huffmanCodes);
    
    fseek(input, 0, SEEK_SET);
    writeTree(root, output);
    fputc('\n', output);

    uint8_t buffer = 0;
    int bitCount = 0;
    while ((ch = fgetc(input)) != EOF) {
        char *code = huffmanCodes[(unsigned char)ch];
        for (int i = 0; code[i] != '\0'; i++) {
            buffer = (buffer << 1) | (code[i] - '0');
            bitCount++;
            if (bitCount == 8) {
                fputc(buffer, output);
                buffer = 0;
                bitCount = 0;
            }
        }
    }
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        fputc(buffer, output);
    }
    
    fclose(input);
    fclose(output);
    printf("File compressed successfully.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }
    compressFile(argv[1], argv[2]);
    return 0;
}
