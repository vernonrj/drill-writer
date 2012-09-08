#include "drillwriter.h"

form_t *form_construct()
{
	return NULL;
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



bool form_checkEndpoints(form_t *form, double x, double y)
{
	if(!form)
		return NULL;
	switch(form->type)
	{
		case 1:		// line
			if (fieldrel_check_dots_within_range(form->endpoints[0][0], form->endpoints[0][1], x, y))
				return true;
			else if (fieldrel_check_dots_within_range(form->endpoints[1][0], form->endpoints[1][1], x, y))
				return true;
			break;
	}
	return false;
}



bool form_contains_coords(form_t *form, double x, double y)
{
	int i;
	//fline_t *line;
	int index;
	double **coords;
	if (!form)
		return false;
	index = form->dot_num;
	switch(form->type)
	{
		case 1:		// line
			//line = form->form->line;
			coords = form->coords;
			x -= form->endpoints[0][0];
			y -= form->endpoints[0][1];
			for (i=0; i<index; i++)
			{
				if (fieldrel_check_dots_within_range(coords[i][0], coords[i][1], x, y))
					return true;
			}
			break;
	}
	return false;
}



form_t *form_find_with_endpoint(form_t *form, double x, double y)
{
	while (form)
	{
		if (form_checkEndpoints(form, x, y))
			return form;
		form = form->next;
	}
	return NULL;
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


int form_find_index_with_coords(form_t *form, double x, double y)
{
	// return an index that matches (x, y)
	int i;
	int *dots;
	double **coords;
	int dot_num;
	double coordx, coordy;
	if (!form)
		return -1;
	dots = form->dots;
	dot_num = form->dot_num;
	coords = form->coords;
	x -= form->endpoints[0][0];
	y -= form->endpoints[0][1];
	for(i=0; i<dot_num; i++)
	{
		coordx = coords[i][0];
		coordy = coords[i][1];
		if (fieldrel_check_dots_within_range(coordx, coordy, x, y))
			return dots[i];
	}
	return -1;
}





form_t *form_build_line(form_t *form, select_t *select_head)
{
	// build a line
	
	int i;
	int index = 0;
	select_t *select;

	form = (form_t*)malloc(sizeof(form_t));

	// initialize
	form->next = NULL;
	form->type = 1;
	for (i=0; i<2; i++)
		form->endpoints[i][0] = form->endpoints[i][1] = 0;

	// get number of selects
	// for allocation purposes
	select = select_head;
	while (select)
	{
		index++;
		select = select->next;
	}
	form->dot_num = index;
	if (index < 3)
	{
		// too few selected dots
		// allow endpoints and midpoint
		index = 3;
		form->dot_num = 3;
	}
	// allocate dot list
	form->dots = (int*)malloc(index*sizeof(int));

	select = select_head;
	// get endpoints, continue allocation
	/*
	if (select)
		coords_retrieve_midset(pshow->sets->currset, select->index, &form->endpoints[0][0], &form->endpoints[0][1]);
		*/
	form->coords = (double**)malloc(index*sizeof(double*));
	for (i=0; i<index; i++)
	{
		form->coords[i] = (double*)malloc(2*sizeof(double));
		if (select)
		{
			coords_retrieve_midset(pshow->sets->currset, select->index, &form->coords[i][0], &form->coords[i][1]);
			form->dots[i] = select->index;
			if (i != 0 && i != index - 1)
				coords_set_managed_by_index(select->index, 0x1);
			else
				coords_set_managed_by_index(select->index, 0x2);
			/*
			if (i == index - 1)
				coords_retrieve_midset(pshow->sets->currset, select->index, &form->endpoints[1][0], &form->endpoints[1][1]);
				*/
			select = select->next;
		}
		else
		{
			form->dots[i] = -1;
			form->coords[i][0] = form->coords[i][1] = 0;
		}
	}
	form_update_line(form);
	return form;
}



bool form_contained_in_rectangle(form_t *form, double x1, double y1, double x2, double y2)
{
	int i;
	int index;
	double **coords;
	index = form->dot_num;
	coords = form->coords;
	for (i=0; i<index; i++)
	{
		if (select_check_dot_in_rectangle(coords[i][0], coords[i][1], x1, y1, x2, y2))
			return true;
	}
	return false;
}



form_t *form_find_form_with_index(form_t *form, int index)
{
	int i;
	int dot_num;
	while (form)
	{
		dot_num = form->dot_num;
		switch(form->type)
		{
			case 1:		// line
				for (i=0; i<dot_num; i++)
				{
					if (form->dots[i] == index)
						return form;
				}
				break;
		}
		form = form->next;
	}
	return form;
}


int form_update_line(form_t *form)
{
	int i;
	int index;
	double slopex, slopey;
	double endpointx, endpointy;
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
				form->coords[i][0] = i*slopex;// + form->endpoints[0][0];
				form->coords[i][1] = i*slopey;// + form->endpoints[0][1];
				if (form->dots[i] != -1)
					coords_set_coord(pshow, form->dots[i], form->coords[i][0]+endpointx, form->coords[i][1]+endpointy);
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



int form_move_endpoint(form_t *form, double x1, double y1, double x2, double y2)
{
	while (form)
	{
		if (form_checkEndpoints(form, x1, y1))
		{
			form_set_endpoint(form, x1, y1, x2, y2);
			return 0;
		}
		form = form->next;
	}
	return -1;
}


int form_movexy(form_t *form, double xoff, double yoff)
{
	int i, index;
	int *dots;
	double endpointx, endpointy;
	if (!form)
		return -1;
	dots = form->dots;
	form->endpoints[0][0] += xoff;
	form->endpoints[1][0] += xoff;
	form->endpoints[0][1] += yoff;
	form->endpoints[1][1] += yoff;
	endpointx = form->endpoints[0][0];
	endpointy = form->endpoints[0][1];
	index = form->dot_num;
	for(i=0; i<index; i++)
		if (dots[i] != -1)
			coords_set_coord(pshow, form->dots[i], form->coords[i][0]+endpointx, form->coords[i][1]+endpointy);
	return 0;
}


int form_unmanage_dot(form_t *form, int index)
{
	int i;
	int *dot;
	int dot_num;

	if (!form)
		return -1;
	dot = form->dots;
	dot_num = form->dot_num;
	for(i=0; i<dot_num; i++)
	{
		if (dot[i] == index)
		{
			// found dot
			dot[i] = -1;
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
	int *dots;
	select_t *select = NULL;

	if (!form)
		return NULL;
	index = form->dot_num;
	dots = form->dots;
	for(i=0; i<index; i++)
	{
		if (dots[i] != -1)
			select = select_add_index(select, dots[i], false);
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
		coords[i]->x = form->coords[i][0];
		coords[i]->y = form->coords[i][1];
		if (form->dots[i] == -1)
			coords[i]->type = 0;
		else
			coords[i]->type = 1;
	}
	return coords;
}	
