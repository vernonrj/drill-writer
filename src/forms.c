#include "drillwriter.h"


form_coord_t *fcoord_construct(void)
{
	form_coord_t *fcoord;
	fcoord = (form_coord_t*)malloc(sizeof(form_coord_t));
	fcoord->dot = -1;
	fcoord->coord = coord_construct();
	fcoord->forms = (form_child_t**)malloc(sizeof(form_child_t*));
	fcoord->forms[0] = NULL;
	fcoord->dot_type = (int*)malloc(sizeof(int));
	fcoord->dot_type[0] = 1;
	fcoord->form_num = 1;
	fcoord->form_alloc = 1;
	return fcoord;
}


form_child_t *form_child_construct(form_parent_t *parent)
{
	return form_child_construct_with_size(parent, 3);
}



form_child_t *form_child_construct_with_size(form_parent_t *parent, int index)
{
	int i;
	form_child_t *form;

	form = (form_child_t*)malloc(sizeof(form_child_t));
	if (index < 3)
		index = 3;

	// initialize
	form->next = NULL;
	form->type = 0;
	form->name = (char*)malloc(sizeof(char));
	form->name[0] = '\0';
	form->parent = parent;
	for (i=0; i<2; i++)
		form->endpoints[i][0] = form->endpoints[i][1] = 0;
	form->dot_num = index;
	form->fcoords = (form_coord_t**)malloc(index*sizeof(form_coord_t*));
	for (i=0; i<index; i++)
	{
		form->fcoords[i] = fcoord_construct();
		form->fcoords[i]->forms[0] = form;
	}
	form->fcoords[0]->dot_type[0] = 2;
	if (index > 0)
		form->fcoords[i-1]->dot_type[0] = 2;
	return form;
}





form_child_t *form_child_destruct(form_child_t *form)
{
	int i;
	int dot_num;
	form_child_t *last;
	//int *dots;
	bool is_selected = form_is_selected(form, pstate.select);
	form_coord_t **fcoords;
	if (is_selected)
		pstate.select = form_flatten(form, pstate.select);


	last = form->next;
	fcoords = form->fcoords;
	if (form->name)
		free(form->name);
	dot_num = form->dot_num;
	for (i=0; i<dot_num; i++)
	{
		if (is_selected)
		{
			//select_add_index(pstate.select, fcoords[i]->dot, false);
			select_add_dot(pstate.select, fcoords[i]->dot);
		}
		coords_set_managed_by_index(fcoords[i]->dot, 0);
		free(form->fcoords[i]->forms);
		free(form->fcoords[i]->dot_type);
		free(form->fcoords[i]->coord);
		free(form->fcoords[i]);
	}
	free(form->fcoords);
	free(form);
	return last;
}



form_child_t *form_add_hole_around_index(form_child_t *form, int index, bool after)
{
	int i;
	int piv = 0;
	form_coord_t **fcoords;
	form_coord_t **fcoords_new;
	int dot_num;

	fcoords = form->fcoords;
	dot_num = form->dot_num;

	fcoords_new = (form_coord_t**)malloc((dot_num+1)*sizeof(form_coord_t*));
	for(i=0; i<dot_num; i++)
	{
		if (fcoords[i]->dot == index)
		{
			fcoords_new[i+after] = fcoord_construct();
			piv++;
			fcoords_new[i+piv-after] = fcoords[i];
		}
		else
			fcoords_new[i+piv] = fcoords[i];
	}
	form->dot_num++;

	return form;
}




form_child_t *form_remove_index(form_child_t *form, int index)
{
	int i;
	int ii = 0;
	int piv_index = -1;
	form_coord_t **fcoords;
	form_coord_t **fcoords_new;
	int dot_num;

	fcoords = form->fcoords;
	dot_num = form->dot_num;

	fcoords_new = (form_coord_t**)malloc((dot_num-1)*sizeof(form_coord_t*));
	for(i=0; i<dot_num; i++)
	{
		if (fcoords[i]->dot != index)
		{
			fcoords_new[ii] = fcoords[i];
			ii++;
		}
		else
			piv_index = i;
	}
	form->dot_num--;
	free(fcoords[piv_index]->forms);
	free(fcoords[piv_index]->dot_type);
	free(fcoords[piv_index]->coord);
	free(fcoords[piv_index]);
	free(fcoords);
	form->fcoords = fcoords_new;

	return form;
}


