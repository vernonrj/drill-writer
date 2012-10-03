#include "drillwriter-gtk.h"
void view_stepsize_gtk(GtkWidget *widget)
{
	// view the stepsize of selected dots
	double stepsize;
	stepsize = perf_average_stepsize_selected(pshow);
	printf("stepsize = %.2f:5\n", stepsize);
	return;
}

/*
void goto_perf (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int perf_buffer;
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer<pshow->perfnum)
			perf_cur=perf_buffer;
		dr_canvas_refresh(drill);
	}
}
*/

int add_perf_gtk(GtkWidget *widget)
{
	int excode = 0;
	mouse_currentMode = ADDPERF;
	//gtk_button_set_label(GTK_BUTTON(menuButton), "Add");
	//excode = add_perf();
	//dr_canvas_refresh(drill);
	if (excode != -1)
		return 0;
	return excode;
}


void revert_perf_gtk(GtkWidget *widget)
{
	// revert selected performers
	if (!pstate.playing)
	{
		perf_revert_selected(pshow);
		//dr_canvas_refresh(drill);
		dr_canvas_refresh(drill);
	}
	return;
}

void delete_perf_gtk(GtkWidget *widget)
{
	// Delete selected performers
	if (!pstate.playing)
	{
		perf_delete_selected();
		//dr_canvas_refresh(drill);
		dr_canvas_refresh(drill);
	}
}



/*
void xperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	double perf_buffer;
	struct coord_proto *coords = pshow->sets->currset->coords[perf_cur];
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_x));
		perf_buffer = atof(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=160)
		{
			coords_set_coord(pshow, perf_cur, perf_buffer, coords->y);
			//perf[setnum][perf_cur][0] = perf_buffer;
		}
		dr_canvas_refresh(drill);
	}
}

void yperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	double perf_buffer;
	struct coord_proto *coords = pshow->sets->currset->coords[perf_cur];
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_y));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=85)
		{
			coords_set_coord(pshow, perf_cur, coords->x, perf_buffer);
			//perf[setnum][perf_cur][1] = perf_buffer;
		}
		dr_canvas_refresh(drill);
	}
}
*/


/*
void next_perf(GtkWidget *widget)
{
	// Go to next sequential dot
	if (perf_cur < pshow->perfnum-1)
	{
		perf_cur++;
		dr_canvas_refresh(drill);
		//dr_canvas_refresh(drill);
	}
}
*/

/*
void prev_perf(GtkWidget *widget)
{
	// Go to previous sequential dot
	if (perf_cur > 0)
	{
		perf_cur--;
		dr_canvas_refresh(drill);
		//dr_canvas_refresh(drill);
	}
}
*/

int select_all_gtk (GtkWidget *widget)
{
	select_all_dots();
	dr_canvas_refresh(drill);
	//dr_canvas_refresh(drill);
	return 0;
}


int select_form_gtk(GtkWidget *widget, form_child_t *form)
{
	//select_t *select = group->selects;
	pstate.select = select_add_form(pstate.select, form, false);
	/*
	while (select)
	{
		pstate.select = select_add(pstate.select, select->index, false);
		select = select->next;
	}
	*/
	dr_canvas_refresh(drill);
	return 0;
}


int select_group_gtk(GtkWidget *widget, group_t *group)
{
	pstate.select = select_add_group(pstate.select, group);
	dr_canvas_refresh(drill);
	return 0;
}


int add_group_gtk (GtkWidget *widget)
{
	// add selection to group
	//int excode = add_group();
	group_t *last = NULL;
	group_t *curr;
	last = group_add_selects(last, pstate.select);
	//pshow->topgroups = group_add_selects(pshow->topgroups, pstate.select);
	curr = pshow->topgroups;
	while (curr && curr->next)
		curr = curr->next;
	if (!curr)
		pshow->topgroups = last;
	else
		curr->next = last;
	dr_sidebar_update((DrSidebar*)sidebar);
	return 0;
}


int select_mode_gtk (GtkWidget *widget)
{
	// go back to normal selectmode
	mouse_currentMode = SELECTONE;
	//gtk_button_set_label(GTK_BUTTON(menuButton), "Select");
	//dr_canvas_refresh(drill);
	dr_canvas_refresh(drill);
	return 0;
}



int isSelected(int index)
{
	// check to see if a dot is in selected dots
	struct select_proto *select;
	//group_t *group = NULL;
	int isin = 0;
	int index_selected;
	select_t *form_select = NULL;

	select = pstate.select;
	while (select != NULL)
	{
		/*
		if (!form_select && select->form)
			form_select = form_get_contained_dots(select->form);
			*/
		if (!form_select && select_has_form(select))
			form_select = form_get_contained_dots(select_get_form(select));
		if (form_select)
		{
			//index_selected = form_select->index;
			index_selected = select_get_dot(form_select);
		}
		else
		{
			//index_selected = select->index;
			index_selected = select_get_dot(select);
		}

		if (index == -1)
		{
			isin = -1;
			break;
		}
		else if (index == index_selected)
		{
			isin = 1;
			break;
		}
		if (form_select)
		{
			//form_select = form_select->next;
			form_select = select_get_next(form_select);
		}
		if (!form_select)
		{
			//select = select->next;
			select = select_get_next(select);
		}
	}
	return isin;
}





form_child_t *dr_check_form_endpoints(GtkWidget *widget, GdkEventButton *event)
{
	double coordx, coordy;
	form_child_t *form = pshow->sets->currset->forms;
	coordx = event->x;
	coordy = event->y;
	pixel_to_field(&coordx, &coordy);
	form = check_endpoints(coordx, coordy, form);
	return form;
}


form_child_t *dr_check_form(GtkWidget *widget, GdkEventButton *event)
{
	double coordx, coordy;
	form_child_t *form = pshow->sets->currset->forms;
	coordx = event->x;
	coordy = event->y;
	pixel_to_field(&coordx, &coordy);
	while (form)
	{
		if (form_contains_coords(form, coordx, coordy))
			return form;
		form = form->next;
	}
	return NULL;
}



form_child_t *check_endpoints(double coordx, double coordy, form_child_t *form)
{

	while (form)
	{
		if (form_endpoint_contains_coords(form, coordx, coordy))
			return form;
		form = form->next;
	}
	return form;
}





