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
    return rect;
}


NODE createNewNode(bool isLeaf) {
    NODE myNode = (NODE) malloc(sizeof(struct node));
    memset(myNode, 0, sizeof(struct node));
    myNode->isLeaf = isLeaf;
    return myNode;
}

RTREE createNewRtree()
{
    RTREE myRtree = (RTREE)malloc(sizeof(struct rtree));
    memset(myRtree, 0, sizeof(struct rtree));
    return myRtree;
}

ITEM createNewItem(int *data)
{
    ITEM myItem = (ITEM)malloc(sizeof(struct item));
    for (int d = 0; d < DIMS; d++)
    {
        myItem->data[d] = data[d];
    }
    return myItem;
}

// function to find area of given rectangle
// will be used in chooseLeaf() function and nodeSplitting
long long findRectArea(MBR rect)
{
    long long area = 1;
    for (int d = 0; d < DIMS; d++)
    {
        area *= (rect->topRight[d] - rect->bottomLeft[d]);
    }

    return area;
}

// finds MBR of 2 given rectangles
MBR mergeRect(MBR r1, MBR r2)
{
    MBR rect = (MBR)malloc(sizeof(struct mbr));
    for (int d = 0; d < DIMS; d++)
    {
        rect->bottomLeft[d] = min(r1->bottomLeft[d], r2->bottomLeft[d]);
        rect->topRight[d] = max(r1->topRight[d], r2->topRight[d]);
    }

    return rect;
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

// function to find the MBR for a given node
// can be used in adjustTree function
MBR findMBR(NODE node)
{
    if (node == NULL)
    {
        printf("Error: node is NULL\n"); // can use perror
        return NULL;
    }

    if (node->numChildren == 0)
    {
        printf("Error: no children in given node!\n"); // can use perror
        return NULL;
    }

    // initalize values for the MBR object to be return
    MBR rect = (MBR)malloc(sizeof(struct mbr));
    for (int d = 0; d < DIMS; d++)
    {
        rect->bottomLeft[d] = INT_MAX;
        rect->topRight[d] = INT_MIN;
    }

    for (int d = 0; d < DIMS; d++)
    {
        for (int i = 0; i < node->numChildren; i++)
        {
            // for leaf node
            if (node->isLeaf)
            {
                ITEM item = node->items[i];
                rect->bottomLeft[d] = min(rect->bottomLeft[d], item->data[d]);
                rect->topRight[d] = max(rect->topRight[d], item->data[d]);
            }

            // for internal node
            else
            {
                MBR childRect = node->rects[i];
                rect->bottomLeft[d] = min(rect->bottomLeft[d], childRect->bottomLeft[d]);
                rect->topRight[d] = max(rect->topRight[d], childRect->topRight[d]);
            }
        }
    }

    return rect;
}

bool compareMBR(MBR r1, MBR r2) {
    for (int i = 0; i < DIMS; i++) {
        if (r1->bottomLeft[i] != r2->bottomLeft[i] || r1->topRight[i] != r2->topRight[i]) {
            return false;
        }
    }
    return true;
}

// checks if two rectangles intersect or not
bool rectIntersects(MBR r1, MBR r2)
{
    for (int d = 0; d < DIMS; d++)
    {
        if (r1->bottomLeft[d] > r2->topRight[d] || r1->topRight[d] < r2->bottomLeft[d])
            return false;
    }

    return true;
}

int area(int a1, int a2, int b1, int b2)
{
    return abs((a1 - a2) * (b1 - b2));
}

bool isequal(ITEM n1, ITEM n2)
{
    int i;
    for (i = 0; i < DIMS; i++)
    {
        if (n1->data[i] != n2->data[i])
        {
            return false;
        }
    }
    return true;
}

long long calc_redundancy(MBR m1, MBR m2)
{
    // printf("printing m1 and m2");
    // printMBR(m1);
    // printf("\n");
    // printMBR(m2);
    // printf("\n");
    long long red = (findRectArea(mergeRect(m1, m2)) - (findRectArea(m1) + findRectArea(m2)));
    if(red < 0) red = 0;
    printf("red=%d", red);
    printf("\n");
    return red;
}

void pickseeds_item(ITEM seedlist[], ITEM first[])
{

    int i, j, max_area;
    first[0] = seedlist[0];
    first[1] = seedlist[1];
    max_area = area(first[0]->data[0], first[1]->data[0], first[0]->data[1], first[1]->data[1]);
    for (i = 0; i < M + 1; i++)
    {
        for (j = i; j < M + 1; j++)
        {
            if (area(seedlist[i]->data[0], seedlist[j]->data[0], seedlist[i]->data[1], seedlist[j]->data[1]) > max_area)
            {
                first[0] = seedlist[i];
                first[1] = seedlist[j];
                max_area = area(first[0]->data[0], first[1]->data[0], first[0]->data[1], first[1]->data[1]);
            }
        }
    }
}

void pickseeds_node(NODE seedlist[], MBR seedmbr[], int indexes[])
{
    // int listSize = sizeof(seedlist)/sizeof(seedlist[0]);
    // if(listSize != M+1) {
    //     printf("ERROR from pickseeds_node(): listSize not equal to M+1\n");
    //     return;
    // }
    // printf("list size: %d", listSize);
    NODE first[2];
    // int indexes[2];
    int i, j;
    long long max_redundancy;
    first[0] = seedlist[0];
    first[1] = seedlist[1];
    indexes[0] = 0;
    indexes[1] = 1;
    max_redundancy = calc_redundancy(seedmbr[0], seedmbr[1]);
    printf("max_red = %d\n", max_redundancy);

    for (i = 0; i < M + 1; i++)
    {
        for (j = i; j < M + 1; j++)
        {
            printf("calc_red(%d, %d) = %d\n", i, j, calc_redundancy(seedmbr[i], seedmbr[j]));
            if (calc_redundancy(seedmbr[i], seedmbr[j]) > max_redundancy)
            {
                first[0] = seedlist[i];
                first[1] = seedlist[j];
                indexes[0] = i;
                indexes[1] = j;
                max_redundancy = calc_redundancy(seedmbr[i], seedmbr[j]);
                printf("max_red updated(%d, %d) = %d\n", i, j, max_redundancy);
            }
        }
    }

    printf("indexes value: %d, %d\n", indexes[0], indexes[1]);
    printMBR(seedmbr[indexes[0]]);
    printMBR(seedmbr[indexes[1]]);
}

int picknext_item(NODE n1, NODE n2, ITEM i)
{
    MBR mi = createNewRect(i->data, i->data);
    MBR m1 = findMBR(n1);
    MBR m2 = findMBR(n2);
    if ((findRectArea(mergeRect(m1, mi)) - findRectArea(m1)) < (findRectArea(mergeRect(m2, mi)) - findRectArea(m2)))
    {
        n1->items[n1->numChildren] = i;
        n1->numChildren++;
        return 1;
    }
    else if ((findRectArea(mergeRect(m1, mi)) - findRectArea(m1)) > (findRectArea(mergeRect(m2, mi)) - findRectArea(m2)))
    {
        n2->items[n2->numChildren] = i;
        n2->numChildren++;
        return 1;
    }
    else if (n1->numChildren > n2->numChildren)
    {
        n2->items[n2->numChildren] = i;
        n2->numChildren++;
        return 1;
    }
    else
    {
        n1->items[n1->numChildren] = i;
        n1->numChildren++;
        return 1;
    }
    return 0;
}

int picknext_child(NODE n1, NODE n2, NODE ni)
{
    MBR mi = findMBR(ni);
    MBR m1 = findMBR(n1);
    MBR m2 = findMBR(n2);

    if ((findRectArea(mergeRect(m1, mi)) - findRectArea(m1)) < (findRectArea(mergeRect(m2, mi)) - findRectArea(m2)))
    {
        n1->children[n1->numChildren] = ni;
        n1->numChildren++;
        return 1;
    }
    else if ((findRectArea(mergeRect(m1, mi)) - findRectArea(m1)) > (findRectArea(mergeRect(m2, mi)) - findRectArea(m2)))
    {
        n2->children[n2->numChildren] = ni;
        n2->numChildren++;
        return 1;
    }
    else if (n1->numChildren > n2->numChildren)
    {
        n2->children[n2->numChildren] = ni;
        n2->numChildren++;
        return 1;
    }
    else
    {
        n1->children[n1->numChildren] = ni;
        n1->numChildren++;
        return 1;
    }
    return 0;
}

// function to choose leaf for inserting a new element in the rtree
NODE chooseLeaf(RTREE r, ITEM x)
{
    NODE n = r->root;
    if (n->isLeaf == true)
    {
        return n;
    }
    int reqIndex, emptyIndex;
    int item[DIMS];
    for (int d = 0; d < DIMS; d++)
    {
        item[d] = x->data[d];
    }
    int minAreaIndex[M];
    while (n->isLeaf == false)
    {
        memset(minAreaIndex, 0, sizeof(minAreaIndex));
        emptyIndex = 0;
        int enlargement = INT_MAX;
        for (int i = 0; i < n->numChildren; i++)
        {
            // Calculating actual area of a particular mbr
            long long area = findRectArea(n->rects[i]);

            // Calculating area of enlarged mbr
            MBR pointMBR = (MBR)malloc(sizeof(struct mbr));

            for (int d = 0; d < DIMS; d++)
            {
                pointMBR->bottomLeft[d] = item[d];
                pointMBR->topRight[d] = item[d];
            }

            long long enlargedArea = findRectArea(mergeRect(n->rects[i], pointMBR));
            if (enlargement == enlargedArea - area)
            {
                minAreaIndex[emptyIndex] = i;
                emptyIndex++;
            }
            else if (enlargement > enlargedArea - area)
            {
                enlargement = enlargedArea - area;
                memset(minAreaIndex, 0, sizeof(minAreaIndex));
                minAreaIndex[0] = i;
                emptyIndex = 1;
            }
        }

        // Index of children requiring least enlargement are stored in minAreaIndex array, now we need child with least
        // area among these
        int minArea = INT_MAX;
        for (int i = 0; i < emptyIndex; i++)
        {
            // int area = 1;
            // for (int j = 0; j < DIMS; j++) {
            // area = area * (n->rects[i]->topRight[j] - n->rects[i]->bottomLeft[j]);
            // }
            int k=minAreaIndex[i];
            long long area = findRectArea(n->rects[k]);
            if (area < minArea)
            {
                reqIndex = k;
                minArea = area;
            }
        }
        n = n->children[reqIndex];
    }
    return n;
}


void LeafQuadraticSplit(NODE n, ITEM i, NODE n1, NODE n2) ///////// TAKEN L as n1 and LL as n2 //////////////////
{
    ITEM seeds[M + 1], picked_seeds[2];
    int j, k;
    // NODE n1 = createNewNode(true);
    // NODE n2 = createNewNode(true);

    for (j = 0; j < M; j++)
    {
        seeds[j] = n->items[j];
    }
    seeds[M] = i;

    pickseeds_item(seeds, picked_seeds);

    n1->parent = n->parent;
    n2->parent = n->parent;
    n1->items[0] = picked_seeds[0];
    n2->items[0] = picked_seeds[1];
    n1->numChildren++;
    n2->numChildren++;

    int flag0 = 0, flag1 = 0;
    int Updated_M = M;
    for (j = 0; j <= M; j++)
    {
        if (flag0 == 0 && isequal(seeds[j], picked_seeds[0]))
        {
            flag0 = 1;
            Updated_M = M + 1 - (flag0 + flag1);
            for (k = j; k < Updated_M; k++)
            {
                seeds[k] = seeds[k + 1];
            }
        }
        else if (flag1 == 0 && isequal(seeds[j], picked_seeds[1]))
        {
            flag1 = 1;
            Updated_M = M + 1 - (flag0 + flag1);
            for (k = j; k < Updated_M; k++)
            {
                seeds[k] = seeds[k + 1];
            }
        }
    }

    for (k = 0; k < Updated_M; k++)
    {
        if (!picknext_item(n1, n2, seeds[k]))
            printf("Split Problem: Unable to insert element into a new node");
    }

    if(n1->numChildren < m && n1->numChildren < m)
    {
        printf("Something's Wrong");
    }
    else if (n1->numChildren<m) //will not work for m>2 -> in that case you have to make a m-n1->numCildren size Array
    {
        int index;
        long long minarea = findRectArea(mergeRect(createNewRect(n1->items[0]->data ,n1->items[0]->data ),createNewRect(n2->items[0]->data ,n2->items[0]->data )));
        for(k=0;k<M;k++)
        {
            if(findRectArea(mergeRect(createNewRect(n1->items[0]->data ,n1->items[0]->data ),createNewRect(n2->items[k]->data ,n2->items[k]->data ))) < minarea)
            {
                minarea = findRectArea(mergeRect(createNewRect(n1->items[0]->data ,n1->items[0]->data ),createNewRect(n2->items[k]->data ,n2->items[k]->data )));
                index = k;
            }
        }
        n1->items[1] = n2->items[index];
        for(int l = index ; l<M-1 ;l++)
        {
            n2->items[l] = n2->items[l+1];
        }
        n1->numChildren++;
        n2->numChildren--;
    }
    else if (n2->numChildren<m)
    {
        int index;
        long long minarea = findRectArea(mergeRect(createNewRect(n2->items[0]->data ,n2->items[0]->data ),createNewRect(n1->items[0]->data ,n1->items[0]->data )));
        for(k=0;k<M;k++)
        {
            if(findRectArea(mergeRect(createNewRect(n2->items[0]->data,n2->items[0]->data ),createNewRect(n1->items[k]->data ,n1->items[k]->data )))<minarea)
            {
                minarea = findRectArea(mergeRect(createNewRect(n2->items[0]->data ,n2->items[0]->data ),createNewRect(n1->items[k]->data ,n1->items[k]->data )));
                index = k;
            }
        }
        n2->items[1] = n1->items[index];
        for(int l = index; l<M-1 ; l++)
        {
            n1->items[l] = n1->items[l+1];
        }
        n2->numChildren++;
        n1->numChildren--;
    }

    // adjustments in parent OR return the 2 nodes

    // destroy node n
}

void InternalQuadraticSplit(NODE n, NODE i, NODE n1, NODE n2)
{
    NODE seeds[M + 1], picked_seeds[2];
    MBR seedsmbr[M + 1];
    int j, k;
    int picked_seeds_indexes[2];
    //NODE n1 = createNewNode(true);
    //NODE n2 = createNewNode(true);
    for (j = 0; j < M; j++)
    {
        seeds[j] = n->children[j];
        seedsmbr[j] = n->rects[j];
    }
    seeds[M] = i;
    seedsmbr[M] = findMBR(i);

    // picked_seeds_indexes = pickseeds_node(seeds,seedsmbr);

    pickseeds_node(seeds, seedsmbr, picked_seeds_indexes);

    n1->parent = n->parent;
    n2->parent = n->parent;
    n1->children[0] = seeds[picked_seeds_indexes[0]];
    n2->children[0] = seeds[picked_seeds_indexes[1]];
    n1->numChildren++;
    n2->numChildren++;

    int flag0 = 0, flag1 = 0;
    int Updated_M;
    for (j = 0; j <= M; j++)
    {
        if (flag0 == 0 && j == picked_seeds_indexes[0])
        {
            flag0 = 1;
            Updated_M = M + 1 - (flag0 + flag1);
            for (k = j; k < Updated_M; k++)
            {
                seeds[k] = seeds[k + 1];
            }
        }
        else if (flag1 == 0 && j == picked_seeds_indexes[1])
        {
            flag1 = 1;
            Updated_M = M + 1 - (flag0 + flag1);
            for (k = j; k < Updated_M; k++)
            {
                seeds[k] = seeds[k + 1];
            }
        }
    }

    for (k = 0; k < Updated_M; k++)
    {
        if (!picknext_child(n1, n2, seeds[k]))
            printf("Split Problem: Unable to insert element into a new node");
    }

    if(n1->numChildren<m && n1->numChildren<m)
    {
        printf("Something's Wrong");
    }
    else if (n1->numChildren<m) //will not work for m>2 -> in that case you have to make a m-n1->numCildren size Array
    {
        int index;
        long long minred = calc_redundancy(findMBR(n1->children[0]),findMBR(n2->children[0]));
        for(k=0;k<M;k++)
        {
            if(calc_redundancy(findMBR(n1->children[0]),findMBR(n2->children[k]))<minred)
            {
                minred = calc_redundancy(findMBR(n1->children[0]),findMBR(n2->children[k]));
                index = k;
            }
        }
        n1->children[1] = n2->children[index];
        for(int l = index ; l<M-1 ; l++)
        {
            n2->children[l] = n2->children[l+1];
        }
        n1->numChildren++;
        n2->numChildren--;
    }
    else if (n2->numChildren<m)
    {
        int index;
        long long minred = calc_redundancy(findMBR(n2->children[0]),findMBR(n1->children[0])); 
        for(k=0;k<M;k++)
        {
            if(calc_redundancy(findMBR(n2->children[0]),findMBR(n1->children[k]))<minred)
            {
                minred = calc_redundancy(findMBR(n2->children[0]),findMBR(n1->children[k]));
                index = k;
            }
        }
        n2->children[1] = n1->children[index];
        for(int l = index; l<M-1; l++)
        {
            n1->children[l] = n1->children[l+1];
        }
        n2->numChildren++;
        n1->numChildren--;
    }
    // adjustments in parent OR return the 2 nodes

    // destroy node n
}

void split(NODE n, NODE L, NODE LL, ITEM i)
{
    int q = n->numChildren;
    if (q < M)
    {
        for (int d = 0; d < q; d++)
        {
            if (L->isLeaf)
            {
                L->items[d] = n->items[d];
            }
            else
                L->children[d] = n->children[d];

            L->numChildren++;
        }
        if (L->isLeaf)
        {
            L->items[q] = i;
        }
        else
            L->children[q] = LL; ///CHECK//

        L->numChildren++;
        return;
    }
    else if (q == M)
    {
        if (n->isLeaf)
        {
            LeafQuadraticSplit(n, i, L, LL);
        }
        else
        {
            InternalQuadraticSplit(n, LL, L, LL);
        }
    }
}

void printItem(ITEM item) {
    printf("Item: Data Stored - (");
    for(int d = 0; d < DIMS; d++) {
        printf("%d, ", item->data[d]);
    }
    printf(")");
}

// prints all items stored in the leaf node
void printLeafNode(NODE node) {
    if(node->isLeaf == false) {
        // printf("Error: trying to print items of INTERNAL node!");
        printf("Error from printLeafNode(): this is a leaf node!\n");
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
        printf("Error from printInternalNode(): trying to print children of LEAF node!\n");
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

void adjustTree(RTREE r, NODE L, NODE LL, MBR mx)
{
    NODE p = L->parent;
    // int req_ind =0;
    MBR mr = findMBR(L);
    MBR parent_mbr = findMBR(p);
    printNode(L);
    printNode(p);
    printMBR(mx);
    if (LL == NULL) // node has not been splitted
    {
        
        for (int i = 0; i < p->numChildren; i++)
        {
            printMBR(p->rects[i]);
            if (compareMBR(p->rects[i], mx))
            {
                p->rects[i] = mr;
                break;
            }
        }
        memset(L, 0, sizeof(struct node)); 
        memset(LL, 0, sizeof(struct node));
        split(p,L,LL, NULL );
        adjustTree(r, L , LL, parent_mbr);
    }
    else     // Case for node splitting in L and LL.
    {
        MBR mrb = findMBR(LL);
        for (int i = 0; i < M; i++) // updating the MBR for the L node 
        {
            if (compareMBR(p->rects[i], mx))
            {
                p->rects[i] = mr;
                break;
            }
        }  
        if(p->numChildren < M) // adding mbr for LL node if vacancy is available 
        {
            printf("here\n");
            MBR par_mbr = findMBR(p);
            p->rects[p->numChildren] = mrb;
            p->numChildren++;
            printNode(p);
            adjustTree(r, p, NULL, par_mbr);
        }
        else // adding mbr for LL node if vacancy is NOT available 
        {
            NODE j = createNewNode(true);
            j->rects[0] = mrb;
            j->children[0] = LL;
            j->numChildren++;
            j->parent = p->parent;
            memset(L, 0, sizeof(struct node)); 
            memset(LL, 0, sizeof(struct node));
            
            MBR mq = findMBR(p);
            InternalQuadraticSplit(p, j, L, LL);
            adjustTree(r, L, LL, mq);
            
        }
    }
    if(p->parent == NULL && LL != NULL)
    {   
        NODE root = createNewNode(false);
        root->children[0] = L;
        root->children[1] = LL;
        root->rects[0] = findMBR(L);
        root->rects[1] = findMBR(LL);
        root->numChildren = 2;
        r->root = root;
        L->parent = root;
        LL->parent = root;
    }
}


void insert(RTREE r, ITEM i)
{
    NODE L = createNewNode(true);
    NODE LL = createNewNode(true);

    NODE n = chooseLeaf(r, i);
    MBR mx = findMBR(n);
    split(n, L, LL, i);
    adjustTree(r, L, LL, mx);

}



void traverse(NODE root) {
    if(root == NULL) return;

    // print root
    printNode(root);

    // leaf nodes dont have children so return
    if(root->isLeaf) return;

    // print children
    for(int i = 0; i < root->numChildren; i++) {
        traverse(root->children[i]);
    }
}

int main()
{
    printf("Hello world\n");

    int data1[2] = {1, 1};
    int data2[2] = {2, 2};
    int data3[2] = {3, 3};
    int data4[2] = {4, 4};
    int data5[2] = {5, 5};
    int data6[2] = {6, 6};
    int data7[2] = {7, 7};
    int data8[2] = {8, 8};
    int data9[2] = {9, 9};
    

    ITEM item1 = createNewItem(data1);
    ITEM item2 = createNewItem(data2);
    ITEM item3 = createNewItem(data3);
    ITEM item4 = createNewItem(data4);
    ITEM item5 = createNewItem(data5);
    ITEM item6 = createNewItem(data6);
    ITEM item7 = createNewItem(data7);
    ITEM item8 = createNewItem(data8);
    ITEM item9 = createNewItem(data9);


    NODE node1 = createNewNode(true);
    NODE node2 = createNewNode(true);
    NODE node3 = createNewNode(true);

    node1->numChildren = 3;
    node1->items[0] = item1;
    node1->items[1] = item2;
    node1->items[2] = item3;
    node1->rects[0] = createNewRect(item1->data, item1->data);
    node1->rects[1] = createNewRect(item2->data, item2->data);
    node1->rects[2] = createNewRect(item3->data, item3->data);

    node2->numChildren = 3;
    node2->items[0] = item4;
    node2->items[1] = item5;
    node2->items[2] = item6;
    node2->rects[0] = createNewRect(item4->data, item4->data);
    node2->rects[1] = createNewRect(item5->data, item5->data);
    node2->rects[2] = createNewRect(item6->data, item6->data);

    node3->numChildren = 2;
    node3->items[0] = item7;
    node3->items[1] = item8;
    node3->rects[0] = createNewRect(item7->data, item7->data);
    node3->rects[1] = createNewRect(item8->data, item8->data);
    


    NODE node4 = createNewNode(false);
    node4->numChildren = 3;
    node4->children[0] = node1;
    node4->children[1] = node2;
    node4->children[2] = node3;
    node4->rects[0] = createNewRect(item1->data, item3->data);
    node4->rects[1] = createNewRect(item4->data, item6->data);
    node4->rects[2] = createNewRect(item7->data, item8->data);

    node1->parent = node4;
    node2->parent = node4;
    node3->parent = node4;

    RTREE r = createNewRtree();
    r->root = node4;
    r->count = 8;
    r->rect = findMBR(node4);

    // // testing pickseeds_item
    // ITEM seedList[5] = {item1, item2, item3, item4, item5};
    // ITEM first[2];
    // pickseeds_item(seedList, first);
    // for(int i = 0; i < 2; i++) {
    //     printItem(first[i]);
    // }


    // // testing picknext_item
    // picknext_item(node1, node2, item9);
    // printNode(node1);
    // printNode(node2);

    // testing pickseeds_node
    // int p1[2] = {0, 0};
    // int p2[2] = {1, 2};
    // int p3[2] = {2, 2};
    // int p4[2] = {4, 0};
    // int p5[2] = {5, 0};
    // int p6[2] = {6, 2};

    // MBR r1 = createNewRect(p1, p3);
    // MBR r2 = createNewRect(p4, p6);
    // MBR r3 = createNewRect(p1, p2);
    // MBR r4 = createNewRect(p5, p6);

    // NODE n1 = createNewNode(false);
    // NODE n2 = createNewNode(false);
    // NODE n3 = createNewNode(false);
    // NODE n4 = createNewNode(false);

    // NODE seedList[4] = {n1, n2, n3, n4};
    // MBR mbrList[4] = {r1, r2, r3, r4};
    // int index[2];
    // // pickseeds_node(seedList, mbrList, index);

    // // testing leafQuadSplit()
    // node1->numChildren++;
    // node1->items[3] = item4;
    // node1->rects[3] = createNewRect(item4->data, item4->data);
    // NODE N1 = createNewNode(true);
    // NODE N2 = createNewNode(true);
    // LeafQuadraticSplit(node1, item5, N1, N2);
    // printNode(N1);
    // printNode(N2);

    
    MBR mbrold = findMBR(node1);
    // node1->numChildren++;
    // node1->items[3] = item9;
    // node1->rects[3] = createNewRect(item9->data, item9->data);
    // printf("Node before sending:\n");
    // printNode(node1);
    
    NODE node5 = createNewNode(true);
    node5->numChildren = 2;
    node5->items[0] = item3;
    node5->items[1] = item9;
    node5->rects[0] = createNewRect(item3->data, item3->data);
    node5->rects[1] = createNewRect(item9->data, item9->data);
    adjustTree(r, node1, node5, mbrold);
    


    return 0;
}
