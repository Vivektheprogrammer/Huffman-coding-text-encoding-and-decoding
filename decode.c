#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Huffman tree node
typedef struct Node {
    char character;
    int frequency;
    struct Node *left, *right;
} Node;

// Priority queue node
typedef struct PriorityQueue {
    Node *nodes[256];
    int size;
} PriorityQueue;

// Function to create a new node
Node *createNode(char character, int frequency) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->character = character;
    node->frequency = frequency;
    node->left = node->right = NULL;
    return node;
}

// Function to insert a node into the priority queue
void insertQueue(PriorityQueue *pq, Node *node) {
    int i = pq->size++;
    while (i && node->frequency < pq->nodes[(i - 1) / 2]->frequency) {
        pq->nodes[i] = pq->nodes[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    pq->nodes[i] = node;
}

// Function to extract the minimum node from the priority queue
Node *extractMin(PriorityQueue *pq) {
    Node *minNode = pq->nodes[0];
    pq->nodes[0] = pq->nodes[--pq->size];
    int i = 0, smallest;
    while ((2 * i + 1) < pq->size) {
        smallest = (2 * i + 1);
        if ((2 * i + 2) < pq->size && pq->nodes[2 * i + 2]->frequency < pq->nodes[smallest]->frequency)
            smallest = 2 * i + 2;
        if (pq->nodes[i]->frequency < pq->nodes[smallest]->frequency)
            break;
        Node *temp = pq->nodes[i];
        pq->nodes[i] = pq->nodes[smallest];
        pq->nodes[smallest] = temp;
        i = smallest;
    }
    return minNode;
}

// Function to build the Huffman tree
Node *buildHuffmanTree(const char *message) {
    int freq[256] = {0};
    for (int i = 0; message[i]; i++)
        freq[(unsigned char)message[i]]++;
    
    PriorityQueue pq = { .size = 0 };
    for (int i = 0; i < 256; i++)
        if (freq[i])
            insertQueue(&pq, createNode(i, freq[i]));
    
    while (pq.size > 1) {
        Node *left = extractMin(&pq);
        Node *right = extractMin(&pq);
        Node *merged = createNode('\0', left->frequency + right->frequency);
        merged->left = left;
        merged->right = right;
        insertQueue(&pq, merged);
    }
    return extractMin(&pq);
}

// Function to save Huffman tree structure
void saveHuffmanTree(Node *root, FILE *file) {
    if (!root) {
        fprintf(file, "#"); // Marker for NULL
        return;
    }
    fprintf(file, "%c", root->character);
    saveHuffmanTree(root->left, file);
    saveHuffmanTree(root->right, file);
}

// Function to load Huffman tree structure
Node *loadHuffmanTree(FILE *file) {
    char c = fgetc(file);
    if (c == '#' || c == EOF)
        return NULL;
    Node *node = createNode(c, 0);
    node->left = loadHuffmanTree(file);
    node->right = loadHuffmanTree(file);
    return node;
}

// Function to decode a message using the Huffman tree
void decodeMessage(Node *root, const char *encoded, char *decoded) {
    Node *current = root;
    int j = 0;
    for (int i = 0; encoded[i] != '\0'; i++) {
        current = (encoded[i] == '0') ? current->left : current->right;
        if (current->left == NULL && current->right == NULL) {
            decoded[j++] = current->character;
            current = root;
        }
    }
    decoded[j] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <encoded message>\n", argv[0]);
        return 1;
    }
    
    FILE *treeFile = fopen("huffman_tree.dat", "r");
    if (!treeFile) {
        printf("Error: Huffman tree file not found.\n");
        return 1;
    }
    
    Node *root = loadHuffmanTree(treeFile);
    fclose(treeFile);
    
    char decoded[1024];
    decodeMessage(root, argv[1], decoded);

    printf("%s\n", decoded);
    return 0;
}
