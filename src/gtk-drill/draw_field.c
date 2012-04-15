// drawfield.c

//#include "drill.h"
#include "d_gtk.h"
#include <unistd.h>

double xstep, yheight;	
extern int set_tot;		// total amount of sets
extern int setnum;
extern int do_field;
extern int playing;
extern int set_step;
extern GTimer * timer;

void force_redraw(GtkWidget *widget)
{	// Refresh the field
	do_field=1;
	gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
}

int field_init(void)
{
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, pstate.width, pstate.height);
	field = cairo_create (surface);
	gaks = cairo_create (surface);
	fnums = cairo_create (surface);
	dots = gdk_cairo_create(drill->window);
	selected = gdk_cairo_create(drill->window);
	pstate.first_time = 0;
	return 0;
}

void def_canvas (GtkWidget *widget)
{
	// define the canvas
	
	double c_width;
	double c_height;
	//double z_x;
	//double z_y;
	// use default values for width and height
	if (pstate.width != widget->allocation.width || pstate.height != widget->allocation.height)
	{
		do_field=1;
	}
	// update width and height
	pstate.width = widget->allocation.width;	// Get the width
	pstate.height = widget->allocation.height;	// Get the height
	//z_x = 100 * width / (double)scrolled_window->allocation.width;
	//z_y = 100 * height / (double)scrolled_window->allocation.height;
	if (pstate.width != pstate.zoom_x || pstate.height != pstate.zoom_y)
	{
		// Catch zoom < 100% and handle
		// TODO: narrow scope of conditional to only catch
		// zoom < 100%
		// Warning: this could cause performance issues:
		// watch closely
		//printf("ping\n");
		//do_field = 1;
		if (pstate.zoom_x < pstate.width)
		{
			c_width = pstate.zoom_x;
		}
		else
			c_width = pstate.width;
		if (pstate.zoom_y < pstate.height)
		{
			c_height = pstate.zoom_y;
		}
		else
			c_height = pstate.height;
	}	
	else
	{
		c_width = pstate.width;
		c_height = pstate.height;
	}
	//c_width = 801;
	//c_height = 426;
	//printf("width=%g\theight=%g\tstep=%g\n", width, height, pstate.step);
	pstate.xoff = (int)c_width % 160; 		// extra margin for the width
	if (!pstate.xoff)	// need some margin
		pstate.xoff = (int)(c_width-1) % 160;
	pstate.xo2 = pstate.xoff / 2;			// half of the offset
	pstate.step = (c_width-pstate.xoff) / 160;	// length of one 8:5 step
	yheight = pstate.step * 85;		// height of the field
	//printf("yheight = %g height = %g\n", yheight, height);
	if (yheight > c_height)
	{	// limiting factor is height, adjust
		pstate.yoff = (int)c_height % 85;
		pstate.yo2 = pstate.yoff / 2;
		pstate.step = (c_height - pstate.yoff) / 85;
		yheight = pstate.step * 85;
		pstate.xoff = c_width - (160*pstate.step);
		pstate.xo2 = pstate.xoff / 2;
	}
	else
	{
		pstate.yoff = c_height - yheight;	// y offset
		pstate.yo2 = pstate.yoff / 2;			// half of the offset
	}
	// account for zooming
	pstate.xoff = pstate.xoff + (pstate.width - c_width);
	pstate.yoff = pstate.yoff + (pstate.height - c_height);
	pstate.xo2 = pstate.xoff / 2;
	pstate.yo2 = pstate.yoff / 2;
	
	return;
}

void drawing_method(cairo_t *cdots, double x, double y)
{
	// dot drawing
	double size = 0.5;
	double oset = size*pstate.step;
	cairo_move_to(cdots, x-oset, y-oset);
	cairo_rel_line_to(cdots, 2*oset, 2*oset);
	cairo_move_to(cdots, x-oset, y+oset);
	cairo_rel_line_to(cdots, 2*oset, -2*oset);
	//cairo_new_sub_path(cdots);
	//cairo_arc(cdots, x, y, 2*(double)pstate.step/3, 0, 360);
	return;
}


int field_to_pixel(double *x_r, double *y_r)
{
	// convert field coordinates to 
	// locations to draw on a screen
	double x, y;
	x = *x_r;
	y = *y_r;

	x = pstate.xo2 + pstate.step * x;
	y = pstate.yo2 + pstate.step * y;

	*x_r = x;
	*y_r = y;
	return 0;
}


