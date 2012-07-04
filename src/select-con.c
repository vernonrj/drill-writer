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
	last = pstate.select;
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
	pstate.center->x = cx;
	pstate.center->y = cy;
	pstate.selnum = selnum;

	return;
}


void add_sel_center(struct coord_proto *coord)
{
	// add a selection to center weight
	//int index;
	int selnum;
	double x, y;
	
	// open up weight
	selnum = pstate.selnum;
	x = pstate.center->x * selnum;
	y = pstate.center->y * selnum;

	// add new dot to center
	x = x + coord->x;
	y = y + coord->y;
	selnum++;

	// take mean of new set
	pstate.center->x = x / selnum;
	pstate.center->y = y / selnum;
	pstate.selnum = selnum;

	return;
}



void rem_sel_center(struct coord_proto *coord)
{
	// remove a selection from center weight
	//int index;
	int selnum;
	double x, y;

	// open up weight
	selnum = pstate.selnum;
	if (selnum == 0)
		return;
	else if (selnum == 1)
	{
		pstate.selnum = 0;
		pstate.center->x = 0;
		pstate.center->y = 0;
		return;
	}
	x = pstate.center->x * selnum;
	y = pstate.center->y * selnum;

	// remove dot from center
	x = x - coord->x;
	y = y - coord->y;
	selnum--;

	// take mean of new set if not 0
	pstate.center->x = x / selnum;
	pstate.center->y = y / selnum;
	pstate.selnum = selnum;
	
	return;
}

void select_dots_discard(void)
{
	// remove all dot selections from state
	pstate.select = select_discard(pstate.select);
	pstate.selnum = 0;
	update_sel_center();
	return;
}

select_t *select_discard(select_t *psel)
{
	// remove all selections from selection list
	struct select_proto *selects;
	struct select_proto *last;
	if (psel != 0)
	{
		selects = psel;
		last = psel;
		while (selects != NULL)
		{
			// remove
			last = selects->next;
			free(selects);
			selects = last;
		}
		psel = 0;
	}
	return psel;
}

select_t *select_add(select_t *psel, int index)
{
	// add a selection if it's not selected;
	// remove a selection if it is selected
	
	struct select_proto *selects;
	struct select_proto *last;
	struct select_proto *curr;
	//struct coord_proto *coord;
	int matched = 0;
	int loop_done = 0;

	if (!psel)
	{
		// no selection yet
		curr = (struct select_proto*)malloc(sizeof(struct select_proto));
		if (!curr)
			return NULL;
		curr->next = 0;
		curr->index = index;
		//pstate.select = curr;
		psel = curr;
		if (select_in_group(index))
			printf("%i in group\n", index);
		// update selection center
		add_sel_center(pshow->sets->currset->coords[index]);
	}
	else
	{
		// have some selections; 
		// add inorder or remove if already selected
		//last = pstate.select;
		last = psel;
		selects = 0;
		while (loop_done == 0)
		{
			// check for grouping
			if (select_in_group(last->index))
				printf("%i in group\n", last->index);
			if (last->index == index)
			{
				// found a match, toggle off
				if (selects == NULL)
				{
					// match is first node
					//pstate.select = last->next;
					psel = last->next;
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
				//rem_sel_center(pshow->sets->currset->coords[index]);
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
				return NULL;
			curr->index = index;
			if (selects == NULL)
			{
				// insert at beginning
				curr->next = pstate.select;
				//pstate.select = curr;
				psel = curr;
			}
			else
			{
				// insert in the middle
				curr->next = selects->next;
				selects->next = curr;
			}
		}
		//add_sel_center(pshow->sets->currset->coords[index]);
	}
	return psel;
}

int select_all(void)
{
	// select all dots
	int i;
	int perfnum;
	struct perf_proto **perfs;
	select_discard(pstate.select);
	perfnum = pshow->perfnum;
	perfs = pshow->perfs;
	for (i=0; i<perfnum; i++)
	{
		if (perfs[i]->valid != 0)
		{
			// performer is valid. Add
			pstate.select = select_add(pstate.select, i);
		}
	}
	return 0;
}


group_t *group_construct(void)
{
	// make a group node
	group_t *group;
	group = (group_t*)malloc(sizeof(group_t));
	if (!group)
		return NULL;
	group->selects = NULL;
	group->forms = NULL;
	group->next = NULL;

	return group;
}


/*
int group_add_selects(group_t *group, select_t *newsels)
{
	// add selects to group
	select_t *scurr = newsels;
	select_t *glast = group->selects;
	select_t *smerged = NULL;
	select_t *smer_head = NULL;
	select_t *select_new;

	while (scurr && glast)
	{
		if (scurr->index < ghead->index)
		{
			// add new selection here
			select_new = scurr->index;
			scurr = scurr->next;
		}
		else if (scurr->index > ghead->index)
		{
			select_new = ghead->index;
			ghead = ghead->next;
		}
		else
		{
			scurr = scurr->next;
		}
		if (select_new && !smer_head)
		{
			smer_head = select_new;


*/




int add_group(void)
{
	/*
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
	ccurr = groups->include;
	if (ccurr)
		clast = ccurr->next;
	else
		clast = 0;
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
	select = pstate.select;
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
			while (select->index > clast->index)
			{
				ccurr = clast;
				clast = clast->next;
				if (!clast)
					break;
			}
		}
		if (clast)
		{
			if (select->index < ccurr->index)
			{
				// make a new node at the beginning
				cdum = (struct select_proto*)malloc(
						sizeof(struct select_proto));
				if (!cdum)
					return -1;
				cdum->index = select->index;
				cdum->next = ccurr;
				groups->include = cdum;
				clast = ccurr;
				ccurr = cdum;
			}
			else if (select->index < clast->index)
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
			ccurr = cdum;
			ccurr->next = 0;
		}
		else
		{
			// build list
			groups->include = (struct select_proto*)malloc(
					sizeof(struct select_proto));
			if (!groups->include)
				return -1;
			ccurr = groups->include;
			ccurr->next = 0;
			ccurr->index = select->index;
			clast = 0;
		}
		select = select->next;
	}
	*/
	return 0;
}


int select_in_group(int index)
{
	/*
	// return if the dot is in a group or not
	int ingroup = 0;
	struct select_proto *select;

	select = pshow->sets->currset->groups->include;
	while (select != NULL)
	{
		if (select->index == index)
		{
			ingroup = 1;
			break;
		}
		select = select->next;
	}
	return ingroup;
	*/
	return 0;
}


