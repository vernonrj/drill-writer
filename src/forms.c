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
			if (fieldrel_check_dots_within_range(line->coords[0][0], line->coords[0][1], x, y))
				return true;
			else if (fieldrel_check_dots_within_range(line->coords[1][0], line->coords[1][1], x, y))
				return true;
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
		line->coords[i][0] = line->coords[i][1] = 0;
	select = group->selects;
	while (select)
	{
		index++;
		select = select->next;
	}
	line->dots = (int*)malloc(index*sizeof(int));
	select = group->selects;
	coords_retrieve_midset(pshow->sets->currset, select->index, &line->coords[0][0], &line->coords[0][1]);
	for (i=0; i<index; i++)
	{
		line->dots[i] = select->index;
		if (i == index - 1)
			coords_retrieve_midset(pshow->sets->currset, select->index, &line->coords[1][0], &line->coords[1][1]);
		select = select->next;

	}
}




