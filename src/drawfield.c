// drawfield.c

#include "drill.h"
#include <unistd.h>

double xstep, yheight;	
extern int set_tot;		// total amount of sets
extern int setnum;
extern int do_field;
extern int playing;
extern int set_step;
extern GTimer * timer;

void def_canvas (GtkWidget *widget)
{
	if (width != widget->allocation.width || height != widget->allocation.height)
		do_field=1;
	width = widget->allocation.width;	// Get the width
	height = widget->allocation.height;	// Get the height
	//printf("width=%g\theight=%g\tstep=%g\n", width, height, step);
	xoff = (int)width % 160; 		// extra margin for the width
	if (!xoff)	// need some margin
		xoff = (int)(width-1) %160;
	xo2 = xoff / 2;			// half of the offset
	step = (width-xoff) / 160;	// length of one 8:5 step
	yheight = step * 85;		// height of the field
	//printf("yheight = %g height = %g\n", yheight, height);
	if (yheight > height)
	{	// limiting factor is height, adjust
		yoff = (int)height % 85;
		yo2 = yoff / 2;
		step = (height - yoff) / 85;
		yheight = step * 85;
		xoff = width - (160*step);
		xo2 = xoff / 2;
	}
	else
	{
		yoff = height - yheight;	// y offset
		yo2 = yoff / 2;			// half of the offset
	}
}


void draw_dots (GtkWidget *widget)
{
	int i;		// loop variable
	// set containers
	struct set_proto *currset;	
	struct set_proto *lastset;
	// coordinate containers
	struct coord_proto *coords;
	// coordinates
	float x, y;	// x and y location
	float xcalc, ycalc;
	// canvases
	cairo_t *dots;	// context for all dots
	cairo_t *selected; // this will eventually have a struct to get dots
	cairo_surface_t *field_surface;
	cairo_t *surface_write;

	// Generate field
	field_surface = cairo_image_surface_create_from_png("field.png");
	surface_write = gdk_cairo_create(widget->window);

	cairo_set_source_surface(surface_write, field_surface, 1, 1);
	cairo_paint (surface_write);
	cairo_destroy(surface_write);
	def_canvas(widget);


	// Define canvases
	dots = gdk_cairo_create(widget->window);
	selected = gdk_cairo_create(widget->window);

	// grab sets from data structure
	currset = pshow->currset;

	// Draw dots
	cairo_set_source_rgb(dots, 0, 0, 0);
	cairo_set_source_rgb(selected, 1, 0, 0);
	//if (setnum+1<set_tot)
	if (currset->next != NULL)
	{	// Not the last set, can step to next set
		g_print("Info from draw_dots function:\nCurrent Set: %i\tPerformers: %i\n", setnum, perfnum);
		// get next set
		lastset = currset->next;
		// draw performers at certain point
		for (i=0; i<pshow->perfnum; i++)
		{	// Draw performers
			// Get dots for current set and next set
			retr_coord(currset->coords[i], &x, &y);
			retr_coord(lastset->coords[i], &xcalc, &ycalc);
			// Build horizontal location
			xcalc = (xcalc - x) / lastset->counts;
			xcalc = xcalc*pshow->step + x;
			x = xo2 + step*xcalc;
			//x=((perf[setnum+1][i][0]-perf[setnum][i][0])/counts[setnum+1]);
			//x=x*set_step+perf[setnum][i][0];
			//xcalc = x;
			//x=xo2+step*x;
			// y location (even if mid-set)
			ycalc = (ycalc - y) / lastset->counts;
			ycalc = ycalc*pshow->step + y;
			y = yo2 + step*ycalc;
			//y=((perf[setnum+1][i][1]-perf[setnum][i][1])/counts[setnum+1]);
			//y=y*set_step+perf[setnum][i][1];
			//ycalc = y;
			//y=yo2+step*y;
			//printf("Drawing Performer %i at:\t\tX = %g\t\tY = %g\n", i, xcalc, ycalc);
			if (i==perf_cur)
			{	// This is the highlighted performer
				//cairo_rectangle(selected, x, y, step, step);
				cairo_new_sub_path(selected);
				cairo_arc(selected, x, y, 2*(float)step/3, 0, 360);
			}
			
			else
			{
				//cairo_rectangle(dots, xo2-step/2+step*x, yo2-step/2+step*y, step, step);
				cairo_new_sub_path(dots);
				cairo_arc(dots, x, y, 2*(float)step/3, 0, 360);
			}

		}
		if (pshow->step >= lastset->counts)
		{
			pshow->step = 0;
			pshow->prevset = pshow->currset;
			pshow->currset = lastset;
		}
		/*
		if (set_step >= counts[setnum+1])
		{	// step to the next set
			set_step = 0;
			setnum++;
			//printf("setnum=%i\n", setnum);
		}
		*/
		if (pshow->currset->next == NULL)
		{
			// At last set, playing is done
			playing = 0;
		}
		/*
		if (setnum+1>=set_tot)
		{	// At last set, playingis done
			playing=0;
		}
		*/
		// Show all the dots
		cairo_fill (dots);
		cairo_fill (selected);
	}
	else
	{	// We're on the last set
		for (i=0; i< pshow->perfnum; i++)
		{	// Draw dots here
			retr_coord(currset->coords[i], &x, &y);
			x = xo2+step*x;
			y = yo2+step*y;
			//cairo_rectangle(dots, x, y, step, step);
			cairo_new_sub_path(dots);
			cairo_arc(dots, x, y, 2*step/3, 0, 360);
		}
		cairo_fill(dots);
		// Draw selected dots
		retr_coord(currset->coords[perf_cur], &x, &y);
		x = xo2+step*x;
		y = yo2+step*y;
		//cairo_set_source_rgb(dots, 1, 0, 0);
		//cairo_rectangle(selected, x, y, step, step);
		cairo_new_sub_path(selected);
		cairo_arc(selected, x, y, 2*step/3, 0, 360);
		cairo_fill(selected);
	}
	// Cleanup loose ends
	cairo_destroy(dots);
	cairo_destroy(selected);
	cairo_surface_destroy(field_surface);
}