form_child_t *form_remove_from(form_child_t *curr, form_child_t *form_head)
{
	// remove form from formlist
	form_child_t *last;
	if (!curr)
		return NULL;
	if (!form_head || curr == form_head)
	{
		return curr->next;
	}
	last = form_head;
	while (last && last->next != curr)
		last = last->next;
	if (!last)
	{
		printf("WARNING: Form to be removed not in scope!\n");
		return form_head;
	}
	last->next = curr->next;
	return form_head;
}


bool form_is_selected(form_child_t *form, select_t *select)
{
	int index;
	select_head(select);
	//while (select)
	while ((index = select_get_form_advance(select)) != -1)
	{
		/*
		//if (!select->form)
		if (!select_has_form(select))
		{
			//select = select->next;
			select = select_get_next(select);
			continue;
		}
		*/
		//if (select->form == form)
		if (form_container_get_form_child(pshow->topforms, select_get_form(select)) == form)
		{
			select_head(select);
			return true;
		}
		//select = select->next;
		//select = select_get_next(select);
	}
	select_head(select);
	return false;
}



bool form_endpoint_contains_coords(form_child_t *form, double x, double y)
{
	if(!form)
		return NULL;
	if (fieldrel_check_dots_within_range(form->endpoints[0][0], form->endpoints[0][1], x, y))
		return true;
	else if (fieldrel_check_dots_within_range(form->endpoints[1][0], form->endpoints[1][1], x, y))
		return true;
	return false;
}



bool form_endpoint_hole_contains_coords(form_child_t *form, double x, double y)
{
	double coordx, coordy;
	form_coord_t **fcoords;
	int dot_num;
	if (!form)
		return NULL;
	dot_num = form->dot_num;
	fcoords = form->fcoords;
	coordx = form->endpoints[0][0];
	coordy = form->endpoints[0][1];
	if (fcoords[0]->dot == -1 && fieldrel_check_dots_within_range(coordx, coordy, x, y))
		return true;
	coordx = form->endpoints[dot_num-1][0];
	coordy = form->endpoints[dot_num-1][1];
	if (fcoords[dot_num-1]->dot == -1 && fieldrel_check_dots_within_range(coordx, coordy, x, y))
		return true;
	return false;
}


bool form_contains_coords(form_child_t *form, double x, double y)
{
	int i;
	int index;
	form_coord_t **fcoords;
	coord_t *coord;
	if (!form)
		return false;
	index = form->dot_num;
	fcoords = form->fcoords;
	x -= form->endpoints[0][0];
	y -= form->endpoints[0][1];
	for (i=0; i<index; i++)
	{
		coord = fcoords[i]->coord;
		if (fieldrel_check_dots_within_range(coord->x, coord->y, x, y))
			return true;
	}
	return false;
}



bool form_hole_contains_coords(form_child_t *form, double x, double y)
{
	int i;
	double coordx, coordy;
	int dot_num;
	coord_t *coord;
	form_coord_t **fcoords;

	fcoords = form->fcoords;
	coordx = x - form->endpoints[0][0];
	coordy = y - form->endpoints[0][1];
	dot_num = form->dot_num;
	for(i=0; i<dot_num; i++)
	{
		coord = fcoords[i]->coord;
		if (fcoords[i]->dot == -1 && fieldrel_check_dots_within_range(coord->x, coord->y, coordx, coordy))
			return true;
	}
	return false;
}






int form_find_index_with_coords(form_child_t *form, double x, double y)
{
	// return an index that matches (x, y)
	int i;
	int dot_num;
	double coordx, coordy;
	form_coord_t **fcoords;
	coord_t *coord;
	int dot;
	if (!form)
		return -1;
	dot_num = form->dot_num;
	fcoords = form->fcoords;
	x -= form->endpoints[0][0];
	y -= form->endpoints[0][1];
	for(i=0; i<dot_num; i++)
	{
		dot = fcoords[i]->dot;
		coord = fcoords[i]->coord;
		coordx = coord->x;
		coordy = coord->y;
		if (fieldrel_check_dots_within_range(coordx, coordy, x, y))
			return dot;
	}
	return -1;
}



