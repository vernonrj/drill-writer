#include "drillwriter.h"

// selection LLL node
struct select_proto
{
	// node with selection information
	// can contain a single dot, or an entire form
	// (not both)
	int index;		// performer 
	form_child_t *form;		// form

	select_t *next;
};



select_t *select_construct(void)
{
	select_t *select;
	select = (select_t*)malloc(sizeof(select_t));
	if (!select)
		return NULL;
	select->index = -1;
	//select->index = index;
	select->form = NULL;
	select->next = NULL;
	return select;
}


select_t *select_construct_with_index(int index)
{
	select_t *select;
	select = select_construct();
	if (select)
		select->index = index;
	return select;
}


int select_has_next(select_t *select)
{
	if (!select)
		return -1;
	return (select->next != NULL);
}


select_t *select_get_next(select_t *select)
{
	if (!select)
		return NULL;
	return select->next;
}


void select_set_next(select_t *select, select_t *last)
{
	select->next = last;
	return;
}


int select_has_index(select_t *select)
{
	if (!select)
		return -1;
	return (select->index != -1);
}

int select_get_dot(select_t *select)
{
	if (!select)
		return -1;
	return select->index;
}


int select_has_form(select_t *select)
{
	if (!select)
		return -1;
	return (select->form != NULL);
}

form_child_t *select_get_form(select_t *select)
{
	if (!select)
		return NULL;
	return select->form;
}


bool select_check_index_selected(int index, select_t *selects)
{
	// check to see if dot is in list of selects
	select_t *last = selects;
	while (last)
	{
		if (index == last->index)
			return true;
		last = last->next;
	}	
	return false;
}






select_t *select_add_index(select_t *psel, int index, bool toggle)
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
		curr = select_construct_with_index(index);
		psel = curr;
		// update selection center
		//add_sel_center(pshow->sets->currset->coords[index]);
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
			if (!last->form && last->index == index)
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
			curr = select_construct_with_index(index);
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



void select_dots_add_index(int index)
{
	// wrapper to add a dot to global selection
	pstate.select = select_add_index(pstate.select, index, true);
	//update_sel_center(pstate.select);
	select_update_center(pstate.select);
	return;
}



select_t *select_add_group(select_t *select, group_t *group)
{
	select_t *group_selects;
	if (!group)
		return select;
	group_selects = group->selects;
	while (group_selects)
	{
		select = select_add_index(select, group_selects->index, false);
		group_selects = group_selects->next;
	}
	return select;
}
	


select_t *select_add_form(select_t *selects, form_child_t *form, bool toggle)
{
	// add group to selection
	select_t *last;
	select_t *curr;
	if (!form)
		return selects;
	last = selects;
	curr = NULL;
	while (last)
	{
		if (last->form && last->form == form)
			return selects;
		curr = last;
		last = last->next;
	}
	if (curr == NULL)
	{
		selects = select_add_index(selects, -1, false);
		selects->form = form;
		return selects;
	}
	last = NULL;
	last = select_add_index(last, -1, toggle);
	last->form = form;
	curr->next = last;
	return selects;
}



void select_add_multiple(select_t **mainlist_r, select_t **modifier_r, bool toggle)
{
	// add multiple nodes
	select_t *mainlist, *modifier;
	mainlist = *mainlist_r;
	modifier = *modifier_r;
	select_t *last = modifier;

	while (last)
	{
		mainlist = select_add_index(mainlist, last->index, toggle);
		last = last->next;
		free(modifier);
		modifier = last;
	}
	*mainlist_r = mainlist;
	*modifier_r = modifier;
	return;
}






select_t *select_drop_multiple(select_t *mainlist, select_t *modifier)
{
	// return mainlist's original entries.
	// Drop everything else
	select_t *last = mainlist;
	select_t *inc = modifier;

	select_t *newlist = NULL;

	if (!mainlist)
		return NULL;
	if (!modifier)
	{
		while (last)
		{
			newlist = select_add_index(newlist, last->index, false);
			if (last->form)
				newlist->form = last->form;
			last = last->next;
		}
	}



	while (last && inc)
	{
		while (inc->next && inc->index < last->index)
			inc = inc->next;
		if (last->index != inc->index)
			newlist = select_add_index(newlist, last->index, false);
		last = last->next;
	}
	return newlist;
}




