# rtree.c

An [R-tree](https://www.cs.princeton.edu/courses/archive/fall08/cos597B/papers/rtrees.pdf) implementation in C. 


## Command to run the code

The `DSA_assignment_group_21.c` file contains tests and benchmarks.
To compile : 
```sh
$ gcc DSA_assignment_group_21.c -o DSA_assignment_group_21
```
To run the executable file
```sh
$ ./DSA_assignment_group_21
```

## Generic interface

The `DSA_assignment_group_21.c` file can be easily customized to change these settings.

Please find the type parameters at the top of the `DSA_assignment_group_21.c` file:

```c
#define DIMS 2
#define M 4
#define m 2
#define INT_MAX 9999999
#define INT_MIN -9999999
```
Change these to suit your needs.


## Functions

```
insert            #inserting an data point in the rtree
chooseLeaf        #choosing the appropriate node for the new data point
split             #spliting the node or the leaf of the rtree
adjustTree        #adjusting the tree by updating its MBR's
traverse          #traversing the obtained tree
search            #searching the rtree
```

## Algorithms

This implementation is a variant of the original paper:  
[R-TREES. A DYNAMIC INDEX STRUCTURE FOR SPATIAL SEARCHING](https://www.cs.princeton.edu/courses/archive/fall08/cos597B/papers/rtrees.pdf)

### Insertion

Implemented the pseudo code given in the original paper. From the root to the leaf, the rects which will incur the least enlargment are chosen. Ties go to rects with the smallest area. 
chooseLeaf takes care of picking up the appropriate node for the item. 
split funciton takes care of splitting the nodes and holding the properties of the rtree.
adjustTree funciton takes care of adjusting all the nodes and their MBR values and handling the links to children and parents in the rtree. 

### Search

Same as the original algorithm.

### Traverse 
We are printing the rTree in Pre-Order Traversal. 


## Some of the real-life applications are 

- Indexing multi-dimensional information.
- Handling geospatial coordinates.
- Implementation of virtual maps.
- Handling game data.

## Properties

- Rtree consists of a single root, internals nodes, and the leaf nodes.
- The root contains the pointer to the largest region in the spatial domain.
- Parent nodes contains pointers to their child nodes where the region of child nodes completely overlaps the regions of parent nodes.
- Leaf nodes contains data about the MBR to the current objects.
- MBR-Minimum bounding region refers to the minimal bounding box parameter surrounding the region/object under consideration.