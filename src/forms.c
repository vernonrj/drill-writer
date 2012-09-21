#include "drillwriter.h"


form_coord_t *fcoord_construct(void)
{
	form_coord_t *fcoord;
	fcoord = (form_coord_t*)malloc(sizeof(form_coord_t));
	fcoord->dot = -1;
	fcoord->coord = coord_construct();
	fcoord->forms = (form_t**)malloc(sizeof(form_t*));
	fcoord->forms[0] = NULL;
	fcoord->dot_type = (int*)malloc(sizeof(int));
	fcoord->dot_type[0] = 1;
	fcoord->form_num = 1;
	fcoord->form_alloc = 1;
	return fcoord;
}


form_t *form_construct(void)
{
	return form_construct_with_size(3);
}



form_t *form_construct_with_size(int index)
{
	int i;
	form_t *form;

	form = (form_t*)malloc(sizeof(form_t));
	if (index < 3)
		index = 3;

	// initialize
	form->next = NULL;
	form->type = 0;
	form->name = (char*)malloc(sizeof(char));
	form->name[0] = '\0';
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





form_t *form_destruct(form_t *form)
{
	int i;
	int dot_num;
	form_t *last;
	//int *dots;
	bool is_selected = form_is_selected(form, pstate.select);
	form_coord_t **fcoords;
	if (is_selected)
		pstate.select = form_flatten(form, pstate.select);


	last = form->next;
	fcoords = form->fcoords;
	//dots = form->dots;
	if (form->name)
		free(form->name);
	dot_num = form->dot_num;
	for (i=0; i<dot_num; i++)
	{
		if (is_selected)
			select_add_index(pstate.select, fcoords[i]->dot, false);
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



form_t *form_add_hole_around_index(form_t *form, int index, bool after)
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




form_t *form_remove_index(form_t *form, int index)
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


form_t *form_remove_from(form_t *curr, form_t *form_head)
{
	// remove form from formlist
	form_t *last;
	if (!curr)
		return NULL;
	if (!form_head || curr == form_head)
	{
		//return (form_destruct(curr));
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
	last->next = curr->next;//form_destruct(curr);
	return form_head;
}


bool form_is_selected(form_t *form, select_t *select)
{
	while (select)
	{
		if (!select->form)
		{
			select = select->next;
			continue;
		}
		if (select->form == form)
			return true;
		select = select->next;
	}
	return false;
}



//bool form_checkEndpoints(form_t *form, double x, double y)
bool form_endpoint_contains_coords(form_t *form, double x, double y)
{
	if(!form)
		return NULL;
	if (fieldrel_check_dots_within_range(form->endpoints[0][0], form->endpoints[0][1], x, y))
		return true;
	else if (fieldrel_check_dots_within_range(form->endpoints[1][0], form->endpoints[1][1], x, y))
		return true;
	return false;
}



bool form_endpoint_hole_contains_coords(form_t *form, double x, double y)
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


bool form_contains_coords(form_t *form, double x, double y)
{
	int i;
	//fline_t *line;
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



bool form_hole_contains_coords(form_t *form, double x, double y)
{
	int i;
	double coordx, coordy;
	int dot_num;
	coord_t *coord;
	form_coord_t **fcoords;

	fcoords = form->fcoords;
	coordx = x - form->endpoints[0][0];
	coordy = y - form->endpoints[0][1];
	//dots = form->dots;
	dot_num = form->dot_num;
	//coords = form->coords;
	for(i=0; i<dot_num; i++)
	{
		coord = fcoords[i]->coord;
		if (fcoords[i]->dot == -1 && fieldrel_check_dots_within_range(coord->x, coord->y, coordx, coordy))
			return true;
	}
	return false;
}






int form_find_index_with_coords(form_t *form, double x, double y)
{
	// return an index that matches (x, y)
	int i;
	//int *dots;
	//double **coords;
	int dot_num;
	double coordx, coordy;
	form_coord_t **fcoords;
	coord_t *coord;
	int dot;
	if (!form)
		return -1;
	//dots = form->dots;
	dot_num = form->dot_num;
	fcoords = form->fcoords;
	//coords = form->coords;
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



form_t *form_find_form_with_index(form_t *form, int index)
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



form_t *form_find_with_coords(form_t *form, double x, double y)
{
	while (form)
	{
		if (form_contains_coords(form, x, y))
			return form;
		form = form->next;
	}
	return NULL;
}




form_t *form_find_with_hole(form_t *form, double x, double y)
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



form_t *form_find_with_endpoint(form_t *form, double x, double y)
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



form_t *form_find_with_endpoint_hole(form_t *form, double x, double y)
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





form_t *form_add_index_to_hole_with_coords(form_t *form, int index, double x, double y)
{
	int i;
	//int *dots;
	int dot_num;
	int dot;
	//double **coords;
	form_coord_t **fcoords;
	coord_t *coord;
	//int manage_state = coords_check_managed_by_index(index);
	double coordx, coordy;
	if (!form)
		return NULL;
	//dots = form->dots;
	//coords = form->coords;
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
					coords_retrieve_midset(pshow->sets->currset, index, &coordx, &coordy);
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





form_t *form_build_line(form_t *form, select_t *select_head)
{
	// build a line
	
	int i;
	int index = 0;
	select_t *select;
	form_coord_t **fcoords;

	// get number of selects
	// for allocation purposes
	select = select_head;
	while (select)
	{
		index++;
		select = select->next;
	}
	form = form_construct_with_size(index);
	fcoords = form->fcoords;
	form->type = 1;

	select = select_head;
	// get endpoints, continue allocation
	for (i=0; i<index; i++)
	{
		//form->coords[i][0] = form->coords[i][1] = 0;
		if (select)
		{
			//coords_retrieve_midset(pshow->sets->currset, select->index, &form->coords[i][0], &form->coords[i][1]);
			//form->dots[i] = select->index;
			fcoords[i]->dot = select->index;
			if (i != 0 && i != index - 1)
				coords_set_managed_by_index(select->index, 0x1);
			else
				coords_set_managed_by_index(select->index, 0x2);
			select = select->next;
		}
		else
		{
			fcoords[i]->dot = -1;
		}
	}
	form_update_line(form);
	return form;
}



bool form_contained_in_rectangle(form_t *form, double x1, double y1, double x2, double y2)
{
	int i;
	int index;
	//double **coords;
	index = form->dot_num;
	coord_t *coord;
	form_coord_t **fcoords;
	//coords = form->coords;
	fcoords = form->fcoords;
	for (i=0; i<index; i++)
	{
		coord = fcoords[i]->coord;
		if (select_check_dot_in_rectangle(coord->x, coord->y, x1, y1, x2, y2))
			return true;
	}
	return false;
}





int form_update_line(form_t *form)
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
				//form->coords[i][0] = i*slopex;// + form->endpoints[0][0];
				//form->coords[i][1] = i*slopey;// + form->endpoints[0][1];
				if (fcoord->dot != -1)
				{
					//coords_set_coord(pshow, form->dots[i], form->coords[i][0]+endpointx, form->coords[i][1]+endpointy);
					coords_set_coord(pshow, fcoord->dot, fcoord->coord->x+endpointx, fcoord->coord->y+endpointy);
				}
			}
			break;
	}
	return 0;
}


int form_set_endpoint(form_t *form, double x1, double y1, double x2, double y2)
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



int form_set_endpoint_grid(form_t *form, double x1, double y1, double x2, double y2)
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




int form_move_endpoint(form_t *form, double x1, double y1, double x2, double y2)
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


int form_move_endpoint_grid(form_t *form, double x1, double y1, double x2, double y2)
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


int form_movexy(form_t *form, double xoff, double yoff)
{
	int i, index;
	//int *dots;
	int dot;
	double endpointx, endpointy;
	form_coord_t *fcoord;
	if (!form)
		return -1;
	//dots = form->dots;
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


int form_unmanage_dot(form_t *form, int index)
{
	int i;
	//int *dot;
	int dot_num;
	form_coord_t **fcoords;

	if (!form)
		return -1;
	//dot = form->dots;
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



select_t *form_get_contained_dots(form_t *form)
{
	int i;
	int index;
	int dot;
	//int *dots;
	select_t *select = NULL;

	if (!form)
		return NULL;
	index = form->dot_num;
	//dots = form->dots;
	for(i=0; i<index; i++)
	{
		dot = form->fcoords[i]->dot;
		if (dot != -1)
			select = select_add_index(select, dot, false);
	}
	return select;
}


void form_add_to_set(form_t *form)
{
	form_t *setform = pshow->sets->currset->forms;
	form_t *curr = NULL;
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


coord_t **form_get_coords(form_t *form)
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



coord_t *form_get_coord_near(form_t *form, double x, double y)
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



void form_scale_from_center(form_t *form, double s_step)
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



select_t *form_flatten(form_t *form, select_t *select_head)
{
	int i;
	int dot_num;
	//int *dots;
	form_coord_t **fcoords;
	select_t *last;
	select_t *select = NULL;
	if (!form)
		return select;
	if (!select_head)
		return NULL;
	last = select_head;
	if (select_head->form == form)
		select_head = select_head->next;
	else
	{
		while (last && last->form != form)
		{
			select = last;
			last = last->next;
		}
		if (!last)
		{
			printf("WARNING: selection not in scope!\n");
			return NULL;
		}
		select->next = last->next;
	}
	fcoords = form->fcoords;
	dot_num = form->dot_num;
	//dots = form->dots;
	for (i=0; i<dot_num; i++)
		if (fcoords[i]->dot != -1)
			select_head = select_add_index(select_head, fcoords[i]->dot, false);
	free(last);
	return select_head;
}


form_t *form_copy(form_t *form)
{
	int i;
	int dot_num;
	int size;
	int index;
	int type;
	form_t *newform;
	dot_num = form->dot_num;
	newform = form_construct_with_size(dot_num);
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



form_container_t *form_container_construct(void)
{
	form_container_t *last;

	last = (form_container_t*)malloc(sizeof(form_container_t));
	last->size = 0;
	last->size_alloc = 0;
	return last;
}

form_container_t *form_container_construct_with_form(form_t *form, int index)
{
	int size_alloc;
	form_container_t *last;
	last = form_container_construct();
	size_alloc = last->size_alloc = 5;
	
	last->forms = (form_t**)malloc(size_alloc*sizeof(form_t*));
	last->set_index = (int*)malloc(size_alloc*sizeof(int));
	last->forms[0] = form;
	last->set_index[0] = index;
	last->size = 1;

	return last;
}
		

form_container_t *form_container_destroy(form_container_t *last)
{
	free(last->forms);
	free(last->set_index);
	LIST_REMOVE(last, formlist_entries);
	free(last);
	return NULL;
}


form_list_t *form_container_add_form(form_list_t *head, form_t *form, int index)
{
	form_container_t *last;

	if (!head)
	{
		head = (form_list_t*)malloc(sizeof(form_list_t));
		LIST_INIT(head);
	}
	
	last = form_container_construct_with_form(form, index);

	LIST_INSERT_HEAD(head, last, formlist_entries);

	return head;
}






int form_container_add_set(form_list_t *head, form_container_t *last, int index)
{
	int i;
	int pivot;
	int size;
	int size_alloc;
	int *set_index;
	form_t **forms;
	int excode;
	bool setnum_broke = false;
	if (!last)
		return -1;
	size = last->size;
	size_alloc = last->size_alloc;
	while (size >= size_alloc)
	{
		set_index = (int*)malloc((size_alloc+5)*sizeof(int));
		forms = (form_t**)malloc((size_alloc+5)*sizeof(form_t*));
		for (i=0; i<size; i++)
		{
			set_index[i] = last->set_index[i];
			forms[i] = last->forms[i];
		}
		size_alloc+=5;
		free(last->set_index);
		free(last->forms);
		last->set_index = set_index;
		last->forms = forms;
		last->size_alloc = size_alloc;
	}
	forms = last->forms;
	set_index = last->set_index;
	for(i=0; i<size; i++)
	{
		if (set_index[i] >= index)
		{
			setnum_broke = true;
			break;
		}
	}
	if (setnum_broke && set_index[i] == index)
		return 0;
	pivot = i;
	for(i = size; i >= pivot; i--)
	{
		set_index[i+1] = set_index[i];
		forms[i+1] = forms[i];
	}
	size++;
	last->size = size;
	set_index[pivot] = index;
	if (pivot > 0 && set_index[pivot] == set_index[pivot-1]+1)
	{
		// re-using form from previous set
		forms[pivot] = form_copy(forms[pivot-1]);
		excode = 0;
	}
	else
	{
		// making new form. Indicate in exit code
		forms[pivot] = form_construct();
		excode = 1;
	}
	form_add_to_set(forms[pivot]);
	return excode;
}


int form_container_remove_set(form_list_t *head, form_container_t *last, int index)
{
	int i;
	int size;
	bool removed_flag = false;
	form_t **forms;
	int *set_index;
	form_container_t *curr;

	if (!last)
		return -1;
	size = last->size;
	forms = last->forms;
	set_index = last->set_index;
	for(i=0; i<size; i++)
	{
		if (set_index[i] == index)
		{
			removed_flag = true;
			form_destruct(forms[i]);
		}
		if (removed_flag && i < (size-1))
		{
			set_index[i] = set_index[i+1];
			forms[i] = forms[i+1];
		}
	}
	last->size--;
	if (last->size <= 0)
	{
		last = form_container_destroy(last);
		return 1;
	}
	return 0;
}



form_container_t *form_container_find_with_form(form_list_t *head, form_t *form)
{
	int i;
	form_t **forms;
	int size;
	form_container_t *last;
	bool found_form = false;

	last = LIST_FIRST(head);
	while (last)
	{
		forms = last->forms;
		size = last->size;
		for (i=0; i<size; i++)
		{
			if (forms[i] == form)
			{
				found_form = true;
				break;
			}
		}
		if (found_form)
			break;
		last = LIST_NEXT(last, formlist_entries);
	}
	return last;
}


form_t *form_container_find_form_at_index(form_container_t *last, int index)
{
	int i;
	int size;
	int *set_index;

	set_index = last->set_index;
	size = last->size;
	for(i=0; i<size; i++)
		if (set_index[i] == index)
			return last->forms[i];
	return NULL;
}


bool form_container_mapped_at_set(form_container_t *last, int setnum)
{
	int i;
	int *set_index;
	if (!last)
		return false;
	set_index = last->set_index;
	for(i=0; i<last->size; i++)
		if (set_index[i] == setnum)
			return true;
	return false;
}