int draw_selected(GtkWidget *widget)
{
	// draw selected dots, not normal dots
	struct select_proto *select;
	struct set_container *sets;
	struct set_proto *currset;
	int index;
	double x, y;

	cairo_destroy(selected);
	selected = gdk_cairo_create(widget->window);
	cairo_set_line_width(selected, 1.5);
	cairo_set_source_rgb(selected, 1, 0, 0);
	// get set information
	sets = pshow->sets;
	currset = sets->currset;
	/*
	prevset = sets->prevset;
	nextset = currset->next;
	// get coord information
	coords = currset->coords;
	if (nextset != NULL)
		ncoords = nextset->coords;
	else
		ncoords = 0;
	*/
	// get select information
	select = pshow->select;
	while (select != NULL)
	{
		index = select->index;
		retr_midset(currset, index, &x, &y);
		field_to_pixel(&x, &y);
		drawing_method(selected, x, y);
		select = select->next;
	}
	cairo_stroke(selected);
	cairo_fill (selected);
	do_selected = 0;
	return 0;
}


	

int draw_dots (GtkWidget *widget)
{
	int i;		// loop variable
	// set containers
	struct set_proto *currset;	
	struct set_proto *lastset;
	//struct set_proto *prevset;
	//struct set_proto *nextset;
	// performer container
	struct perf_proto **perf;
	// coordinate containers
	//struct coord_proto *coords;
	// coordinates
	double x, y;	// x and y location
	//double xcalc, ycalc;
	//double xprev, yprev;
	// canvases
	//cairo_t *dots;	// context for all dots
	//cairo_t *selected; // this will eventually have a struct to get dots
	//cairo_surface_t *field_surface;
	//cairo_surface_t *bak_surface;
	struct select_proto *selects;
	int was_selected;

	// Generate field
	//def_canvas(widget);

	if (do_dots)
	{
		// Draw the field
		cairo_destroy(surface_write);
		surface_write = gdk_cairo_create(widget->window);

		//cairo_set_source_surface(surface_write, field_surface, 1, 1);
		cairo_set_source_surface(surface_write, surface, 1, 1);
		cairo_paint (surface_write);


		// Define canvases
		cairo_destroy(dots);
		dots = gdk_cairo_create(widget->window);

		cairo_set_line_width(dots, 1.5);

		// grab sets from data structure
		currset = pshow->sets->currset;
		lastset = currset->next;
		//prevset = pshow->sets->prevset;
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
		// get previous set
		//prevset = pshow->sets->prevset;
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
				retr_midset(currset, i, &x, &y);
				field_to_pixel(&x, &y);
				// print selection if dot is selected
				was_selected = 0;
				if (selects)
				{
					// check to see if dot is selected
					if (selects->index == i)
					{
						// dot is selected
						//drawing_method(selected, x, y);
						// TODO: Should draw_selected
						// 	be broken up to be used indiv.
						// 	and called here?
						selects = selects->next;
						was_selected = 1;
					}
				}
				if (was_selected == 0)
				{
					drawing_method(dots, x, y);
				}
			}
		}
		if (lastset)
		{
			if (pshow->step >= lastset->counts)
			{
				// Go to next set
				set_next();
				lastset = pshow->sets->currset->next;
			}
		}
		if (pshow->sets->currset->next == NULL)
		{
			// At last set, playing is done
			pstate.playing = 0;
		}
		// Show all the dots
		cairo_stroke(dots);
		cairo_fill (dots);
		// Cleanup loose ends
		if (do_selected)
			draw_selected(widget);
		do_dots = 0;
	}
	//cairo_surface_destroy(field_surface);
	return 0;
}

