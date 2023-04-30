
int area (int a1, int a2, int b1, int b2)
{
	return abs((a1-a2)*(b1-b2));
}

bool isequal(ITEM n1, ITEM n2)
{
	int i;
	for(i=0;i<DIMS;i++)
	{
		if(n1->data[i] != n2->data[i])
		{
			return false;
		}
	}
	return true;
}


int calc_redundancy(MBR m1, MBR m2)
{
	return (findRectArea(mergeRect(m1, m2)) - (findRectArea(m2)+findRectArea(m2)));
}

void LeafQuadraticSplit(NODE n, ITEM i)
{
	ITEM seeds[M+1], picked_seeds[2];
	int j, k;
	NODE n1 = createNewNode(true);
	NODE n2 = createNewNode(true);
	
	for( j =0;j<M;j++)
	{
		seeds[j] = n->items[j];
	}
	seeds[M] = i;
	
    ///////////////CHECK THIS PICKED_SEEDS/////////////
	picked_seeds[1] = pickseeds_item(seeds);
	
	n1->parent = n->parent;
	n2->parent = n->parent;
	n1->items[0] = picked_seeds[0];
	n2->items[0] = picked_seeds[1];
	n1->numChildren++;
	n2->numChildren++;
	
	int flag0=0, flag1=0;
	int Updated_M;
	for(j = 0; j<=M;j++)
	{
		if(flag0 == 0 && isequal(seeds[j],picked_seeds[0]))
		{
			flag0 = 1;
			Updated_M = M + 1 -(flag0+flag1);
			for(k=j;k<Updated_M;k++)
			{
				seeds[k] = seeds[k+1];
			}
		}
		else
		if(flag1 == 0 && isequal(seeds[j],picked_seeds[1]))
		{
			flag1 = 1;
			Updated_M = M + 1 -(flag0+flag1);
			for(k=j;k<Updated_M;k++)
			{
				seeds[k] = seeds[k+1];
			}
		}
	}
	
	for(k=0;k<Updated_M;k++)
	{
		if(!picknext_item(n1,n2,seeds[k]))
			printf("Split Problem: Unable to insert element into a new node");
	}
	
	//adjustments in parent OR return the 2 nodes
	
	//destroy node n
	
	
	
}

void InternalQuadraticSplit(NODE n, NODE i)
{
	NODE seeds[M+1], picked_seeds[2];
	MBR seedsmbr[M+1];
	int j,k;
	int picked_seeds_indexes[2];
	NODE n1 = createNewNode(true);
	NODE n2 = createNewNode(true);
	for( j =0;j<M;j++)
	{
		seeds[j] = n->children[j];
		seedsmbr[j] = n->rects[j];
	}
	seeds[M] = i;
	seedsmbr[M] = findMBR(i);
    
	picked_seeds_indexes = pickseeds_node(seeds,seedsmbr);
	
	n1->parent = n->parent;
	n2->parent = n->parent;
	n1->children[0] = seeds[picked_seeds_indexes[0]];
	n2->children[0] = seeds[picked_seeds_indexes[1]];
	n1->numChildren++;
	n2->numChildren++;
	
	int flag0=0, flag1=0;
	int Updated_M;
	for(j = 0; j<=M;j++)
	{
		if(flag0 == 0 && j == picked_seeds_indexes[0])
		{
			flag0 = 1;
			Updated_M = M + 1 -(flag0+flag1);
			for(k=j;k<Updated_M;k++)
			{
				seeds[k] = seeds[k+1];
			}
		}
		else
		if(flag1 == 0 && j == picked_seeds_indexes[1])
		{
			flag1 = 1;
			Updated_M = M + 1 -(flag0+flag1);
			for(k=j;k<Updated_M;k++)
			{
				seeds[k] = seeds[k+1];
			}
		}
	}
	
	for(k=0;k<Updated_M;k++)
	{
		if(!picknext_child(n1,n2,seeds[k]))
			printf("Split Problem: Unable to insert element into a new node");
	}
	
	
	//adjustments in parent OR return the 2 nodes
	
	//destroy node n
	

}




ITEM pickseeds_item(ITEM seedlist)
{
	ITEM first[2];
	int i,j, max_area;
	first[0] = seedlist[0];
	first[1] = seedlist[1];
	max_area = area(first[0].x,first[1].x,first[0].y,first[1].y);
	for(i = 0;i<M+1;i++)
	{
		for(j=i;j<M+1;j++)
		{
			if( area(seedlist[i].x,seedlist[j].x, seedlist[i].y,seedlist[j].y) > max_area)
			{
				first[0] = seedlist[i];
				first[1] = seedlist[j];
				max_area = area(first[0].x,first[1].x,first[0].y,first[1].y);				
			}
		}
	}
	return first;
}

int* pickseeds_node( NODE seedlist, MBR seedmbr)
{
	NODE first[2];
	int indexes[2];
	int i,j;
	long long max_area;
	first[0] = seedlist[0];
	first[1] = seedlist[1];
	indexes[0] = 0;
	indexes[1] = 1;
	max_area = findRectArea(mergeRect(seedmbr[0],seedmbr[1]));
	for(i = 0;i<M+1;i++)
	{
		for(j=i;j<M+1;j++)
		{
			if( findRectArea(mergeRect(seedmbr[i],seedmbr[j]) > max_area))
			{
				first[0] = seedlist[i];
				first[1] = seedlist[j];
				indexes[0] = i;
				indexes[1] = j;
				max_area = findRectArea(mergeRect(seedmbr[0],seedmbr[1]));				
			}
		}
	}
	return indexes;
}


int picknext_item( NODE n1, NODE n2, ITEM i)
{
	MBR mi = createNewRect(i,i);
	MBR m1 = findMBR(n1);
	MBR m2 = findMBR(n2);
	if( findRectArea(mergeRect(m1, mi)) < findRectArea(mergeRect(m2,mi)))
	{
		n1->items[n1->numChildren] = i;
		n1->numChildren++;
		return 1;
	}
	else if( findRectArea(mergeRect(m1, mi)) > findRectArea(mergeRect(m2,mi)))
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

int picknext_child( NODE n1, NODE n2, NODE ni)
{
	MBR mi = findMBR(ni);
	MBR m1 = findMBR(n1);
	MBR m2 = findMBR(n2);
	if( calc_redundancy(m1,mi) < calc_redundancy(m2,mi))
	{
		n1->children[n1->numChildren] = ni;
		n1->numChildren++;
		return 1;
	}
	else if( calc_redundancy(m1,mi) > calc_redundancy(m2,mi))
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


RTREE adjustLeaf(RTREE r, NODE L)
{
    NODE n = L;
    while(n->parent != NULL)
    {
        NODE p = n->parent;

    }
}
