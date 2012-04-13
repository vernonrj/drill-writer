#include "d_gtk.h"
void view_stepsize_gtk(GtkWidget *widget)
{
	// view the stepsize of selected dots
	double stepsize;
	stepsize = check_stepsize_selected(pshow);
	printf("stepsize = %.2f:5\n", stepsize);
	return;
}

void goto_perf (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int perf_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer<pshow->perfnum)
			perf_cur=perf_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

int add_perf_gtk(GtkWidget *widget)
{
	int excode;
	excode = add_perf();
	gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	if (excode != -1)
		return 0;
	return excode;
}


void revert_perf_gtk(GtkWidget *widget)
{
	// revert selected performers
	if (!playing)
	{
		revert_perf_selected(pshow);
		gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	}
	return;
}

void delete_perf_gtk(GtkWidget *widget)
{
	// Delete selected performers
	if (!playing)
	{
		delete_perf_selected();
		gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	}
}



void xperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	double perf_buffer;
	struct coord_proto *coords = pshow->currset->coords[perf_cur];
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_x));
		perf_buffer = atof(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=160)
		{
			set_coord(pshow, perf_cur, perf_buffer, coords->y);
			//perf[setnum][perf_cur][0] = perf_buffer;
		}
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void yperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	double perf_buffer;
	struct coord_proto *coords = pshow->currset->coords[perf_cur];
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_y));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=85)
		{
			set_coord(pshow, perf_cur, coords->x, perf_buffer);
			//perf[setnum][perf_cur][1] = perf_buffer;
		}
		gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	}
}


void next_perf(GtkWidget *widget)
{
	// Go to next sequential dot
	if (perf_cur < pshow->perfnum-1)
	{
		perf_cur++;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void prev_perf(GtkWidget *widget)
{
	// Go to previous sequential dot
	if (perf_cur > 0)
	{
		perf_cur--;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

int select_all_gtk (GtkWidget *widget)
{
	select_all();
	gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	return 0;
}

gboolean clicked(GtkWidget *widget, GdkEventButton *event)
{
	// Handle click event on canvas
	
	// Length from click location to nearest dot


	if (event->button == 1)
	{
		switch(mouse_currentMode)
		{
			case SELECTONE:
				// select 1 performer
				select_oneperf_gtk(widget, event);
				gtk_widget_queue_draw_area(window, 0, 0, width, height);
				break;
			case SELECTDRAG:
				// Select (by dragging) performers
				gtk_widget_queue_draw_area(window, 0, 0, width, height);
				break;
			case ADDPERF:
				// Add performers by clicking on canvas
				gtk_widget_queue_draw_area(window, 0, 0, width, height);
				break;
			case MVPERF:
				// Move performers by clicking on canvas?
				gtk_widget_queue_draw_area(window, 0, 0, width, height);
				break;
		}
	}
	return TRUE;
}

int select_oneperf_gtk(GtkWidget *widget, GdkEventButton *event)
{
	guint state = event->state;
	double dist_threshold = 9;
	double distance;
	double workx, worky;
	double coordx, coordy;
	int perfnum;
	int i;
	int found_dot = 0;
	if (state == 0)
	{
		// normal click; discard other clicks
		select_discard();
	}
	coordx = event->x;
	coordy = event->y;
	//printf("x = %g, y = %g\n", coordx, coordy);
	// Adjust for various canvas offsets
	coordx = (coordx-xo2)/step;
	//coordy = (coordy-yo2-25)/step;
	coordy = (coordy-yo2)/step;

	//printf("button 1 pressed at %g %g %g\n", coordx, coordy, yo2);
	perfnum = pshow->perfnum;
	//for (i=0; i<perfnum; i++)
	perf_cur = 0;
	for (i=0; i<perfnum; i++)
	{
		retr_midset(pshow->currset, i, &workx, &worky);
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
		select_add(perf_cur);
	return 0;
}


