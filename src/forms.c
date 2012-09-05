#include "drillwriter.h"

bool form_checkEndpoints(form_t *form, double x, double y)
{
	//int i;
	//double distance;
	//double dist_threshold = 9;
	//double **endpoints;
	//double *coords;
	fline_t *line;
	if(!form)
		return NULL;
	switch(form->type)
	{
		case 1:		// line
			line = form->form->line;
			if (fieldrel_check_dots_within_range(line->endpoints[0][0], line->endpoints[0][1], x, y))
				return true;
			else if (fieldrel_check_dots_within_range(line->endpoints[1][0], line->endpoints[1][1], x, y))
				return true;
			break;
	}
	return false;
}



bool form_contains_coords(form_t *form, double x, double y)
{
	int i;
	fline_t *line;
	int index;
	double **coords;
	if (!form)
		return false;
	index = form->dot_num;
	switch(form->type)
	{
		case 1:		// line
			line = form->form->line;
			coords = line->coords;
			for (i=0; i<index; i++)
			{
				if (fieldrel_check_dots_within_range(coords[i][0], coords[i][1], x, y))
					return true;
			}
			break;
	}
	return false;
}






void form_build_line(group_t *group)
{
	// build a line
	form_t *form;
	fline_t *line;
	select_t *select;
	int i;
	int index = 0;
	double slopex, slopey;

	if (!group->forms)
		group->forms = (form_t*)malloc(sizeof(form_t));
	else
		return;
	form = group->forms;
	form->next = NULL;
	form->type = 1;
	form->form = (formType_t*)malloc(sizeof(formType_t));
	form->form->line = (fline_t*)malloc(sizeof(fline_t));
	line = form->form->line;
	for (i=0; i<2; i++)
		line->endpoints[i][0] = line->endpoints[i][1] = 0;
	select = group->selects;
	while (select)
	{
		index++;
		select = select->next;
	}
	form->dot_num = index;
	line->dots = (int*)malloc(index*sizeof(int));
	select = group->selects;
	coords_retrieve_midset(pshow->sets->currset, select->index, &line->endpoints[0][0], &line->endpoints[0][1]);
	line->coords = (double**)malloc(index*sizeof(double*));
	for (i=0; i<index; i++)
	{
		line->coords[i] = (double*)malloc(2*sizeof(double));
		coords_retrieve_midset(pshow->sets->currset, select->index, &line->coords[i][0], &line->coords[i][1]);
		line->dots[i] = select->index;
		if (i != 0 && i != index - 1)
			coords_set_managed_by_index(select->index, 0x1);
		else
			coords_set_managed_by_index(select->index, 0x2);
		if (i == index - 1)
			coords_retrieve_midset(pshow->sets->currset, select->index, &line->endpoints[1][0], &line->endpoints[1][1]);
		select = select->next;

	}
	slopey = (line->endpoints[1][1] - line->endpoints[0][1]) / (index - 1);
       	slopex = (line->endpoints[1][0] - line->endpoints[0][0]) / (index - 1);
	select = group->selects;
	for (i=0; i<index; i++)
	{
		line->coords[i][0] = i*slopex + line->endpoints[0][0];
		line->coords[i][1] = i*slopey + line->endpoints[0][1];
		coords_set_coord(pshow, select->index, line->coords[i][0], line->coords[i][1]);
		select = select->next;
	}
}



bool form_contained_in_rectangle(form_t *form, double x1, double y1, double x2, double y2)
{
	int i;
	int index;
	double **coords;
	fline_t *line;
	switch(form->type)
	{
		case 1:		// line
			line = form->form->line;
			index = form->dot_num;
			coords = line->coords;
			for (i=0; i<index; i++)
			{
				if (select_check_dot_in_rectangle(coords[i][0], coords[i][1],
							x1, y1, x2, y2))
					return true;
			}
			break;
	}
	return false;
}



group_t *form_find_group_with_index(group_t *group, int index)
{
	int i;
	int dot_num;
	fline_t *line;
	while (group)
	{
		if (!group->forms)
		{
			group = group->next;
			continue;
		}
		switch(group->forms->type)
		{
			case 1:		// line
				line = group->forms->form->line;
				dot_num = group->forms->dot_num;
				for (i=0; i<dot_num; i++)
				{
					if (line->dots[i] == index)
						return group;
				}
				break;
		}
		group = group->next;
	}
	return group;
}


