#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


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
    memset(myNode, 0, sizeof(myNode));
    myNode->isLeaf = isLeaf;
    return myNode;
}

RTREE createNewRtree() {
    RTREE myRtree = (RTREE) malloc(sizeof(struct rtree));
    memset(myRtree, 0, sizeof(myRtree));
    return myRtree;
}

// function to find the MBR for a given node
// MBR findMBR(NODE node) {
//     if(node->numChildren == 0) {
//         printf("Error: no children in given node!");
//     }

//     int x1, x2, y1, y2;

//     // for leaf node
//     if(isLeaf) {
//         for(int i = 0; i < node->numChildren; i++) {
            
//         }
//     }

//     // for internal node
// }




int main() {
    printf("Hello world");
    return 0;
}