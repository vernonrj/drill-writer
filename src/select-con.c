#include "drill.h"

void update_sel_center(void)
{
	// update the center of the form based on dot selection
	// TODO: Use heaps to keep track of max, min values?
	int index;
	int selnum = 0;
	double cx, cy;
	//double minx, miny, maxx, maxy;
	struct select_proto *last;
	struct coord_proto **coords;
	struct coord_proto *coord;

	cx = 0;
	cy = 0;
	/*
	minx = -1;
	miny = -1;
	maxx = 0;
	maxy = 0;
	*/
	last = pshow->select;
	coords = pshow->sets->currset->coords;
	while (last)
	{
		// get coordinates for selected dot
		index = last->index;
		coord = coords[index];
		cx = cx + coord->x;
		cy = cy + coord->y;
		selnum++;
		last = last->next;
	}
	if (selnum == 0)
	{
		// no selections
		cx = 0;
		cy = 0;
	}
	else
	{
		// have selections
		cx = cx / selnum;
		cy = cy / selnum;
	}
	// store
	pshow->center->x = cx;
	pshow->center->y = cy;
	pshow->selnum = selnum;

	return;
}


void add_sel_center(struct coord_proto *coord)
{
	// add a selection to center weight
	//int index;
	int selnum;
	double x, y;
	
	// open up weight
	selnum = pshow->selnum;
	x = pshow->center->x * selnum;
	y = pshow->center->y * selnum;

	// add new dot to center
	x = x + coord->x;
	y = y + coord->y;
	selnum++;

	// take mean of new set
	pshow->center->x = x / selnum;
	pshow->center->y = y / selnum;
	pshow->selnum = selnum;

	return;
}



void rem_sel_center(struct coord_proto *coord)
{
	// remove a selection from center weight
	//int index;
	int selnum;
	double x, y;

	// open up weight
	selnum = pshow->selnum;
	if (selnum == 0)
		return;
	else if (selnum == 1)
	{
		pshow->selnum = 0;
		pshow->center->x = 0;
		pshow->center->y = 0;
		return;
	}
	x = pshow->center->x * selnum;
	y = pshow->center->y * selnum;

	// remove dot from center
	x = x - coord->x;
	y = y - coord->y;
	selnum--;

	// take mean of new set if not 0
	pshow->center->x = x / selnum;
	pshow->center->y = y / selnum;
	pshow->selnum = selnum;
	
	return;
}

void select_discard(void)
{
	// remove all selections from selection list
	struct select_proto *selects;
	struct select_proto *last;
	if (pshow->select != 0)
	{
		selects = pshow->select;
		last = pshow->select;
		while (selects != NULL)
		{
			// remove
			last = selects->next;
			free(selects);
			selects = last;
		}
		pshow->select = 0;
	}
	pshow->selnum = 0;
	update_sel_center();
	return;
}

int select_add(int index)
{
	// add a selection if it's not selected;
	// remove a selection if it is selected
	
	struct select_proto *selects;
	struct select_proto *last;
	struct select_proto *curr;
	//struct coord_proto *coord;
	int matched = 0;
	int loop_done = 0;

	if (!pshow->select)
	{
		// no selection yet
		curr = (struct select_proto*)malloc(sizeof(struct select_proto));
		if (!curr)
			return -1;
		curr->next = 0;
		curr->index = index;
		pshow->select = curr;
		// update selection center
		add_sel_center(pshow->sets->currset->coords[index]);
	}
	else
	{
		// have some selections; 
		// add inorder or remove if already selected
		last = pshow->select;
		selects = 0;
		while (loop_done == 0)
		{
			if (last->index == index)
			{
				// found a match, toggle off
				if (selects == NULL)
				{
					// match is first node
					pshow->select = last->next;
					free(last);
				}
				else
				{
					// match is not first node, remove
					selects->next = last->next;
					free(last);
				}
				loop_done = 1;
				matched = 1;
				rem_sel_center(pshow->sets->currset->coords[index]);
			}
			else if (last->index > index)
			{
				// no match found
				// exit to create new node
				loop_done = 1;
				matched = 0;
			}
			else
			{
				// continue searching
				selects = last;
				last = last->next;
				if (last == NULL)
					loop_done = 1;
			}
		}
		if (matched == 0)
		{
			// create a new node
			curr = (struct select_proto*)malloc(sizeof(struct select_proto));
			if (!curr)
				return -1;
			curr->index = index;
			if (selects == NULL)
			{
				// insert at beginning
				curr->next = pshow->select;
				pshow->select = curr;
			}
			else
			{
				// insert in the middle
				curr->next = selects->next;
				selects->next = curr;
			}
		}
		add_sel_center(pshow->sets->currset->coords[index]);
	}
	return 0;
}

int select_all(void)
{
	// select all dots
	int i;
	int perfnum;
	struct perf_proto **perfs;
	select_discard();
	perfnum = pshow->perfnum;
	perfs = pshow->perfs;
	for (i=0; i<perfnum; i++)
	{
		if (perfs[i]->valid != 0)
		{
			// performer is valid. Add
			select_add(i);
		}
	}
	return 0;
}


int add_group(void)
{
	// make a group from selected dots
	struct select_proto *select;
	struct select_proto *gselect;
	struct select_proto *glast;
	struct select_proto *clast;
	struct select_proto *ccurr;
	struct select_proto *cdum;
	// grouping
	struct group_container *groups;
	struct group_proto *group;

	groups = pshow->sets->currset->groups;
	clast = groups->include;
	ccurr = groups->include;
	group = (struct group_proto*)malloc(sizeof(struct group_proto));
	if (!group)
		return -1;
	if (!groups->head)
	{
		groups->head = group;
		group->next = 0;
	}
	else
	{
		group->next = groups->head;
		groups->head = group;
	}
	select = pshow->select;
	gselect = 0;
	while (select != NULL)
	{
		// copy selects
		glast = (struct select_proto*)malloc(sizeof(struct select_proto));
		if (!glast)
			return -1;
		if (gselect)
		{
			// has head for group
			gselect->next = glast;
		}
		else
		{
			// no head for group yet
			group->selects = glast;
		}
		gselect = glast;
		gselect->next = 0;
		gselect->index = select->index;
		if (clast)
		{
			// not at the end of current data
			if (select->index < clast->index)
			{
				// make a new node for this one
				cdum = (struct select_proto*)malloc(
						sizeof(struct select_proto));
				if (!cdum)
					return -1;
				cdum->index = select->index;
				cdum->next = clast;
				ccurr->next = cdum;
				ccurr = cdum;
			}
			else if (select->index > clast->index)
			{
				ccurr = clast;
				clast = clast->next;
			}
		}
		else if (ccurr)
		{
			// have data indexed, but these
			// dots are definitely new
			cdum = (struct select_proto*)malloc(
					sizeof(struct select_proto));
			if (!cdum)
				return -1;
			cdum->index = select->index;
			ccurr->next = cdum;
			clast = cdum;
			clast->next = 0;
		}
		else
		{
			// build list
			groups->include = (struct select_proto*)malloc(
					sizeof(struct select_proto));
			if (!groups->include)
				return -1;
			groups->include->next = 0;
			groups->include->index = select->index;
			clast = 0;
			ccurr = groups->include;
		}
		select = select->next;
	}
	return 0;
}




