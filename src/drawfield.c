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
	// define the canvas
	
	double c_width;
	double c_height;
	double z_x;
	double z_y;
	// use default values for width and height
	if (width != widget->allocation.width || height != widget->allocation.height)
	{
		do_field=1;
	}
	// update width and height
	width = widget->allocation.width;	// Get the width
	height = widget->allocation.height;	// Get the height
	z_x = 100 * width / (double)scrolled_window->allocation.width;
	z_y = 100 * height / (double)scrolled_window->allocation.height;
	//printf("width = %.2f\n", z_x);
	//printf("height = %.2f\n", z_y);
	if (width != zoom_x || height != zoom_y)
	{
		// Catch zoom < 100% and handle
		// TODO: narrow scope of conditional to only catch
		// zoom < 100%
		// Warning: this could cause performance issues:
		// watch closely
		//printf("ping\n");
		//do_field = 1;
		if (zoom_x < width)
		{
			c_width = zoom_x;
		}
		else
			c_width = width;
		if (zoom_y < height)
		{
			c_height = zoom_y;
		}
		else
			c_height = height;
	}	
	else
	{
		c_width = width;
		c_height = height;
	}
	//c_width = 801;
	//c_height = 426;
	//printf("width=%g\theight=%g\tstep=%g\n", width, height, step);
	xoff = (int)c_width % 160; 		// extra margin for the width
	if (!xoff)	// need some margin
		xoff = (int)(c_width-1) % 160;
	xo2 = xoff / 2;			// half of the offset
	step = (c_width-xoff) / 160;	// length of one 8:5 step
	yheight = step * 85;		// height of the field
	//printf("yheight = %g height = %g\n", yheight, height);
	if (yheight > c_height)
	{	// limiting factor is height, adjust
		yoff = (int)c_height % 85;
		yo2 = yoff / 2;
		step = (c_height - yoff) / 85;
		yheight = step * 85;
		xoff = c_width - (160*step);
		xo2 = xoff / 2;
	}
	else
	{
		yoff = c_height - yheight;	// y offset
		yo2 = yoff / 2;			// half of the offset
	}
	// account for zooming
	xoff = xoff + (width - c_width);
	yoff = yoff + (height - c_height);
	xo2 = xoff / 2;
	yo2 = yoff / 2;
	
	return;
}

void drawing_method(cairo_t *cdots, float x, float y)
{
	cairo_move_to(cdots, x-step, y-step);
	cairo_rel_line_to(cdots, 2*step, 2*step);
	cairo_move_to(cdots, x-step, y+step);
	cairo_rel_line_to(cdots, 2*step, -2*step);
	//cairo_new_sub_path(cdots);
	//cairo_arc(cdots, x, y, 2*(float)step/3, 0, 360);
	return;
}