form_child_t *form_find_form_with_index(form_child_t *form, int index)
{
	int i;
	int dot_num;
	form_coord_t **fcoords;
	while (form)
	{
		fcoords = form->fcoords;
		dot_num = form->dot_num;
		for (i=0; i<dot_num; i++)
		{
			if (fcoords[i]->dot == index)
				return form;
		}
		form = form->next;
	}
	return form;
}



form_child_t *form_find_with_coords(form_child_t *form, double x, double y)
{
	while (form)
	{
		if (form_contains_coords(form, x, y))
			return form;
		form = form->next;
	}
	return NULL;
}




form_child_t *form_find_with_hole(form_child_t *form, double x, double y)
{
	// find a form with a hole at coords
	while (form)
	{
		if (form_hole_contains_coords(form, x, y))
			return form;
		form = form->next;
	}
	return NULL;
}



form_child_t *form_find_with_endpoint(form_child_t *form, double x, double y)
{
	// find a form with an endpoint that matches coords
	while (form)
	{
		if (form_endpoint_contains_coords(form, x, y))
			return form;
		form = form->next;
	}
	return NULL;
}



form_child_t *form_find_with_endpoint_hole(form_child_t *form, double x, double y)
{
	// find a form with an endpoint hole that matches the coords
	while (form)
	{
		if (form_endpoint_hole_contains_coords(form, x, y))
			return form;
		form = form->next;
	}
	return NULL;
}


form_child_t *form_find_with_attr(select_t *select, double x, double y, bool (*fptr)(form_child_t*,double,double))
{
	// use comparison fptr to find a selected form with certain attributes
	int index;
	form_child_t *form;
	//while(select)
	select_head(select);
	while((index = select_get_form_advance(select)) != -1)
	{
		/*
		//if (!select->form)
		if (!select_get_form(select))
		{
			//select = select->next;
			select = select_get_next(select);
			continue;
		}
		*/
		//form = select->form;
		//form = select_get_form(select);
		form = form_container_get_form_child(pshow->topforms, index);
		if (fptr(form, x, y))
			return form;
		//select = select_get_next(select);
	}
	return NULL;
}


form_child_t *form_find_selected_with_endpoint(select_t *select, double x, double y)
{
	// find and return a selected form
	// that contains an endpoint (x, y)
	return form_find_with_attr(select, x, y, &form_endpoint_contains_coords);
}


form_child_t *form_find_selected_with_endpoint_hole(select_t *select, double x, double y)
{
	// find and return a selected form
	// that contains an endpoint hole (x, y)
	return form_find_with_attr(select, x, y, &form_endpoint_hole_contains_coords); 
}




form_child_t *form_add_index_to_hole_with_coords(form_child_t *form, int index, double x, double y)
{
	int i;
	int dot_num;
	int dot;
	form_coord_t **fcoords;
	coord_t *coord;
	double coordx, coordy;
	if (!form)
		return NULL;
	dot_num = form->dot_num;
	fcoords = form->fcoords;
	x -= form->endpoints[0][0];
	y -= form->endpoints[0][1];
	for(i=0; i<dot_num; i++)
	{
		dot = fcoords[i]->dot;
		coord = fcoords[i]->coord;
		if (dot == -1 && fieldrel_check_dots_within_range(coord->x, coord->y, x, y))
		{
			fcoords[i]->dot = index;
			if (i == 0 || i == dot_num - 1)
			{
				if (coords_check_managed_by_index(index) == 2)
				{
					// pairing endpoints together
					coords_retrieve_midset(pstate.setnum, index, &coordx, &coordy);
					form_set_endpoint(form, form->endpoints[0][0], form->endpoints[0][1], coordx, coordy);
				}
				else
					coords_set_managed_by_index(index, 2);
			}
			else
				coords_set_managed_by_index(index, 1);
			form_update_line(form);
			return form;
		}
	}
	return NULL;
}





