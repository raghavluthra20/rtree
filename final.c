#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DIMS 2
#define M 4
#define m 2
#define INT_MAX 9999999
#define INT_MIN -9999999

int min(int a, int b)
{
    return a < b ? a : b;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

typedef struct mbr *MBR;
struct mbr
{
    int bottomLeft[DIMS];
    int topRight[DIMS];
};

typedef struct item *ITEM;
struct item
{
    int data[DIMS];
};

typedef struct node *NODE;
struct node
{
    bool isLeaf;
    NODE parent;

    // for leaf nodes this represents number of items
    // for internal nodes this represents number of children
    int numChildren;
    MBR rects[M]; // MBRs for all children/items
    NODE children[M];
    ITEM items[M];
};

typedef struct rtree *RTREE;
struct rtree
{
    int count; // total number of items (data objects)
    NODE root;
    MBR rect;
};

NODE createNewNode(bool isLeaf)
{
    NODE myNode = (NODE)malloc(sizeof(struct node));
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

MBR createNewRect(int bottomLeft[DIMS], int topRight[DIMS])
{
    MBR rect = (MBR)malloc(sizeof(struct mbr));
    for (int d = 0; d < DIMS; d++)
    {
        rect->bottomLeft[d] = bottomLeft[d];
        rect->topRight[d] = topRight[d];
    }
    return rect;
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

int calc_redundancy(MBR m1, MBR m2)
{
    return (findRectArea(mergeRect(m1, m2)) - (findRectArea(m2) + findRectArea(m2)));
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
    NODE first[2];
    // int indexes[2];
    int i, j;
    long long max_redundancy;
    first[0] = seedlist[0];
    first[1] = seedlist[1];
    indexes[0] = 0;
    indexes[1] = 1;
    max_redundancy = calc_redundancy(seedmbr[0], seedmbr[1]);
    for (i = 0; i < M + 1; i++)
    {
        for (j = i; j < M + 1; j++)
        {
            if (calc_redundancy(seedmbr[i], seedmbr[j]) > max_redundancy)
            {
                first[0] = seedlist[i];
                first[1] = seedlist[j];
                indexes[0] = i;
                indexes[1] = j;
                max_redundancy = calc_redundancy(seedmbr[0], seedmbr[1]);
                ;
            }
        }
    }
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
NODE chooseLeaf(RTREE r, ITEM i)
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
        item[d] = i->data[d];
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
            long long area = findRectArea(n->rects[i]);
            if (area < minArea)
            {
                reqIndex = i;
            }
        }
        n = n->children[reqIndex];
    }
    return n;
}

/////// TO CHECK FOR 4-1 SPLIT IN THE NODE ///////////////////////////////////////////////

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

void adjustTree(RTREE r, NODE L, NODE LL, MBR mx)
{
    NODE p = L->parent;
    // int req_ind =0;
    MBR mr = findMBR(L);
    if (LL == NULL) // node has not been splitted
    {
        
        for (int i = 0; i < M; i++)
        {
            if (p->rects[i] == mx)
            {
                p->rects[i] = mr;
                break;
            }
        }
    }
    else     // Case for node splitting in L and LL.
    {
        MBR mrb = findMBR(LL);
        for (int i = 0; i < M; i++) // updating the MBR for the L node 
        {
            if (p->rects[i] == mx)
            {
                p->rects[i] = mr;
                break;
            }
        }  
        if(p->numChildren < M) // adding mbr for LL node if vacancy is available 
        {
            p->rects[p->numChildren] = mrb;
            p->numChildren++;
        }
        else // adding mbr for LL node if vacancy is NOT available 
        {
            NODE j = createNewNode(true);
            j->rects[0] = mrb;
            NODE N = createNewNode(true);
            NODE NN = createNewNode(true);
             /////// CHECK THIS CONDITION//////////////////////
            
            MBR mq = findMBR(p);
            InternalQuadraticSplit(p, j, N, NN);
            adjustTree(r, N, NN, mq);//p = p->parent;
            
        }
        

    }
}

// RTREE adjustNode(RTREE r, NODE L, NODE LL)
// {
//     NODE n = L;
//     while (n->parent != NULL)
//     {
//         NODE p = n->parent;
//     }

//     return r;
// }

// void adjustTree(RTREE r, NODE L, NODE LL, MBR mx)
// {
//     if (L->isLeaf)
//         adjustLeaf(r, L, LL, mx);
//     else
//         adjustNode(r, L, LL);
// }

void insert(RTREE r, ITEM i)
{
    NODE L = createNewNode(true);
    NODE LL = createNewNode(true);

    NODE n = chooseLeaf(r, i);
    //while(n->parent!= NULL)
    //{
        MBR mx = findMBR(n);
        split(n, L, LL, i);
        adjustTree(r, L, LL, mx);
       // n = n->parent;
   // }
}


int main()
{
    printf("Hello world\n");

    return 0;
}
