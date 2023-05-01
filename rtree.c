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


// function to choose leaf for inserting a new element in the rtree
NODE chooseLeaf(RTREE r, ITEM i) {
    NODE n = r->root;
    if (n->isLeaf == true) {
        return n;
    }
    int reqIndex, emptyIndex;
    int item[DIMS];
    for (int d = 0; d < DIMS; d++) {
        item[d] = i->data[d];
    }
    int minAreaIndex[M];
    while (n->isLeaf == false) {
        memset(minAreaIndex, 0, sizeof(minAreaIndex));
        emptyIndex = 0;
        int enlargement = INT_MAX;
        for (int i = 0; i < n->numChildren; i++) {
            // Calculating actual area of a particular mbr
            long long area = findRectArea(n->rects[i]);

            // Calculating area of enlarged mbr
            MBR pointMBR = (MBR) malloc(sizeof(struct mbr));
            
            for (int d = 0; d < DIMS; d++) {
                pointMBR->bottomLeft[d] = item[d];
                pointMBR->topRight[d] = item[d];
            }
        
            long long enlargedArea = findRectArea(mergeRect(n->rects[i], pointMBR));
            if (enlargement == enlargedArea - area) {
                minAreaIndex[emptyIndex] = i;
                emptyIndex++;
            }
            else if (enlargement > enlargedArea - area) {
                enlargement = enlargedArea - area;
                memset(minAreaIndex, 0, sizeof(minAreaIndex));
                minAreaIndex[0] = i;
                emptyIndex = 1;
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


void printItem(ITEM item) {
    printf("Item: Data Stored - (");
    for(int d = 0; d < DIMS; d++) {
        printf("%d, ", item->data[d]);
    }
    printf(")");
}

void printMBR(MBR rect) {
    printf("MBR: Top Right - (");
    for(int d = 0; d < DIMS; d++) {
        printf("%d, ", rect->topRight[d]);
    }
    printf("),   ");

    printf("Bottom Left - (");
    for(int d = 0; d < DIMS; d++) {
        printf("%d, ", rect->bottomLeft[d]);
    }
    printf(")");
}

// prints all items stored in the leaf node
void printLeafNode(NODE node) {
    if(!node->isLeaf) {
        printf("Error: trying to print items of LEAF node!");
        return;
    }

    if(node->numChildren == 0) {
        printf("Empty leaf node.\n");
        return;
    }

    printf("Leaf node with %d items: \n", (node->numChildren));
    for(int i = 0; i < node->numChildren; i++) {
        printItem(node->items[i]);
        printf("\n");
    }
}

// prints the MBR of the leaf node
void printInternalNode(NODE node) {
    if(node->isLeaf) {
        printf("Error: trying to print children of INTERNAL node!");
        return;
    }

    if(node->numChildren == 0) {
        printf("Empty internal node.\n");
        return;
    }
    printf("Internal node with %d children, ", node->numChildren);
    printMBR(findMBR(node));
    printf("\n");
}

void printNode(NODE node) {
    if(node->isLeaf) {
        printLeafNode(node);
    }

    else {
        printInternalNode(node);
    }
}


void traverse(NODE root) {
    if(root == NULL) return;

    // print root
    printNode(root);

    // print children
    for(int i = 0; i < root->numChildren; i++) {
        traverse(root->children[i]);
    }
}

int main() {
    printf("Hello world\n");
    int data1[2] = {1, 3};
    int data2[2] = {2, 2};
    int data3[2] = {6, 1};
    int data4[2] = {4, 5};
    int data5[2] = {2, 4};
    int data6[2] = {7, 5};
    int data7[2] = {2, 9};
    int data8[2] = {1, 7};

    ITEM item1 = createNewItem(data1);
    ITEM item2 = createNewItem(data2);
    ITEM item3 = createNewItem(data3);
    ITEM item4 = createNewItem(data4);
    ITEM item5 = createNewItem(data5);
    ITEM item6 = createNewItem(data6);
    ITEM item7 = createNewItem(data7);
    ITEM item8 = createNewItem(data8);


    NODE myNode1 = createNewNode(true);
    NODE myNode2 = createNewNode(false);
    NODE node1 = createNewNode(true);
    NODE node2 = createNewNode(true);
    NODE node3 = createNewNode(true);

    myNode1->items[0] = item1;
    myNode1->items[1] = item2;
    myNode1->items[2] = item3;
    myNode1->numChildren = 3;

    myNode2->children[0] = node1;
    myNode2->children[1] = node2;
    myNode2->children[2] = node3;
    myNode2->numChildren = 3;

    printNode(myNode1);
    printMBR(findMBR(myNode1));

    return 0;
}