form_child_t *form_build_line(form_parent_t *form_parent, select_t *select)
{
	// build a line
	
	int i;
	int index = 0;
	int size = 0;
	form_coord_t **fcoords;
	form_child_t *form;

	// get number of selects
	// for allocation purposes
	select_head(select);
	//while (select)
	while ((index = select_get_dot_advance(select)) != -1)
	{
		size++;
		//select = select->next;
		//select = select_get_next(select);
	}
	form = form_child_construct_with_size(size);
	fcoords = form->fcoords;
	form->type = 1;

	select = select_head;
	// get endpoints, continue allocation
	for (i=0; i<size; i++)
	{
		if (select)
		{
			//fcoords[i]->dot = select->index;
			fcoords[i]->dot = select_get_dot(select);
			if (i != 0 && i != size - 1)
			{
				//coords_set_managed_by_index(select->index, 0x1);
				coords_set_managed_by_index(select_get_dot(select), 0x1);
			}
			else
			{
				//coords_set_managed_by_index(select->index, 0x2);
				coords_set_managed_by_index(select_get_dot(select), 0x2);
			}
			//select = select->next;
			select = select_get_next(select);
		}
		else
		{
			fcoords[i]->dot = -1;
		}
	}
	form_update_line(form);
	return form;
}



bool form_contained_in_rectangle(form_child_t *form, double x1, double y1, double x2, double y2)
{
	int i;
	int index;
	index = form->dot_num;
	coord_t *coord;
	form_coord_t **fcoords;
	fcoords = form->fcoords;
	for (i=0; i<index; i++)
	{
		coord = fcoords[i]->coord;
		if (field_check_dot_in_rectangle(coord->x, coord->y, x1, y1, x2, y2))
			return true;
	}
	return false;
}





int form_update_line(form_child_t *form)
{
	int i;
	int index;
	double slopex, slopey;
	double endpointx, endpointy;
	form_coord_t *fcoord;
	switch(form->type)
	{
		case 1:		// line
			index = form->dot_num;
			slopex = (form->endpoints[1][0] - form->endpoints[0][0]) / (index - 1);
			slopey = (form->endpoints[1][1] - form->endpoints[0][1]) / (index - 1);
			endpointx = form->endpoints[0][0];
			endpointy = form->endpoints[0][1];

			for(i=0; i<index; i++)
			{
				fcoord = form->fcoords[i];
				fcoord->coord->x = i*slopex;
				fcoord->coord->y = i*slopey;
				if (fcoord->dot != -1)
				{
					coords_set_coord(pshow, fcoord->dot, fcoord->coord->x+endpointx, fcoord->coord->y+endpointy);
				}
			}
			break;
	}
	return 0;
}


int form_set_endpoint(form_child_t *form, double x1, double y1, double x2, double y2)
{
	int i;
	double *endpoint;
	if (!form)
		return -1;
	for (i=0; i<2; i++)
	{
		endpoint = form->endpoints[i];
		if (fieldrel_check_dots_within_range(endpoint[0], endpoint[1], x1, y1))
		{
			endpoint[0] = x2;
			endpoint[1] = y2;
			break;
		}
	}
	form_update_line(form);
	return 0;
}



int form_set_endpoint_grid(form_child_t *form, double x1, double y1, double x2, double y2)
{
	int i;
	double *endpoint;
	if (!form)
		return -1;
	for (i=0; i<2; i++)
	{
		endpoint = form->endpoints[i];
		if (fieldrel_check_dots_within_range(endpoint[0], endpoint[1], x1, y1))
		{
			endpoint[0] = round(x2);
			endpoint[1] = round(y2);
			break;
		}
	}
	form_update_line(form);
	return 0;
}




int form_move_endpoint(form_child_t *form, double x1, double y1, double x2, double y2)
{
	while (form)
	{
		if (form_endpoint_contains_coords(form, x1, y1))
		{
			form_set_endpoint(form, x1, y1, x2, y2);
			return 0;
		}
		form = form->next;
	}
	return -1;
}


