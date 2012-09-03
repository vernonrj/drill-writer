#include "d_gtk.h"
void view_stepsize_gtk(GtkWidget *widget)
{
	// view the stepsize of selected dots
	double stepsize;
	stepsize = check_stepsize_selected(pshow);
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
		revert_perf_selected(pshow);
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
		delete_perf_selected();
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
			set_coord(pshow, perf_cur, perf_buffer, coords->y);
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
			set_coord(pshow, perf_cur, coords->x, perf_buffer);
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
	wrap_select_all();
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


gboolean unclicked(GtkWidget *widget, GdkEventButton *event)
{
	// handle un-click events on canvas
	double x, y;
	double x1, y1;
	int index;
	if (event->button == 1)
	{
		switch(mouse_currentMode)
		{
			case SELECTONE:
				// move a performer
				x = event->x;
				y = event->y;
				pixel_to_field(&x, &y);
				mouse_clickx = x - mouse_clickx;
				mouse_clicky = y - mouse_clicky;
				//printf("event->state == %i\n", event->state);
				if (event->state == 256)
				{
					// regular click

					if ((mouse_clickx != 0 || mouse_clicky != 0) && !mouse_discarded)
					{
						movexy(mouse_clickx, mouse_clicky);
						mouse_discarded = 0;
						//dr_canvas_refresh(drill);
						dr_canvas_refresh(drill);
					}
				}
				else if (event->state == 256 + GDK_CONTROL_MASK)
				{
					// ctrl-click
					index = checkSelected(widget, event);
					if (index != -1)
						select_dots_add(index);
				}


				break;
		}
	}
	fldstate.mouse_clicked = 0;
	dr_canvas_refresh(drill);
	return TRUE;
}


gboolean clicked(GtkWidget *widget, GdkEventButton *event)
{
	// Handle click event on canvas
	
	int index;
	double coordx, coordy;
	group_t *group;
	// Length from click location to nearest dot


	mouse_discarded = 0;
	if (event->button == 8)
	{
		// back button
		if (event->state == GDK_SHIFT_MASK)
			set_prev_count();
		else if (event->state == GDK_CONTROL_MASK)
			set_first();
		else
			set_prev();
		fldstate.mouse_clicked = 0x0;
	}
	else if (event->button == 9)
	{
		// forward button
		if (event->state == GDK_SHIFT_MASK)
			set_next_count();
		else if (event->state == GDK_CONTROL_MASK)
			set_last();
		else
			set_next();
		fldstate.mouse_clicked = 0x0;
	}
	else if (event->button == 1)
	{
		fldstate.mouse_clicked = 0x1;
		mouse_clickx = event->x;
		mouse_clicky = event->y;
		pixel_to_field(&mouse_clickx, &mouse_clicky);
		fldstate.mouse_clickx = mouse_clickx;
		fldstate.mouse_clicky = mouse_clicky;
		switch(mouse_currentMode)
		{
			case SELECTONE:
				// select 1 performer
				/*
				group = dr_check_form_selected(widget, event);
				if (group)
				{
					pstate.select = select_add_group(pstate.select, group, false);
					index = -1;
				}
				else
					index = checkSelected(widget, event);
					*/
				group = dr_check_form_selected(widget, event);
				if (!group)
					index = checkSelected(widget, event);
				else
					index = -1;
				if (event->state == GDK_CONTROL_MASK)// && index != -1)
				{
					// ctrl-click
					//select_dots_add(index);
				}
				else if (event->state == GDK_SHIFT_MASK)// && index != -1)
				{
					// shift-click
					//if (pshow->topgroups && is_in_select(index, pshow->topgroups->selects))
						//pstate.select = select_add_group(pstate.select, pshow->topgroups, false);
				}
				else
				{
					// regular click
					if (!group && !isSelected(index))
					{
						// dot is not selected
						select_dots_discard();
						mouse_discarded = 1;
						if (index != -1)
						{
							select_dots_add(index);
						}
					}
					else if (group && !group_is_selected(group, pstate.select))
					{
						select_dots_discard();
						mouse_discarded = 1;
						pstate.select = select_add_group(pstate.select, group, false);
						if (isSelected(index))
							fldstate.mouse_clicked |= 0x2;
					}
					else
					{
						// hold click; dots being moved
						fldstate.mouse_clicked |= 0x2;
					}
				}
				//printf("event = %i\n", event->state);
				//dr_canvas_refresh(drill);
				dr_canvas_refresh(drill);
				break;
			case SELECTDRAG:
				// Select (by dragging) performers
				//dr_canvas_refresh(drill);
				dr_canvas_refresh(drill);
				break;
			case ADDPERF:
				// Add performers by clicking on canvas
				coordx = event->x;
				coordy = event->y;
				pixel_to_field(&coordx, &coordy);
				/*
				// Adjust for various canvas offsets
				coordx = round((coordx-fldstate.xo2)/fldstate.canv_step);
				//coordy = (coordy-yo2-25)/step;
				coordy = round((coordy-fldstate.yo2)/fldstate.canv_step);
				*/
				index = add_perf();
				set_coord(pshow, index, coordx, coordy);
				//dr_canvas_refresh(drill);
				dr_canvas_refresh(drill);
				//mouse_currentMode = SELECTONE;
				break;
			case MVPERF:
				// Move performers by clicking on canvas?
				//dr_canvas_refresh(drill);
				dr_canvas_refresh(drill);
				break;
		}
	}
	else
		fldstate.mouse_clicked = 0;
	return TRUE;
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
	double **endpoints;
	double dotx, doty;
	double distance;
	double dist_threshold = 9;
	select_t *select;


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
		retr_midset(pshow->sets->currset, i, &workx, &worky);
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


