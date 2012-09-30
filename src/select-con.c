#include "drillwriter.h"


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



bool select_check_dot_in_rectangle(double x, double y, double x1, double y1, double x2, double y2)
{
	//double xmin, xmax, ymin, ymax;
	bool chkx, chky;
	if (x1 < x2)
	{
		if (x < x2 && x1 <= x)
			chkx = true;
		else
			chkx = false;
	}
	else if (x2 < x1)
	{
		if (x <= x1 && x2 < x)
			chkx = true;
		else
			chkx = false;
	}
	else
		chkx = false;

	if (y1 < y2)
	{
		if (y < y2 && y1 <= y)
			chky = true;
		else
			chky = false;
	}
	else if (y2 < y1)
	{
		if (y <= y1 && y2 < y)
			chky = true;
		else
			chky = false;
	}
	else
		chky = false;
	return (chkx && chky);
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
	


select_t *select_add_form(select_t *selects, form_t *form, bool toggle)
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



select_t *select_add_in_rectangle(select_t *select, double x1, double y1, double x2, double y2, bool toggle)
{
	int i;
	int perfnum = pshow->perfnum;
	double x, y;

	/*
	while (group)
	{
		if (group->forms && form_contained_in_rectangle(group->forms, x1, y1, x2, y2))
		{
			select = select_add_group(select, group, true);
		}
		group = group->next;
	}
	*/
	for (i=0; i<perfnum; i++)
	{
		coords_retrieve_midset(pstate.setnum, i, &x, &y);
		if (select_check_dot_in_rectangle(x, y, x1, y1, x2, y2))
		{
			if (coords_check_managed_by_index(i) != 0x0)
			{
				select = select_add_form(select, 
						form_find_form_with_index(pshow->sets->currset->forms, i), 
						toggle);
			}
			else if (!select_check_index_selected(i, select) && pshow->perfs[i]->valid)
				select = select_add_index(select, i, true);
		}
	}
	return select;
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
	form_t *form = NULL;
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
	form_t *formnext, *form;
	form_t *formnext_head;
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

select_t *select_find_form_with_attr(select_t *select, double x, double y, bool (*fptr)(form_t*,double,double))
{
	// use comparison fptr to find a selected form with certain attributes
	form_t *form;
	while(select)
	{
		if (!select->form)
		{
			select = select->next;
			continue;
		}
		form = select->form;
		if (fptr(form, x, y))
			return select;
		select = select->next;
	}
	return NULL;
}


select_t *select_find_form_with_coords(select_t *select, double x, double y)
{
	// find and return a selected form
	// that contains a coordinate (x, y)
	return select_find_form_with_attr(select, x, y, &form_contains_coords);
}

select_t *select_find_form_with_hole(select_t *select, double x, double y)
{
	// find and return a selected form
	// that contains a hole (x, y)
	return select_find_form_with_attr(select, x, y, &form_hole_contains_coords);
}



select_t *select_find_form_with_endpoint(select_t *select, double x, double y)
{
	// find and return a selected form
	// that contains an endpoint (x, y)
	return select_find_form_with_attr(select, x, y, &form_endpoint_contains_coords);
}



select_t *select_find_form_with_endpoint_hole(select_t *select, double x, double y)
{
	// find and return a selected form
	// that contains an endpoint hole (x, y)
	return select_find_form_with_attr(select, x, y, &form_endpoint_hole_contains_coords); 
}


select_t *select_get_in_area(double x, double y)
{
	int i;
	int min_index = -1;
	int index;
	form_t *form = pshow->sets->currset->forms;
	form_t *min_form = NULL;
	int perfnum = pshow->perfnum;
	double coordx, coordy;
	double distance, distance_min = -1;
	coord_t *coord;
	select_t *dot_select = NULL;
	select_t *form_select = NULL;
	select_t *select = NULL;
	for(i=0; i<perfnum; i++)
	{
		if (!coords_check_managed_by_index(i))
		{
			coords_retrieve_midset(pstate.setnum, i, &coordx, &coordy);
			if (fieldrel_check_dots_within_range(x, y, coordx, coordy))
				dot_select = select_add_index(dot_select, i, false);
		}
	}
	while (form)
	{
		form = form_find_with_coords(form, x, y);
		form_select = select_add_form(form_select, form, false);
		if (form)
			form = form->next;
	}
	if (dot_select)
	{
		select = dot_select;
		distance_min = -1;
		while (select)
		{
			coords_retrieve_midset(pstate.setnum, select->index, &coordx, &coordy);
			distance = pow((coordx-x),2) + pow((coordy-y),2);
			if (distance < distance_min || distance_min == -1)
			{
				distance_min = distance;
				min_index = select->index;
			}
			select = select->next;
		}
		select = NULL;
		dot_select = select_discard(dot_select);
		dot_select = select_add_index(dot_select, min_index, false);
	}
	if (form_select)
	{
		select = form_select;
		distance_min = -1;
		while (select)
		{
			index = form_find_index_with_coords(select->form, x, y);
			if (index == -1)
			{
				coord = form_get_coord_near(select->form, x, y);
				coordx = coord->x;
				coordy = coord->y;
			}
			else
				coords_retrieve_midset(pstate.setnum, index, &coordx, &coordy);
			distance = pow((coordx-x),2) + pow((coordy-y),2);
			if (distance < distance_min || distance_min == -1)
			{
				distance_min = distance;
				min_form = select->form;
			}
			select = select->next;
		}
		form_select = select_discard(form_select);
		form_select = select_add_form(form_select, min_form, false);
	}

	if (!form_select)
		return dot_select;
	else if (!dot_select)
		return form_select;
	if (form_hole_contains_coords(form_select->form, x, y))
		return dot_select;
	else
		return form_select;
	return NULL;
}



