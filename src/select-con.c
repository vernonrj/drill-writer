#include "drill.h"

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
	select_t *selects;
	select_t *last;
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

void update_sel_center(void)
{
	// update the center of the form based on dot selection
	// TODO: Use heaps to keep track of max, min values?
	int index;
	int selnum = 0;
	double cx, cy;
	select_t *last;
	coord_t **coords;
	coord_t *coord;

	cx = 0;
	cy = 0;
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


void add_sel_center(coord_t *coord)
{
	// add a selection to center weight
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



void rem_sel_center(coord_t *coord)
{
	// remove a selection from center weight
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


void select_dots_add(int index)
{
	// wrapper to add a dot to global selection
	pstate.select = select_add(pstate.select, index, true);
	update_sel_center();
	return;
}

select_t *select_add(select_t *psel, int index, bool toggle)
{
	// add a selection if it's not selected;
	// remove a selection if it is selected and toggle is TRUE
	
	select_t *selects;
	select_t *last;
	select_t *curr;
	int matched = 0;
	int loop_done = 0;

	if (!psel)
	{
		// no selection yet
		curr = (select_t*)malloc(sizeof(select_t));
		if (!curr)
			return NULL;
		curr->next = 0;
		curr->index = index;
		psel = curr;
		// update selection center
		add_sel_center(pshow->sets->currset->coords[index]);
	}
	else
	{
		// have some selections; 
		// add inorder or remove if already selected
		last = psel;
		selects = 0;
		while (loop_done == 0)
		{
			// check for grouping
			if (last->index == index)
			{
				// found a match, remove if toggle enabled
				if (toggle && selects == NULL)
				{
					// match is first node
					psel = last->next;
					free(last);
				}
				else if (toggle)
				{
					// match is not first node, remove
					selects->next = last->next;
					free(last);
				}
				loop_done = 1;
				matched = 1;
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
			curr = (select_t*)malloc(sizeof(select_t));
			if (!curr)
				return NULL;
			curr->index = index;
			if (selects == NULL)
			{
				// insert at beginning
				curr->next = pstate.select;
				psel = curr;
			}
			else
			{
				// insert in the middle
				curr->next = selects->next;
				selects->next = curr;
			}
		}
	}
	return psel;
}



select_t *select_add_group(select_t *selects, group_t *group, bool toggle)
{
	// add group to selection
	select_t *last;
	if (!group)
		return selects;
	last = group->selects;
	while (last)
	{
		selects = select_add(selects, last->index, toggle);
		last = last->next;
	}
	return selects;
}



int select_all(void)
{
	// select all dots
	int i;
	int perfnum;
	perf_t **perfs;
	select_discard(pstate.select);
	perfnum = pshow->perfnum;
	perfs = pshow->perfs;
	for (i=0; i<perfnum; i++)
	{
		if (perfs[i]->valid != 0)
		{
			// performer is valid. Add
			select_dots_add(i);
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



group_t *group_add_selects(group_t *group, select_t *newsels)
{
	// add selects to group
	select_t *scurr = newsels;
	select_t *glast;
	select_t *select_new = NULL;

	if (!group)
		group = group_construct();
	glast = group->selects;

	while (scurr && glast)
	{
		// merge selection and group selects in order
		if (scurr->index < glast->index)
		{
			// selection goes next
			select_new = select_add(select_new, scurr->index, false);
			scurr = scurr->next;
		}
		else if (scurr->index > glast->index)
		{
			// group goes next
			select_new = select_add(select_new, glast->index, false);
			glast = glast->next;
		}
		else
		{
			// duplicate
			scurr = scurr->next;
		}
	}
	// clean up
	while (scurr)
	{
		select_new = select_add(select_new, scurr->index, false);
		scurr = scurr->next;
	}
	while (glast)
	{
		select_new = select_add(select_new, glast->index, false);
		glast = glast->next;
	}
	// add new selection to group
	select_discard(group->selects);
	group->selects = select_new;
	return group;
}



bool is_in_select(int index, select_t *selects)
{
	select_t *last = selects;
	while (last)
	{
		if (index == last->index)
			return true;
		last = last->next;
	}	
	return false;
}



int add_group(void)
{
	// Make a new group
	return 0;
}