int draw_dots (GtkWidget *widget)
{
	int i;		// loop variable
	// set containers
	struct set_proto *currset;	
	struct set_proto *lastset;
	// performer container
	struct perf_proto **perf;
	// coordinate containers
	struct coord_proto *coords;
	// coordinates
	float x, y;	// x and y location
	float xcalc, ycalc;
	// canvases
	//cairo_t *dots;	// context for all dots
	cairo_t *selected; // this will eventually have a struct to get dots
	cairo_surface_t *field_surface;
	cairo_surface_t *bak_surface;
	cairo_t *surface_write;
	struct select_proto *selects;
	int was_selected;

	// Generate field
	def_canvas(widget);

	/*
	bak_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
	surface_write = cairo_create(bak_surface);
	cairo_set_source_rgb(surface_write, 1, 1, 1);	
	cairo_paint (surface_write);
	cairo_destroy(surface_write);
	cairo_surface_destroy(bak_surface);
	*/

	//field_surface = cairo_image_surface_create_from_png("field.png");
	surface_write = gdk_cairo_create(widget->window);

	//cairo_set_source_surface(surface_write, field_surface, 1, 1);
	cairo_set_source_surface(surface_write, surface, 1, 1);
	cairo_paint (surface_write);
	cairo_destroy(surface_write);


	// Define canvases
	dots = gdk_cairo_create(widget->window);
	selected = gdk_cairo_create(widget->window);

	// grab sets from data structure
	currset = pshow->currset;
	perf = pshow->perfs;



	// Draw dots
	if (pshow)
	{
		// draw grayed out if stepped
		if (pshow->step)
		{
			cairo_set_source_rgb(dots, 0.3, 0.3, 0.3);
		}
		else
			cairo_set_source_rgb(dots, 0, 0, 0);
	}
	else
		cairo_set_source_rgb(dots, 0, 0, 0);
	cairo_set_source_rgb(selected, 1, 0, 0);
	//if (setnum+1<set_tot)
	if (currset->next != NULL)
	{	// Not the last set, can step to next set
		//g_print("Info from draw_dots function:\nCurrent Set: %i\tPerformers: %i\n", setnum, pshow->perfnum);
		// get next set
		lastset = currset->next;
		// get first selected dot
		selects = pshow->select;
		// draw performers at certain point
		for (i=0; i<pshow->perfnum; i++)
		{	// Draw performers only if they have valid dots
			if (!perf[i])
				return -1;
			if (perf[i]->valid)
			{
				// performer should be drawn
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
				// print selection if dot is selected
				was_selected = 0;
				if (selects)
				{
					// check to see if dot is selected
					if (selects->index == i)
					{
						// dot is selected
						/*
						cairo_new_sub_path(selected);
						cairo_arc(selected, x, y, 2*(float)step/3, 0, 360);
						*/
						drawing_method(selected, x, y);
						selects = selects->next;
						was_selected = 1;
					}
				}
				if (was_selected == 0)
				{
					drawing_method(dots, x, y);
					// dot is not selected
					//cairo_new_sub_path(dots);
					//cairo_arc(dots, x, y, 2*(float)step/3, 0, 360);
				}
				/*
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
				*/
			}
		}
		if (pshow->step >= lastset->counts)
		{
			// Go to next set
			pshow->step = 0;
			pshow->prevset = pshow->currset;
			pshow->currset = lastset;
			setnum++;
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
		cairo_stroke(dots);
		cairo_stroke(selected);
		cairo_fill (dots);
		cairo_fill (selected);
	}
	else
	{	// We're on the last set
		selects = pshow->select;
		for (i=0; i< pshow->perfnum; i++)
		{	// Draw dots here
			if (!perf[i])
				return -1;
			if (perf[i]->valid)
			{
				// draw only if valid
				retr_coord(currset->coords[i], &x, &y);
				x = xo2+step*x;
				y = yo2+step*y;
				// print selection if dot is selected
				was_selected = 0;
				if (selects)
				{
					// check to see if dot is selected
					if (selects->index == i)
					{
						// dot is selected
						/*
						cairo_new_sub_path(selected);
						cairo_arc(selected, x, y, 2*(float)step/3, 0, 360);
						*/
						drawing_method(selected, x, y);
						selects = selects->next;
						was_selected = 1;
					}
				}
				if (was_selected == 0)
				{
					// dot is not selected
					drawing_method(dots, x, y);
					/*
					cairo_move_to(dots, x-step, y-step);
					cairo_rel_line_to(dots, 2*step, 2*step);
					cairo_move_to(dots, x-step, y+step);
					cairo_rel_line_to(dots, 2*step, -2*step);
					//cairo_new_sub_path(dots);
					//cairo_arc(dots, x, y, 2*(float)step/3, 0, 360);
					*/
				}

				//cairo_rectangle(dots, x, y, step, step);
				//cairo_new_sub_path(dots);
				//cairo_arc(dots, x, y, 2*step/3, 0, 360);
			}
		}
		// Draw selected dots
		//retr_coord(currset->coords[perf_cur], &x, &y);
		//x = xo2+step*x;
		//y = yo2+step*y;
		//cairo_set_source_rgb(dots, 1, 0, 0);
		//cairo_rectangle(selected, x, y, step, step);
		//cairo_new_sub_path(selected);
		//cairo_arc(selected, x, y, 2*step/3, 0, 360);
		cairo_stroke(dots);
		cairo_stroke(selected);
		cairo_fill(dots);
		cairo_fill(selected);
	}
	// Cleanup loose ends
	cairo_destroy(dots);
	cairo_destroy(selected);
	cairo_surface_destroy(field_surface);
}

void draw_field (GtkWidget *widget)
{	// This function will draw the actual football field
	int tempo;
	int i, j, k;		// loop vars
	float x, y;
	char text[3];		// text for yardline names
	int ynum = 0;		// yardline numbering
	int past_fifty = 0;	// check to see if past 50 yardline
	float x_bear;		// text centering
	float y_bear;		// text centering
	cairo_text_extents_t te;
	cairo_font_options_t *fopts;


	def_canvas(widget);	// Refresh dimensions and such
	// Set up to redraw the field
	//printf("do_field %i\n", do_field);
	if (do_field)
	{
		//printf("Redrawing: (width, height) = (%.2f, %.2f)\n", width, height);
		//printf(">>>>>zoom: (width, height) = (%.2f, %.2f)\n\n", zoom_x, zoom_y);
		// Set background to White
		// (Re)allocate field
		if (!first_time)
		{
			cairo_destroy(fnums);
			cairo_destroy(gaks);
			cairo_destroy(field);
			cairo_surface_destroy(surface);
		}
		else
			first_time = 0;
		surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
		field = cairo_create (surface);
		gaks = cairo_create (surface);
		fnums = cairo_create (surface);
		cairo_set_source_rgb(fnums, .7, .7, .7);
		cairo_set_font_size(fnums, 20);

		cairo_set_source_rgb(field, 1, 1, 1);	
		cairo_paint (field);

		for (i=xo2; i<=width-(int)xo2; i+=(width-(int)xoff)/20)
		{	// Yardlines
			cairo_set_line_width(field, 1);
			cairo_set_source_rgb(field, 0, 0, 0);
			cairo_move_to (field, i, height-yheight-yo2);
			cairo_line_to (field, i, yheight+yo2);
			// Yardline Numbers
			sprintf(text, "%i", ynum);
			cairo_text_extents (fnums, text, &te);
			//cairo_get_font_options(fnums, fopts);
			//cairo_font_options_set_height(fopts, 4*step);
			x_bear = te.x_bearing + te.width / 2;
			y_bear = te.y_bearing + te.height / 2;
			cairo_move_to (fnums, i - x_bear, height-yo2-step*12);
			cairo_show_text(fnums, text);
			cairo_move_to (fnums, i - x_bear, height-yo2-step*73);
			cairo_show_text(fnums, text);
			if (ynum == 50)
				past_fifty = 1;
			if (past_fifty == 0)
			{
				// not past fifty yet; add 5
				ynum = ynum + 5;
			}
			else
			{
				// past fifty; sub 5
				ynum = ynum - 5;
			}
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
				// only draw if window is large enough
				cairo_set_line_width (gaks, 0.5);
				//cairo_set_source_rgb(gaks, 0.9, 0.9, 0.9);
				cairo_set_source_rgb(gaks, 0.9, 0.9, 1);
				if (width-xo2 > 800)
				{
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
				}

				// Med Stroke
				// only draw if window is large enough
				if (width-xo2 > 600)
				{
					// 4-step X
					//cairo_set_source_rgb(gaks, 0.5, 0.5, 0.5);
					cairo_set_source_rgb(gaks, 0.5, 0.5, 0.9);
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
		}
		// Maybe I can write this to a pixmap instead...
		cairo_surface_write_to_png(surface, "field.png");
	}
	// Cleanup
	// Show the dots
	draw_dots(widget);
	
	// Make sure field won't be redrawn by default
	do_field = 0;
	update_entries();


}


