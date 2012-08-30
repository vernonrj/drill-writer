#include "drill.h"

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
	retr_midset(pshow->sets->currset, select->index, &line->coords[0][0], &line->coords[0][1]);
	for (i=0; i<index; i++)
	{
		line->dots[i] = select->index;
		if (i == index - 1)
			retr_midset(pshow->sets->currset, select->index, &line->coords[1][0], &line->coords[1][1]);
		select = select->next;

	}
}




