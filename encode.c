#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_NODES 256

// Huffman tree node
typedef struct Node {
    char character;
    int frequency;
    struct Node *left, *right;
} Node;

// Priority queue for Huffman Tree nodes
typedef struct {
    Node *nodes[MAX_TREE_NODES];
    int size;
} PriorityQueue;

void initQueue(PriorityQueue *pq) {
    pq->size = 0;
}

// Insert node into priority queue
void insertQueue(PriorityQueue *pq, Node *node) {
    int i = pq->size++;
    while (i && node->frequency < pq->nodes[(i - 1) / 2]->frequency) {
        pq->nodes[i] = pq->nodes[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    pq->nodes[i] = node;
}

// Remove node from priority queue
Node *removeQueue(PriorityQueue *pq) {
    Node *node = pq->nodes[0];
    pq->nodes[0] = pq->nodes[--pq->size];

    int i = 0, j = 1;
    while (j < pq->size) {
        if (j + 1 < pq->size && pq->nodes[j]->frequency > pq->nodes[j + 1]->frequency)
            j++;
        if (pq->nodes[i]->frequency <= pq->nodes[j]->frequency)
            break;
        Node *temp = pq->nodes[i];
        pq->nodes[i] = pq->nodes[j];
        pq->nodes[j] = temp;
        i = j;
        j = 2 * i + 1;
    }
    return node;
}

// Build Huffman Tree
Node *buildHuffmanTree(const char *text) {
    int freq[256] = {0};
    for (int i = 0; text[i] != '\0'; i++)
        freq[(unsigned char)text[i]]++;

    PriorityQueue pq;
    initQueue(&pq);

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            Node *node = (Node *)malloc(sizeof(Node));
            node->character = (char)i;
            node->frequency = freq[i];
            node->left = node->right = NULL;
            insertQueue(&pq, node);
        }
    }

    while (pq.size > 1) {
        Node *left = removeQueue(&pq);
        Node *right = removeQueue(&pq);
        Node *parent = (Node *)malloc(sizeof(Node));
        parent->character = '\0';
        parent->frequency = left->frequency + right->frequency;
        parent->left = left;
        parent->right = right;
        insertQueue(&pq, parent);
    }
    return removeQueue(&pq);
}

// Generate Huffman codes
void generateHuffmanCodes(Node *root, char *code, int depth, char codes[256][256]) {
    if (!root)
        return;

    if (root->left == NULL && root->right == NULL) {
        code[depth] = '\0';
        strcpy(codes[(unsigned char)root->character], code);
        return;
    }

    code[depth] = '0';
    generateHuffmanCodes(root->left, code, depth + 1, codes);

    code[depth] = '1';
    generateHuffmanCodes(root->right, code, depth + 1, codes);
}

// Save Huffman tree structure
void saveHuffmanTree(Node *root, FILE *file) {
    if (!root) {
        fprintf(file, "#"); // Marker for NULL
        return;
    }
    fprintf(file, "%c", root->character);
    saveHuffmanTree(root->left, file);
    saveHuffmanTree(root->right, file);
}

// Encode message
void encodeMessage(const char *text, char *encoded, Node **root) {
    *root = buildHuffmanTree(text);
    char codes[256][256] = {0};
    char code[256];
    generateHuffmanCodes(*root, code, 0, codes);

    encoded[0] = '\0';
    for (int i = 0; text[i] != '\0'; i++) {
        strcat(encoded, codes[(unsigned char)text[i]]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <message>\n", argv[0]);
        return 1;
    }

    char encoded[1024];  
    Node *root;
    encodeMessage(argv[1], encoded, &root);

    FILE *treeFile = fopen("huffman_tree.dat", "w");
    if (!treeFile) {
        printf("Error: Unable to save Huffman tree.\n");
        return 1;
    }
    saveHuffmanTree(root, treeFile);
    fclose(treeFile);

    printf("Encoded Message: %s\n", encoded);
    return 0;
}