int form_move_endpoint_grid(form_child_t *form, double x1, double y1, double x2, double y2)
{
	while (form)
	{
		if (form_endpoint_contains_coords(form, x1, y1))
		{
			form_set_endpoint_grid(form, x1, y1, x2, y2);
			return 0;
		}
		form = form->next;
	}
	return -1;
}


int form_movexy(form_child_t *form, double xoff, double yoff)
{
	int i, index;
	int dot;
	double endpointx, endpointy;
	form_coord_t *fcoord;
	if (!form)
		return -1;
	form->endpoints[0][0] += xoff;
	form->endpoints[1][0] += xoff;
	form->endpoints[0][1] += yoff;
	form->endpoints[1][1] += yoff;
	endpointx = form->endpoints[0][0];
	endpointy = form->endpoints[0][1];
	index = form->dot_num;
	for(i=0; i<index; i++)
	{
		fcoord = form->fcoords[i];
		dot = fcoord->dot;
		if (dot != -1)
			coords_set_coord(pshow, dot, fcoord->coord->x+endpointx, fcoord->coord->y+endpointy);
	}
	return 0;
}


int form_unmanage_dot(form_child_t *form, int index)
{
	int i;
	int dot_num;
	form_coord_t **fcoords;

	if (!form)
		return -1;
	dot_num = form->dot_num;
	fcoords = form->fcoords;
	for(i=0; i<dot_num; i++)
	{
		if (fcoords[i]->dot == index)
		{
			// found dot
			fcoords[i]->dot = -1;
			coords_set_managed_by_index(index, 0);
			break;
		}
	}
	return 0;
}



select_t *form_get_contained_dots(form_child_t *form)
{
	int i;
	int index;
	int dot;
	select_t *select = NULL;

	if (!form)
		return NULL;
	index = form->dot_num;
	for(i=0; i<index; i++)
	{
		dot = form->fcoords[i]->dot;
		if (dot != -1)
			select = select_add_index(select, dot, false);
	}
	return select;
}


void form_add_to_set(form_child_t *form)
{
	form_child_t *setform = pshow->sets->currset->forms;
	form_child_t *curr = NULL;
	if (!setform)
	{
		pshow->sets->currset->forms = form;
		return;
	}
	while (setform)
	{
		if (setform == form)
			return;
		curr = setform;
		setform = setform->next;
	}
	curr->next = form;
	return;
}


coord_t **form_get_coords(form_child_t *form)
{
	int i;
	int dot_num;
	coord_t **coords;
	if (!form)
		return NULL;
	dot_num = form->dot_num;
	coords = coords_construct(dot_num);
	if (!coords)
		return NULL;
	for(i=0; i<dot_num; i++)
	{
		coords[i]->x = form->fcoords[i]->coord->x;
		coords[i]->y = form->fcoords[i]->coord->y;
		if (form->fcoords[i]->dot == -1)
			coords[i]->type = 0;
		else
			coords[i]->type = 1;
	}
	return coords;
}	



coord_t *form_get_coord_near(form_child_t *form, double x, double y)
{
	int i;
	int min_index = -1;
	double min_distance = -1;
	double distance;
	int dot_num;
	coord_t *coord;
	form_coord_t **fcoords;
	if (!form)
		return NULL;
	dot_num = form->dot_num;
	coord = coord_construct();
	fcoords = form->fcoords;
	if (!coord)
		return NULL;
	x -= form->endpoints[0][0];
	y -= form->endpoints[0][1];
	for(i=0; i<dot_num; i++)
	{
		coord = fcoords[i]->coord;
		if (fieldrel_check_dots_within_range(coord->x, coord->y, x, y))
		{
			distance = pow(coord->x,2) + pow(coord->y,2);
			if (distance < min_distance || min_distance == -1)
			{
				min_index = i;
				min_distance = distance;
			}
		}
	}
	if (min_index != -1)
	{
		coord = coord_construct_with_data(fcoords[min_index]->coord->x+form->endpoints[0][0],
				fcoords[min_index]->coord->y+form->endpoints[0][1]);
		return coord;
	}
	return NULL;
}



