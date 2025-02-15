#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_TREE_NODES 512

typedef struct Node {
    char ch;
    struct Node *left, *right;
} Node;

Node *createNode(char ch) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->ch = ch;
    node->left = node->right = NULL;
    return node;
}

// Read a single bit from the file
int readBit(FILE *file, uint8_t *buffer, int *bitPos) {
    if (*bitPos == 0) {
        if (fread(buffer, 1, 1, file) != 1)
            return -1; // End of file
        *bitPos = 8;
    }
    int bit = (*buffer >> (--(*bitPos))) & 1;
    return bit;
}

// Rebuild Huffman Tree from stored format
Node *rebuildHuffmanTree(FILE *file) {
    uint8_t flag;
    if (fread(&flag, 1, 1, file) != 1)
        return NULL;

    if (flag == 'L') {  // Leaf node
        char ch;
        if (fread(&ch, 1, 1, file) != 1)
            return NULL;
        return createNode(ch);
    }

    // Internal node
    Node *node = createNode('\0');
    node->left = rebuildHuffmanTree(file);
    node->right = rebuildHuffmanTree(file);
    return node;
}

// Decompress file using Huffman Tree
void decompressFile(const char *inputFile, const char *outputFile) {
    FILE *input = fopen(inputFile, "rb");
    FILE *output = fopen(outputFile, "wb");

    if (!input || !output) {
        printf("Error opening files!\n");
        return;
    }

    // Rebuild Huffman Tree
    Node *root = rebuildHuffmanTree(input);
    if (!root) {
        printf("Error: Failed to reconstruct Huffman tree.\n");
        fclose(input);
        fclose(output);
        return;
    }

    Node *current = root;
    uint8_t buffer;
    int bitPos = 0;

    // Read encoded bits and decode characters
    while (1) {
        int bit = readBit(input, &buffer, &bitPos);
        if (bit == -1) break;

        current = (bit == 0) ? current->left : current->right;

        if (!current->left && !current->right) { // Leaf node
            fputc(current->ch, output);
            current = root;
        }
    }

    fclose(input);
    fclose(output);
    printf("File decompressed successfully.\n");
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    decompressFile(argv[1], argv[2]);
    return 0;
}