void draw_field (GtkWidget *widget)
{	// This function will draw the actual football field
	int i, j, k;		// loop vars
	def_canvas(widget);	// Refresh dimensions and such
	cairo_surface_t *surface =
		cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
	cairo_t *field = cairo_create (surface);
	cairo_t *gaks = cairo_create (surface);

	
	// Set up to redraw the field
	//printf("do_field %i\n", do_field);
	if (do_field)
	{
		// Set background to White
		cairo_set_source_rgb(field, 1, 1, 1);	
		cairo_paint (field);

		for (i=xo2; i<=width-(int)xo2; i+=(width-(int)xoff)/20)
		{	// Yardlines
			cairo_set_line_width(field, 1);
			cairo_set_source_rgb(field, 0, 0, 0);
			cairo_move_to (field, i, height-yheight-yo2);
			cairo_line_to (field, i, yheight+yo2);
			// Front Hash
			cairo_move_to (field, i-2*step, height-yo2-step*32);
			cairo_line_to (field, i+2*step, height-yo2-step*32);
			// Back Hash
			cairo_move_to (field, i-2*step, height-yo2-step*53);
			cairo_line_to (field, i+2*step, height-yo2-step*53);
			cairo_stroke (field);

			
			// Split Yardlines
			if (i<((int)xo2+(int)step*160))
			{
				// Light Stroke
				cairo_set_line_width (gaks, 0.5);
				cairo_set_source_rgb(gaks, 0.9, 0.9, 0.9);
				for (j=i; j<i+(int)step*4; j+=(int)step)
				{	// 1-step yardlines
					cairo_move_to (gaks, j, height-yheight-yo2);
					cairo_line_to (gaks, j, yheight+yo2);
					
					cairo_move_to (gaks, j+step*5, height-yheight-yo2);
					cairo_line_to (gaks, j+step*5, yheight+yo2);
				}
				for (j=yo2+yheight; j>=yo2; j-=(int)step)
				{	// 1-step y-grid
					if (((int)(j-yo2-step)%(int)(step*4)) == 0)
						continue;
					cairo_move_to (gaks, i, j);
					cairo_line_to (gaks, i+step*8, j);
				}
				// Light Stroke Draw
				cairo_stroke (gaks);

				// Med Stroke

				// 4-step X
				cairo_set_source_rgb(gaks, 0.5, 0.5, 0.5);
				cairo_move_to (gaks, i+(int)step*4, height-yheight-yo2);
				cairo_line_to (gaks, i+(int)step*4, yheight+yo2);
				
				for (j=yo2+yheight; j>=yo2; j-=4*(int)step)
				{	// 4-Step Gaks
					cairo_move_to (gaks, i, j);
					cairo_line_to (gaks, i+(int)step*8, j);
				}
				// Med Stroke Draw
				cairo_stroke(gaks);
			}
		}
		// Maybe I can write this to a pixmap instead...
		cairo_surface_write_to_png(surface, "field.png");
	}
	// Cleanup
	cairo_destroy(gaks);
	cairo_destroy(field);
	// Show the dots
	draw_dots(widget);
	
	cairo_surface_destroy(surface);
	// Make sure field will be redrawn by default
	do_field=0;

	// Update all the entries
	// Convert all numbers into strings first
	sprintf(set_buf, "%i", setnum);
	sprintf(count_buf, "%i", counts[setnum]);
	sprintf(perf_buf, "%i", perf_cur);
	sprintf(perf_buf_x, "%g", perf[setnum][perf_cur][0]);
	sprintf(perf_buf_y, "%g", perf[setnum][perf_cur][1]);
	// Now Update entries with new data
	gtk_entry_set_text(GTK_ENTRY (entry_sets), set_buf);
	gtk_entry_set_text(GTK_ENTRY (entry_counts), count_buf);
	gtk_entry_set_text(GTK_ENTRY (entry_perf), perf_buf);
	gtk_entry_set_text(GTK_ENTRY (entry_perf_x), perf_buf_x);
	gtk_entry_set_text(GTK_ENTRY (entry_perf_y), perf_buf_y);

}