void form_scale_from_center(form_child_t *form, double s_step)
{
	int i;
	double cx, cy;
	if (!form)
		return;
	cx = pstate.center->x;
	cy = pstate.center->y;
	for(i=0; i<2; i++)
		coords_scale_coords_from_center(s_step, &form->endpoints[i][0], &form->endpoints[i][1], cx, cy);
	form_update_line(form);
	return;
}


void form_rotate_around_center(form_child_t *form, double s_step)
{
	int i;
	double cx, cy;
	double distx, disty;
	double angle, hypo;
	if (!form)
		return;
	cx = pstate.center->x;
	cy = pstate.center->y;
	for(i=0; i<2; i++)
	{
		distx = cx - form->endpoints[i][0];
		disty = cy - form->endpoints[i][1];
		if (distx != 0 || disty != 0)
		{
			angle = atan(disty / distx);
			hypo = pow(distx, 2) + pow(disty, 2);
			hypo = sqrt(hypo);
			if (distx < 0)
				angle = angle + M_PI;
			angle = angle + s_step;
			distx = hypo*cos(angle);
			disty = hypo*sin(angle);
			form->endpoints[i][0] = cx - distx;
			form->endpoints[i][1] = cy - disty;
		}
	}
	form_update_line(form);
	return;
}



select_t *form_flatten(form_child_t *form, select_t *select_head)
{
	int i;
	int dot_num;
	form_coord_t **fcoords;
	select_t *last;
	select_t *select = NULL;
	if (!form)
		return select;
	if (!select_head)
		return NULL;
	last = select_head;
	/*
	if (select_head->form == form)
		select_head = select_head->next;
		*/
	if (select_get_form(select_head) == form)
		select_head = select_get_next(select_head);
	else
	{
		//while (last && last->form != form)
		while (last && select_get_form(last) != form)
		{
			select = last;
			//last = last->next;
			last = select_get_next(last);
		}
		if (!last)
		{
			printf("WARNING: selection not in scope!\n");
			return NULL;
		}
		//select->next = last->next;
		select_set_next(select, select_get_next(last));
	}
	fcoords = form->fcoords;
	dot_num = form->dot_num;
	for (i=0; i<dot_num; i++)
		if (fcoords[i]->dot != -1)
			select_head = select_add_index(select_head, fcoords[i]->dot, false);
	free(last);
	return select_head;
}


form_child_t *form_copy(form_child_t *form)
{
	int i;
	int dot_num;
	int size;
	int index;
	int type;
	form_child_t *newform;
	dot_num = form->dot_num;
	newform = form_child_construct_with_size(dot_num);
	newform->type = form->type;
	size = strlen(form->name)+1;
	newform->name = (char*)malloc(size*sizeof(char));
	strncpy(newform->name, form->name, size);
	for(i=0; i<dot_num; i++)
	{
		index = form->fcoords[i]->dot;
		newform->fcoords[i]->dot = index;
		if (index != -1)
		{
			type = (0 || (dot_num -1)) + 1;
			coords_set_managed_by_index(index, type);
		}
		memcpy(newform->fcoords[i]->coord, form->fcoords[i]->coord, sizeof(coord_t));
	}
	for(i=0; i<2; i++)
	{
		newform->endpoints[i][0] = form->endpoints[i][0];
		newform->endpoints[i][1] = form->endpoints[i][1];
	}
	form_update_line(newform);
	return newform;
}



form_parent_t *form_parent_construct(void)
{
	int i;
	form_parent_t *last;

	last = (form_parent_t*)malloc(sizeof(form_parent_t));
	last->forms = (form_child_t**)malloc(pshow->sets->size_alloc*sizeof(form_child_t*));
	for (i=0; i<pshow->sets->size_alloc; i++)
		last->forms[i] = NULL;
	return last;
}



form_parent_t *form_parent_construct_with_form(form_child_t *form, int index)
{
	form_parent_t *last;
	last = form_parent_construct();
	
	last->forms[0] = form;

	return last;
}



form_parent_t *form_parent_destruct(form_parent_t *last)
{
	free(last->forms);
	//LIST_REMOVE(last, formlist_entries);
	free(last);
	return NULL;
}



