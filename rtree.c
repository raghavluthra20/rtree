#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#define DIMS 2
#define M 4
#define m 2
#define INT_MAX 999999
#define INT_MIN -999999

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}


typedef struct mbr* MBR;
struct mbr {
    int bottomLeft[DIMS];
    int topRight[DIMS];
};

typedef struct item* ITEM;
struct item {
    int data[DIMS];
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

//     int x0 = INT_MIN, x1 = INT_MAX, y0 = INT_MIN, y1 = INT_MAX;

//     // for leaf node
//     if(node->isLeaf) {
//         for(int i = 0; i < node->numChildren; i++) {
//             ITEM item = node->items[i];
//             int data_x = item->x;
//             int data_y = item->y;
//             x0 = min(x0, data_x);
//             y0 = min(y0, data_y);
//             x1 = max(x1, data_x);
//             y1 = max(y1, data_y);
//         }

//         MBR rect = (MBR) malloc(sizeof(struct mbr));
//         rect->x[0] = x0;
//         rect->x[1] = x1;
//         rect->y[0] = y0;
//         rect->y[1] = y1;

//         return rect;
//     }

//     // for internal node
//     else {
//         for(int i = 0; i < node->numChildren; i++) {
//             MBR childRect = node->rects[i];
//             int data_x0 = childRect->x[0];
//             int data_y0 = childRect->y[0];
//             int data_x1 = childRect->x[1];
//             int data_y1 = childRect->y[1];

//             x0 = min(x0, data_x0);
//             y0 = min(y0, data_y0);
//             x1 = max(x1, data_x1);
//             y1 = max(y1, data_y1);
//         }

//         MBR rect = (MBR) malloc(sizeof(struct mbr));
//         rect->x[0] = x0;
//         rect->x[1] = x1;
//         rect->y[0] = y0;
//         rect->y[1] = y1;

//         return rect;
//     }
// }

// function to choose leaf for inserting a new element in the rtree
NODE chooseLeaf(RTREE r, ITEM i) {
    NODE n = r->root;
    if (n->isLeaf == true) {
        return n;
    }
    int reqIndex, emptyIndex;
    int item[DIMS] = i->data;
    int minAreaIndex[M];
    while (n->isLeaf == false) {
        memset(minAreaIndex, 0, sizeof(minAreaIndex));
        emptyIndex = 0;
        int enlargement = INT_MAX;
        for (int i = 0; i < n->numChildren; i++) {
            int area = 1;
            // Calculating actual area of a particular mbr
            for (int j = 0; j < DIMS; j++) {
                area = area * (n->rects[i]->topRight[j] - n->rects[i]->bottomLeft[j]);
            }
            // Calculating area of enhanced mbr
            int newArea = 1;
            for (int j = 0; j < DIMS; j++) {
                newArea = newArea * (max(n->rects[i]->topRight[j], item[j]) - min(n->rects[i]->bottomLeft[j], item[j]));
            }
            if (enlargement == newArea - area) {
                minAreaIndex[emptyIndex] = i;
                emptyIndex++;
            }
            else if (enlargement > newArea - area) {
                enlargement = newArea - area;
                memset(minAreaIndex, 0, sizeof(minAreaIndex));
                emptyIndex = 0;
                minAreaIndex[0] = i;
            }
        }
        // Index of children requiring least enlargement are stored in minAreaIndex array, now we need child with least
        // area among these
        int minArea = INT_MAX;
        for (int i = 0; i < emptyIndex; i++) {
            int area = 1;
            for (int j = 0; j < DIMS; j++) {
                area = area * (n->rects[i]->topRight[j] - n->rects[i]->bottomLeft[j]);
            }
            if (area < minArea) {
                reqIndex = i;
            }
        }
        n = n->children[reqIndex];
    }
    return n;
}

RTREE adjustLeaf(RTREE r, NODE L)
{
    NODE n = L;
    
}

bool rectIntersects(MBR r1, MBR r2) {
    for(int d = 0; d < DIMS; d++) {
        if(r1->bottomLeft[d] > r2->topRight[d] || r1->topRight[d] < r2->bottomLeft[d])
            return false;
    }

    return true;
}

MBR mergeRect(MBR r1, MBR r2) {
    MBR rect = (MBR) malloc(sizeof(struct mbr));
    for(int d = 0; d < DIMS; d++) {
        rect->bottomLeft[d] = min(r1->bottomLeft[d], r2->bottomLeft[d]);
        rect->topRight[d] = max(r1->topRight[d], r2->topRight[d]);
    }

    return rect;
}


int main() {
    printf("Hello world");
    return 0;
}
