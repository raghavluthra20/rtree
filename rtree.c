#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#define DIMS 2
#define M 4
#define m 2
#define INT_MAX 9999999
#define INT_MIN -9999999

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
    NODE parent;

    // for leaf nodes this represents number of items
    // for internal nodes this represents number of children
    int numChildren;

    MBR rects[M]; // MBRs for all children/items
    NODE children[M];
    ITEM items[M];
};

typedef struct rtree* RTREE;
struct rtree {
    int count; // total number of items (data objects)
    NODE root;
    MBR rect;
};


MBR createNewRect(int bottomLeft[DIMS], int topRight[DIMS]) {
    MBR rect = (MBR) malloc(sizeof(struct mbr));
    for(int d = 0; d < DIMS; d++) {
        rect->bottomLeft[d] = bottomLeft[d];
        rect->topRight[d] = topRight[d];
    }
}

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

ITEM createNewItem(int* data) {
    ITEM myItem = (ITEM) malloc(sizeof(struct item));
    for(int d = 0; d < DIMS; d++) {
        myItem->data[d] = data[d];
    }
    return myItem;
}

// function to find the MBR for a given node
// can be used in adjustTree function
MBR findMBR(NODE node) {
    if(node == NULL) {
        printf("Error: node is NULL\n");
        return NULL;
    }

    if(node->numChildren == 0) {
        printf("Error: no children in given node!\n");
        return NULL;
    }

    // initalize values for the MBR object to be return
    MBR rect = (MBR) malloc(sizeof(struct mbr));
    for(int d = 0; d < DIMS; d++) {
        rect->bottomLeft[d] = INT_MAX;
        rect->topRight[d] = INT_MIN;
    }

    for(int d = 0; d < DIMS; d++) {
        for(int i = 0; i < node->numChildren; i++) {
            // for leaf node
            if(node->isLeaf) {
                ITEM item = node->items[i];
                rect->bottomLeft[d] = min(rect->bottomLeft[d], item->data[d]);
                rect->topRight[d] = max(rect->topRight[d], item->data[d]);
            }

            // for internal node
            else {
                MBR childRect = node->rects[i];
                rect->bottomLeft[d] = min(rect->bottomLeft[d], childRect->bottomLeft[d]);
                rect->topRight[d] = max(rect->topRight[d], childRect->topRight[d]);
            }
        }
    }

    return rect;
}

// checks if two rectangles intersect or not
bool rectIntersects(MBR r1, MBR r2) {
    for(int d = 0; d < DIMS; d++) {
        if(r1->bottomLeft[d] > r2->topRight[d] || r1->topRight[d] < r2->bottomLeft[d])
            return false;
    }

    return true;
}

// checks if an item(data object) is contained within a given rectangle
bool rectContainsItem(MBR rect, ITEM item) {
    MBR itemRect = createNewRect(item->data, item->data);
    return rectIntersects(rect, itemRect);
}


// function to find area of given rectangle
// will be used in chooseLeaf() function and nodeSplitting
long long findRectArea(MBR rect) {
    long long area = 1;
    for(int d = 0; d < DIMS; d++) {
        area *= (rect->topRight[d] - rect->bottomLeft[d]);
    }

    return area;
}


// finds MBR of 2 given rectangles
MBR mergeRect(MBR r1, MBR r2) {
    MBR rect = (MBR) malloc(sizeof(struct mbr));
    for(int d = 0; d < DIMS; d++) {
        rect->bottomLeft[d] = min(r1->bottomLeft[d], r2->bottomLeft[d]);
        rect->topRight[d] = max(r1->topRight[d], r2->topRight[d]);
    }

    return rect;
}

/*
    traverses the rtree and inserts the data objects
    intersecting with the query rectangle in the ans array
*/
void search(MBR qRect, NODE t, ITEM* ans, int *count) {
    if(t->isLeaf) {
        for(int i = 0; i < t->numChildren; i++) {
            ITEM item = t->items[i];
            if(rectContainsItem(qRect, item)) {
                ans[(*count)++] = item;
            }
        }
    }

    else {
        for(int i = 0; i < t->numChildren; i++) {
            MBR childRect = t->rects[i];
            if(rectIntersects(qRect, childRect)) {
                search(qRect, t->children[i], ans, count);
            }
        }
    }
}


int main() {
    printf("Hello world\n");

    return 0;
}