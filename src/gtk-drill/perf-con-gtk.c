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
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}
*/

int add_perf_gtk(GtkWidget *widget)
{
	int excode = 0;
	mouse_currentMode = ADDPERF;
	//gtk_button_set_label(GTK_BUTTON(menuButton), "Add");
	//excode = add_perf();
	//gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height+2*fldstate.canv_step);
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
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height+2*fldstate.canv_step);
	}
	return;
}

void delete_perf_gtk(GtkWidget *widget)
{
	// Delete selected performers
	if (!pstate.playing)
	{
		delete_perf_selected();
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height+2*fldstate.canv_step);
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
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
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
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height+2*fldstate.canv_step);
	}
}
*/


void next_perf(GtkWidget *widget)
{
	// Go to next sequential dot
	if (perf_cur < pshow->perfnum-1)
	{
		perf_cur++;
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}

void prev_perf(GtkWidget *widget)
{
	// Go to previous sequential dot
	if (perf_cur > 0)
	{
		perf_cur--;
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}

int select_all_gtk (GtkWidget *widget)
{
	select_all();
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height+2*fldstate.canv_step);
	return 0;
}


int add_group_gtk (GtkWidget *widget)
{
	// add selection to group
	int excode = add_group();
	return excode;
}


int select_mode_gtk (GtkWidget *widget)
{
	// go back to normal selectmode
	mouse_currentMode = SELECTONE;
	//gtk_button_set_label(GTK_BUTTON(menuButton), "Select");
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height+2*fldstate.canv_step);
	return 0;
}


gboolean unclicked(GtkWidget *widget, GdkEventButton *event)
{
	// handle un-click events on canvas
	double x, y;
	if (event->button == 1)
	{
		switch(mouse_currentMode)
		{
			case SELECTONE:
				// move a performer
				x = event->x;
				y = event->y;
				pixel_to_field(&x, &y);
				mousex = x - mousex;
				mousey = y - mousey;
				//printf("event->state == %i\n", event->state);
				if (event->state != 260)
				{
					if ((mousex != 0 || mousey != 0) && !mouse_discarded)
					{
						movexy(mousex, mousey);
						mouse_discarded = 0;
						gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
					}
				}
				break;
		}
	}
	return TRUE;
}


gboolean clicked(GtkWidget *widget, GdkEventButton *event)
{
	// Handle click event on canvas
	
	int index;
	double coordx, coordy;
	int excode;
	// Length from click location to nearest dot


	mouse_discarded = 0;
	if (event->button == 1)
	{
		mousex = event->x;
		mousey = event->y;
		pixel_to_field(&mousex, &mousey);
		switch(mouse_currentMode)
		{
			case SELECTONE:
				// select 1 performer
				index = checkSelected(widget, event);
				if (event->state == 4 && index != -1)
				{
					// ctrl-click
					pstate.select = select_add(pstate.select, index);
				}
				else if (event->state != 4)
				{
					// regular click
					if (!isSelected(index))
					{
						// dot is not selected
						select_discard();
						mouse_discarded = 1;
						if (index != -1)
						{
							pstate.select = select_add(pstate.select, index);
						}
					}
				}
				gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
				break;
			case SELECTDRAG:
				// Select (by dragging) performers
				gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
				break;
			case ADDPERF:
				// Add performers by clicking on canvas
				coordx = event->x;
				coordy = event->y;
				// Adjust for various canvas offsets
				coordx = round((coordx-fldstate.xo2)/fldstate.canv_step);
				//coordy = (coordy-yo2-25)/step;
				coordy = round((coordy-fldstate.yo2)/fldstate.canv_step);
				index = add_perf();
				set_coord(pshow, index, coordx, coordy);
				gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
				//mouse_currentMode = SELECTONE;
				break;
			case MVPERF:
				// Move performers by clicking on canvas?
				gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
				break;
		}
	}
	return TRUE;
}


int isSelected(int index)
{
	// check to see if a dot is in selected dots
	struct select_proto *select;
	int isin = 0;

	select = pstate.select;
	while (select != NULL)
	{
		if (index == select->index)
		{
			isin = 1;
			break;
		}
		select = select->next;
	}
	return isin;
}



int checkSelected(GtkWidget *widget, GdkEventButton *event)
{
	//guint state = event->state;
	double dist_threshold = 9;
	double distance;
	double workx, worky;
	double coordx, coordy;
	int perfnum;
	int i;
	int found_dot = 0;

	coordx = event->x;
	coordy = event->y;
	// Adjust for various canvas offsets
	coordx = (coordx-fldstate.xo2)/fldstate.canv_step;
	//coordy = (coordy-yo2-25)/step;
	coordy = (coordy-fldstate.yo2)/fldstate.canv_step;

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
			if (i == 0)
				printf("0 valid\n");
		}
		else if (pshow->perfs[i]->valid == 0 && i == 0)
			printf("invalid at %i\n", i);

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


