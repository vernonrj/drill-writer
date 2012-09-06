#include "drillwriter.h"

bool form_checkEndpoints(form_t *form, double x, double y)
{
	//int i;
	//double distance;
	//double dist_threshold = 9;
	//double **endpoints;
	//double *coords;
	//fline_t *line;
	if(!form)
		return NULL;
	switch(form->type)
	{
		case 1:		// line
			//line = form->form->line;
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




/*
form_t *form_find_with_endpoint_from_group(group_t *group, double x, double y)
{
*/




group_t *form_build_line(group_t *group)
{
	// build a line
	
	form_t *form;
	select_t *select;
	int i;
	int index = 0;

	// check to see if starting from empty group
	if (!group)
		group = group_construct();
	// check to see if there are more forms
	if (!group->forms)
		group->forms = (form_t*)malloc(sizeof(form_t));
	else
		return group;

	// initialize
	form = group->forms;
	form->next = NULL;
	form->type = 1;
	for (i=0; i<2; i++)
		form->endpoints[i][0] = form->endpoints[i][1] = 0;

	// get number of selects
	// for allocation purposes
	select = group->selects;
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

	// get endpoints, continue allocation
	select = group->selects;
	if (select)
		coords_retrieve_midset(pshow->sets->currset, select->index, &form->endpoints[0][0], &form->endpoints[0][1]);
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
			if (i == index - 1)
				coords_retrieve_midset(pshow->sets->currset, select->index, &form->endpoints[1][0], &form->endpoints[1][1]);
			select = select->next;
		}
		else
		{
			form->dots[i] = -1;
			form->coords[i][0] = form->coords[i][1] = 0;
		}
	}
	form_update_line(form);
	return group;
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



group_t *form_find_group_with_index(group_t *group, int index)
{
	int i;
	int dot_num;
	form_t *form;
	while (group)
	{
		form = group->forms;
		if (!form)
		{
			group = group->next;
			continue;
		}
		dot_num = form->dot_num;
		switch(form->type)
		{
			case 1:		// line
				for (i=0; i<dot_num; i++)
				{
					if (form->dots[i] == index)
						return group;
				}
				break;
		}
		group = group->next;
	}
	return group;
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



int form_move_endpoint(group_t *group, double x1, double y1, double x2, double y2)
{
	while (group)
	{
		if (group->forms && form_checkEndpoints(group->forms, x1, y1))
		{
			form_set_endpoint(group->forms, x1, y1, x2, y2);
			return 0;
		}
		group = group->next;
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