void draw_field (GtkWidget *widget)
{	// This function will draw the actual football field
	//int tempo;
	int i, j;		// loop vars
	//double x, y;
	char text[3];		// text for yardline names
	int ynum = 0;		// yardline numbering
	int past_fifty = 0;	// check to see if past 50 yardline
	double x_bear;		// text centering
	//double y_bear;		// text centering
	cairo_text_extents_t te;
	//cairo_font_options_t *fopts;


	def_canvas(widget);	// Refresh dimensions and such
	// Set up to redraw the field
	//printf("do_field %i\n", do_field);
	do_dots = 1;
	do_selected = 1;
	if (do_field)
	{
		// draw dots also
		do_dots = 1;
		do_selected = 1;
		// (Re)allocate field
		if (!pstate.first_time)
		{
			cairo_destroy(fnums);
			cairo_destroy(gaks);
			cairo_destroy(field);
			cairo_surface_destroy(surface);
		}
		else
			pstate.first_time = 0;
		surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, pstate.width, pstate.height);
		field = cairo_create (surface);
		gaks = cairo_create (surface);
		fnums = cairo_create (surface);
		cairo_set_source_rgb(fnums, .7, .7, .7);
		cairo_set_font_size(fnums, 20);

		cairo_set_source_rgb(field, 1, 1, 1);	
		cairo_paint (field);

		for (i=pstate.xo2; i<=pstate.width-(int)pstate.xo2; i+=(pstate.width-(int)pstate.xoff)/20)
		{	// Yardlines
			cairo_set_line_width(field, 1);
			cairo_set_source_rgb(field, 0, 0, 0);
			cairo_move_to (field, i, pstate.height-yheight-pstate.yo2);
			cairo_line_to (field, i, yheight+pstate.yo2);
			// Yardline Numbers
			sprintf(text, "%i", ynum);
			cairo_text_extents (fnums, text, &te);
			//cairo_get_font_options(fnums, fopts);
			//cairo_font_options_set_height(fopts, 4*pstate.step);
			x_bear = te.x_bearing + te.width / 2;
			//y_bear = te.y_bearing + te.height / 2;
			cairo_move_to (fnums, i - x_bear, pstate.height-pstate.yo2-pstate.step*12);
			cairo_show_text(fnums, text);
			cairo_move_to (fnums, i - x_bear, pstate.height-pstate.yo2-pstate.step*73);
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
			cairo_move_to (field, i-2*pstate.step, pstate.height-pstate.yo2-pstate.step*32);
			cairo_line_to (field, i+2*pstate.step, pstate.height-pstate.yo2-pstate.step*32);
			// Back Hash
			cairo_move_to (field, i-2*pstate.step, pstate.height-pstate.yo2-pstate.step*53);
			cairo_line_to (field, i+2*pstate.step, pstate.height-pstate.yo2-pstate.step*53);
			cairo_stroke (field);

			
			// Split Yardlines
			if (i<((int)pstate.xo2+(int)pstate.step*160))
			{
				// Light Stroke
				// only draw if window is large enough
				cairo_set_line_width (gaks, 0.5);
				//cairo_set_source_rgb(gaks, 0.9, 0.9, 0.9);
				cairo_set_source_rgb(gaks, 0.9, 0.9, 1);
				if (pstate.width-pstate.xo2 > 800)
				{
					for (j=i; j<i+(int)pstate.step*4; j+=(int)pstate.step)
					{	// 1-step yardlines
						cairo_move_to (gaks, j, pstate.height-yheight-pstate.yo2);
						cairo_line_to (gaks, j, yheight+pstate.yo2);
						
						cairo_move_to (gaks, j+pstate.step*5, pstate.height-yheight-pstate.yo2);
						cairo_line_to (gaks, j+pstate.step*5, yheight+pstate.yo2);
					}
					for (j=pstate.yo2+yheight; j>=pstate.yo2; j-=(int)pstate.step)
					{	// 1-step y-grid
						if (((int)(j-pstate.yo2-pstate.step)%(int)(pstate.step*4)) == 0)
							continue;
						cairo_move_to (gaks, i, j);
						cairo_line_to (gaks, i+pstate.step*8, j);
					}
					// Light Stroke Draw
					cairo_stroke (gaks);
				}

				// Med Stroke
				// only draw if window is large enough
				if (pstate.width-pstate.xo2 > 600)
				{
					// 4-step X
					//cairo_set_source_rgb(gaks, 0.5, 0.5, 0.5);
					cairo_set_source_rgb(gaks, 0.5, 0.5, 0.9);
					cairo_move_to (gaks, i+(int)pstate.step*4, pstate.height-yheight-pstate.yo2);
					cairo_line_to (gaks, i+(int)pstate.step*4, yheight+pstate.yo2);
					
					for (j=pstate.yo2+yheight; j>=pstate.yo2; j-=4*(int)pstate.step)
					{	// 4-Step Gaks
						cairo_move_to (gaks, i, j);
						cairo_line_to (gaks, i+(int)pstate.step*8, j);
					}
					// Med Stroke Draw
					cairo_stroke(gaks);
				}
			}
		}
		// Maybe I can write this to a pixmap instead...
		//cairo_surface_write_to_png(surface, "field.png");
	}
	// Cleanup
	// Show the dots
	draw_dots(widget);
	
	// Make sure field won't be redrawn by default
	do_field = 0;
	update_entries();


}


