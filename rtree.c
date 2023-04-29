#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


#define DIMS 2
#define M 4
#define m 2

typedef struct mbr* MBR;
struct mbr {
    int x[DIMS];
    int y[DIMS];
};

typedef struct item* ITEM;
struct item {
    int x;
    int y;
};

typedef struct node* NODE;
struct node {
    bool isLeaf;

    // for leaf nodes this represents number of items
    // for internal nodes this represents number of children
    int numChildren;

    MBR rects[M]; // MBRs for all children
    NODE children[M];
    ITEM items[M];
};

typedef struct rtree* RTREE;
struct rtree {
    int count; // total number of data objects
    NODE root;
    MBR rect;
};


NODE createNewNode(bool isLeaf) {
    NODE myNode = (NODE) malloc(sizeof(struct node));
    memset(node, 0, sizeof(node));
    myNode->isLeaf = isLeaf;
    return myNode;
}

RTREE createNewRtree() {
    RTREE myRtree = (RTREE) malloc(sizeof(struct rtree));
    memset(myRtree, 0, sizeof(myRtree));
    return myRtree
}

// function to find the MBR for a given node
MBR findMBR(NODE node) {
    if(node->numChildren == 0) {
        printf("Error: no children in given node!");
    }

    // for leaf node

    // for internal node
}


int main() {
    printf("Hello world");
    return 0;
}