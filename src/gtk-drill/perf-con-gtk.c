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

int select_all_gtk (GtkWidget *widget)
{
	select_all_dots();
	dr_canvas_refresh(drill);
	//dr_canvas_refresh(drill);
	return 0;
}


int select_group_gtk(GtkWidget *widget, group_t *group)
{
	//select_t *select = group->selects;
	pstate.select = select_add_group(pstate.select, group, false);
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
	select_t *group_select = NULL;
	//group_t *group = NULL;
	int isin = 0;
	int index_selected;

	select = pstate.select;
	while (select != NULL)
	{
		if (!group_select && select->group)
			group_select = select->group->selects;
		if (group_select)
			index_selected = group_select->index;
		else
			index_selected = select->index;

		//if (index == select->index)
		if (index == index_selected)
		{
			isin = 1;
			break;
		}
		if (group_select)
			group_select = group_select->next;
		if (!group_select)
			select = select->next;
	}
	return isin;
}



group_t *dr_check_form_selected(GtkWidget *widget, GdkEventButton *event)
{
	double coordx, coordy;
	group_t *group = pshow->sets->currset->groups;
	//group = checkGroups(widget, event, group);
	//if (group)
	//	return group;
	coordx = event->x;
	coordy = event->y;
	pixel_to_field(&coordx, &coordy);
	group = checkGroups(coordx, coordy, group);
	return group;
}



group_t *checkGroups(double coordx, double coordy, group_t *group)
{
	//double **endpoints;
	//double dotx, doty;
	//double distance;
	//double dist_threshold = 9;
	//select_t *select;


	while (group)
	{
		if (group->forms && form_checkEndpoints(group->forms, coordx, coordy))
			return group;
		group = group->next;
	}
	return group;
}



int checkSelected(GtkWidget *widget, GdkEventButton *event)
{
	// find a dot close to mouseclick
	double dist_threshold = 9;
	double distance;
	double workx, worky;
	double coordx, coordy;
	int perfnum;
	int i;
	int found_dot = 0;

	coordx = event->x;
	coordy = event->y;
	pixel_to_field(&coordx, &coordy);

	//printf("button 1 pressed at %g %g %g\n", coordx, coordy, yo2);
	perfnum = pshow->perfnum;
	//for (i=0; i<perfnum; i++)
	perf_cur = 0;
	for (i=0; i<perfnum; i++)
	{
		coords_retrieve_midset(pshow->sets->currset, i, &workx, &worky);
		workx = workx - coordx;
		worky = worky - coordy;
		distance = pow(workx, 2) + pow(worky, 2);
		/*
		if (i == 0)
		{
			printf("(x,y) at %.2f, %.2f (valid = %i)\n", workx, worky, pshow->perfs[i]->valid);
		}
		*/
		if (distance < dist_threshold && pshow->perfs[i]->valid)
		{
			// Found a closer dot
			perf_cur = i;
			dist_threshold = distance;
			found_dot = 1;
		}

	}
	if (found_dot == 1)
	{
		return perf_cur;
		/*
		i = inSelected(perf_cur);
		if (state != 4) 
		{
			if (!i)
				select_discard();
		}
		if (state != 0 || !i)
			select_add(perf_cur);
		*/
	}
	else
		return -1;
	return 0;
}