select_t *select_discard(select_t *psel)
{
	// remove all selections from selection list
	select_t *selects;
	select_t *last;
	if (psel != NULL)
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



void select_dots_discard(void)
{
	// remove all dot selections from state
	pstate.select = select_discard(pstate.select);
	pstate.selnum = 0;
	select_update_center(pstate.select);
	return;
}



select_t *select_push(select_t *mainlist, select_t **modifier_r, bool toggle)
{
	// push an item from the modifier to the mainlist
	select_t *modifier;
	select_t *last;

	modifier = *modifier_r;

	if (modifier)
	{
		if (modifier->form)
			last = select_add_form(mainlist, modifier->form, toggle);
		else
			last = select_add_index(mainlist, modifier->index, toggle);
		*modifier_r = modifier->next;
		free(modifier);
		return last;
	}
	else
		return mainlist;
}


void select_push_all(select_t **mainlist_r, select_t **modifier_r, bool toggle)
{
	select_t *mainlist, *modifier;
	mainlist = *mainlist_r;
	modifier = *modifier_r;

	while (modifier)
		mainlist = select_push(mainlist, &modifier, toggle);
	*modifier_r = modifier;
	*mainlist_r = mainlist;
	return;
}




int select_all_dots(void)
{
	pstate.select = select_all(pstate.select, pshow->perfs, pshow->perfnum);
	return 0;
}



select_t *select_all(select_t *selects, perf_t **perfs, int perfnum)
{
	// select all dots
	int i;
	//int perfnum;
	//perf_t **perfs;
	//select_discard(pstate.select);
	selects = select_discard(selects);
	//perfnum = pshow->perfnum;
	//perfs = pshow->perfs;
	for (i=0; i<perfnum; i++)
	{
		if (perfs[i]->valid != 0)
		{
			// performer is valid. Add
			selects = select_add_index(selects, i, true);
		}
	}
	select_update_center(selects);
	return selects;
}



void select_update_center(select_t *last)
{
	// update the center of the form based on dot selection
	int index;
	int selnum = 0;
	double cx, cy;
	//select_t *last;
	coord_t **coords;
	coord_t *coord;
	form_child_t *form = NULL;
	double minx, miny, maxx, maxy;
	double x, y;

	cx = 0;
	cy = 0;
	minx = miny = -1;
	maxx = maxy = -1;
	//last = pstate.select;
	coords = pshow->sets->currset->coords;
	while (last)
	{
		// get coordinates for selected dot
		index = last->index;
		if (!form && !last->form)
		{
			coord = coords[index];
			x = coord->x;
			y = coord->y;
		}
		else if (!form)
		{
			form = last->form;
			x = form->endpoints[0][0];
			y = form->endpoints[0][1];
		}
		else
		{
			x = form->endpoints[1][0];
			y = form->endpoints[1][1];
			form = NULL;
		}
		if (x > maxx)
			maxx = x;
		if (x < minx || minx == -1)
			minx = x;
		if (y > maxy)
			maxy = y;
		if (y < miny || miny == -1)
			miny = y;
		//cx = cx + coord->x;
		//cy = cy + coord->y;
		if (!form)
		{
			selnum++;
			last = last->next;
		}
	}
	if (!selnum)
	{
		cx = 0;
		cy = 0;
	}
	else
	{
		cx = (maxx - minx) / 2 + minx;
		cy = (maxy - miny) / 2 + miny;
	}
	// store
	pstate.center->x = cx;
	pstate.center->y = cy;
	pstate.selnum = selnum;

	return;
}

select_t *select_update_scope_set1_set2(select_t *select_head, set_t *currset, set_t *nextset)
{
	form_child_t *formnext, *form;
	form_child_t *formnext_head;
	select_t *select;

	select = select_head;

	formnext_head = nextset->forms;
	while (select)
	{
		if (!select->form)
		{
			select = select->next;
			continue;
		}
		form = select->form;
		formnext = formnext_head;
		while (formnext)
		{
			if (form == formnext)
			{
				break;
			}
			formnext = formnext->next;
		}
		if (formnext == NULL)
			select_head = form_flatten(form, select_head);
		select = select->next;
	}
	return select_head;
}