bool form_parent_contiguous(form_parent_t *fparent, int set_num)
{
	int size;
	size = pshow->sets->size;
	if (set_num < size-1 && fparent->forms[set_num] && fparent->forms[set_num+1])
		return true;
	return false;
}



form_container_t *form_parent_add_form(form_container_t *head, form_child_t *form, int index)
{
	form_parent_t *last;

	if (!head)
	{
		head = form_container_construct();
		//head = (form_list_t*)malloc(sizeof(form_list_t));
		//LIST_INIT(head);
	}
	
	last = form_parent_construct_with_form(form, index);
	form_container_insert_head(head, last);

	//LIST_INSERT_HEAD(head, last, formlist_entries);

	return head;
}



int form_parent_add_set(form_container_t *head, form_parent_t *last, int index)
{
	form_child_t **forms;
	int excode;
	if (!last)
		return -1;
	forms = last->forms;
	if (index > 0)
		forms[index] = form_copy(forms[index-1]);
	else
		forms[index] = form_child_construct(last);
	return 0;
	return excode;
}



int form_parent_remove_set(form_container_t *head, form_parent_t *last, int index)
{
	form_child_t **forms;

	if (!last)
		return -1;
	forms = last->forms;
	if (forms[index])
	{
		form_child_destruct(forms[index]);
		forms[index] = NULL;
	}
	return 0;
}



form_parent_t *form_parent_find_with_form(form_container_t *head, form_child_t *form)
{
	int i, j;
	form_child_t **forms;
	int size;
	form_parent_t *last;
	bool found_form = false;

	//last = LIST_FIRST(head);
	last = head->forms[0];
	for(i=0; i<head->size; i++)
	{
		last = head->forms[i];
		forms = last->forms;
		size = pshow->sets->size;
		for (j=0; j<size; j++)
		{
			if (forms[j] == form)
			{
				found_form = true;
				break;
			}
		}
		if (found_form)
			break;
		//last = LIST_NEXT(last, formlist_entries);
	}
	return last;
}



form_child_t *form_parent_find_form_at_index(form_parent_t *last, int index)
{
	return last->forms[index];
}



bool form_parent_mapped_at_set(form_parent_t *last, int setnum)
{
	return (last->forms[setnum] != NULL);
}


form_container_t *form_container_construct(void)
{
	form_container_t *fcont;

	fcont = (form_container_t*)malloc(sizeof(form_container_t));
	fcont = form_container_realloc(fcont, 5);
	return fcont;
}


form_container_t *form_container_destruct(form_container_t *fcont)
{
	free(fcont->forms);
	free(fcont);
	return NULL;
}


form_container_t *form_container_realloc(form_container_t *fcont, size_t size_alloc)
{
	int i;
	form_parent_t **forms;
	size_t size;

	forms = (form_parent_t**)malloc(size_alloc*sizeof(form_parent_t*));
	size = fcont->size;
	for (i = 0; i < size; i++)
	{
		forms[i] = fcont->forms[i];
		if (forms[i])
			forms[i]->index = i;
	}
	free(fcont->forms);
	fcont->forms = forms;
	fcont->size_alloc = size_alloc;
	return fcont;
}


form_container_t *form_container_insert_head(form_container_t *fcont, form_parent_t *last)
{
	int i;
	int size;
	form_parent_t **forms;

	forms = fcont->forms;
	size = fcont->size;

	if (size+1 >= fcont->size_alloc)
		fcont = form_container_realloc(fcont, fcont->size_alloc+5);
	for (i=size-1; i>=0; i--)
	{
		forms[i+1] = forms[i];
		if (forms[i+1])
			forms[i+1]->index = i+1;
	}
	forms[0] = last;
	fcont->size++;
	return fcont;
}

form_parent_t *form_container_get_form_parent(form_container_t *fcont, int index)
{
	return fcont->forms[index];
}

form_child_t *form_container_get_form_child_at_set(form_container_t *fcont, int index, int setnum)
{
	return form_container_get_form_parent(fcont, index)->forms[setnum];
}

form_child_t *form_container_get_form_child(form_container_t *fcont, int index)
{
	return form_container_get_form_parent(fcont, index)->forms[pstate.setnum];
}
