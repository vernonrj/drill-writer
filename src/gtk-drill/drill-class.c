// drill.c

//#include "drill.h"
#include "d_gtk.h"
#include "dr-drill.h"

static void gtk_drill_class_init(GtkDrillClass *klass);
static void gtk_drill_init(GtkDrill *drill);
static void gtk_drill_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_drill_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_drill_realize(GtkWidget *widget);
static gboolean gtk_drill_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_drill_paint(GtkWidget *widget);
static void gtk_drill_destroy(GtkObject *object);



void zoom_amnt(double invalue, bool from_mouse)
{
	// for now, just zoom relative
	double value;
	double offsetx1, offsety1;
	double offsetx2, offsety2;
	double page_sizex, page_sizey;
	double mousex, mousey;
	// block scroll signals for scrollbars
	// while adjusting canvas scaling, scroll amount
	g_signal_handler_block(hscroll, hscroll_id);
	g_signal_handler_block(vscroll, vscroll_id);
	if (!invalue || (fldstate.zoom_amnt * invalue) < 1)
	{
		// set zoom to 100%
		//value = fldstate.zoom_amnt;
		fldstate.zoom_amnt = 1;
		hscroll->page_size = fldstate.width;
		vscroll->page_size = fldstate.height;
		fldstate.fieldx = 0;
		fldstate.fieldy = 0;
		gtk_adjustment_configure(hscroll, 0.0, 0.0, fldstate.width, hscroll->page_size / 10, hscroll->page_size / 10 * 9, hscroll->page_size);
		gtk_adjustment_configure(vscroll, 0.0, 0.0, fldstate.height, vscroll->page_size / 10, vscroll->page_size / 10 * 9, vscroll->page_size);
		canvas_move(drill, 0, 0);
		g_signal_handler_unblock(hscroll, hscroll_id);
		g_signal_handler_unblock(vscroll, vscroll_id);
		return;
	}
	else
	{
		// zoom and scale the canvas
		value = invalue;
		// get the old page size
		offsetx1 = hscroll->upper / fldstate.zoom_amnt;
		offsety1 = vscroll->upper / fldstate.zoom_amnt;
		// new zoom factor
		fldstate.zoom_amnt *= value;
		// new page size
		page_sizex = hscroll->upper / fldstate.zoom_amnt;
		page_sizey = vscroll->upper / fldstate.zoom_amnt;
		// change scrollbars
		gtk_adjustment_configure(hscroll, fldstate.fieldx, 0.0, fldstate.width, page_sizex / 10, page_sizex / 10 * 9, page_sizex);
		gtk_adjustment_configure(vscroll, fldstate.fieldy, 0.0, fldstate.height, page_sizey / 10, page_sizey / 10 * 9, page_sizey);
		// calculate base offset after zoom
		offsetx2 = (offsetx1 - page_sizex) / 2;
		offsety2 = (offsety1 - page_sizey) / 2;
		if (from_mouse)
		{
			// get mouse position
			// use it in canvas offset
			mousex = fldstate.mousex;
			mousey = fldstate.mousey;
			// translate mouse position to pixel values
			field_to_pixel(&mousex, &mousey);
			// calculate a mouse offset factor
			mousex = 2*(mousex - fldstate.fieldx) / offsetx1;
			mousey = 2*(mousey - fldstate.fieldy) / offsety1;
		}
		else
		{
			// don't use mouse for zoom offset
			mousex = 1;
			mousey = 1;
		}
		// offset the canvas after zooming based on mouse position
		canvas_move(drill, offsetx2*mousex, offsety2*mousey);
		// release scrollbars
		g_signal_handler_unblock(hscroll, hscroll_id);
		g_signal_handler_unblock(vscroll, vscroll_id);
		return;
	}
	return;
}



void zoom_fit(GtkWidget *widget)
{
	zoom_amnt(0.0, false);
	return;
}



void canvas_apply(cairo_t *cr)
{
	// Scale and move canvas to correct amounts

	// scale canvas to correct zoom amount
	cairo_scale(cr, fldstate.zoom_amnt, fldstate.zoom_amnt);
	// move canvas to highlight section of field
	// note: for some reason, (x,y) must be negated to translate correctly
	//cairo_translate(cr, -1*fldstate.fieldx, -1*fldstate.fieldy);
	cairo_translate(cr, -1*hscroll->value, -1*vscroll->value);
	return;
}



void canvas_move(GtkWidget *widget, double valuex, double valuey)
{
	double fieldx, fieldy;
	// move the canvas up, down, left, or right
	// Move horizontally if specified
	fieldx = fldstate.fieldx + valuex;
	// move vertically if specified
	fieldy = fldstate.fieldy + valuey;

	// Bounds checking
	if (fieldx < 0)
		fieldx = 0;
	if (fieldx + hscroll->page_size > hscroll->upper)
		fieldx = hscroll->upper - hscroll->page_size;
	if (fieldy < 0)
		fieldy = 0;
	if (fieldy + vscroll->page_size > vscroll->upper)
		fieldy = vscroll->upper - vscroll->page_size;

	// apply new position
	hscroll->value = fieldx;
	vscroll->value = fieldy;
	// redraw canvas
	do_field = 1;
	gtk_widget_queue_draw_area(window, mybox->allocation.x, mybox->allocation.y, mybox->allocation.width, mybox->allocation.height);
	fldstate.fieldx = hscroll->value;
	fldstate.fieldy = vscroll->value;

	return;
}



gboolean handle_mouse_scroll_event(GtkWidget *widget, GdkEventScroll *event)
{
	// handle zoom events
	// propagate everything except control modifier
	if (event->direction == GDK_SCROLL_UP)
	{
		if (event->state == 0)
		{
			// no modifiers
			// move up
			canvas_move(widget, 0.0, -1*vscroll->step_increment);
		}
		else if (event->state == 1)
		{
			// shift modifier
			// move left
			canvas_move(widget, -1*hscroll->step_increment, 0.0);
		}
		else if (event->state == 4)
		{
			// ctrl modifier
			// zoom in
			zoom_in(widget, true);
		}
	}
	else if (event->direction == GDK_SCROLL_DOWN)
	{
		if (event->state == 0)
		{
			// move down
			canvas_move(widget, 0.0, vscroll->step_increment);
		}
		else if (event->state == 1)
		{
			// shift modifier
			// move right
			canvas_move(widget, hscroll->step_increment, 0.0);
		}
		else if (event->state == 4)
		{
			// ctrl modifier
			// zoom out
			zoom_out(widget, true);
		}
	}
	else if (event->direction == GDK_SCROLL_LEFT)
		canvas_move(widget, -1*hscroll->step_increment, 0.0);
	else if (event->direction == GDK_SCROLL_RIGHT)
		canvas_move(widget, hscroll->step_increment, 0.0);
	return TRUE;
}



void zoom_in(GtkWidget *widget, bool from_mouse)
{
	// zoom canvas in
	zoom_amnt(1.1, from_mouse);
}



void zoom_out(GtkWidget *widget, bool from_mouse)
{
	// zoom canvas out
	zoom_amnt(0.9, from_mouse);
}



void zoom_standard(GtkWidget *widget)
{
	// zoom canvas to 100%
	zoom_amnt(0, false);
}



GType gtk_drill_get_type(void)
{
	static GType gtk_drill_type = 0;
	if (!gtk_drill_type)
	{
		const GTypeInfo gtk_drill_info =
		{
			sizeof(GtkDrillClass),
			NULL, 	// base_init
			NULL, 	// base_finalize
			(GClassInitFunc) gtk_drill_class_init,
			NULL,	// class_finalize
			NULL,	// class_data
			sizeof(GtkDrill),
			0,	// n_preallocs
			(GInstanceInitFunc) gtk_drill_init,
		};
		gtk_drill_type = g_type_register_static(GTK_TYPE_WIDGET,
				"GtkDrill",
				&gtk_drill_info,
				0);
	}
	return gtk_drill_type;
}



void gtk_drill_set_state(GtkDrill *drill, gint num)
{
	drill->sel = num;
	gtk_drill_paint(GTK_WIDGET(drill));
}



GtkWidget * gtk_drill_new(void)
{
	return GTK_WIDGET(g_object_new(GTK_DRILL_TYPE, NULL));
}



enum {
	DOT_RIGHT,
	DOT_LEFT,
	DOT_UP,
	DOT_DOWN,
	LAST_SIGNAL
};


/*
enum {
	PROP_0,
	PROP_HADJUSTMENT,
	PROP_VADJUSTMENT,
	PROP_SHADOW_TYPE
};

*/



static void gtk_drill_class_init(GtkDrillClass *class)
{
	//printf("ping drill_class_init\n");
	GtkWidgetClass *widget_class;
	GtkObjectClass *object_class;
	//GtkBindingSet *binding_set;
	//gchar *binder;

	widget_class = (GtkWidgetClass *) class;
	object_class = (GtkObjectClass *) class;
	//binding_set = gtk_binding_set_new(binder);

	widget_class->realize = gtk_drill_realize;
	widget_class->size_request = gtk_drill_size_request;
	widget_class->size_allocate = gtk_drill_size_allocate;
	widget_class->expose_event = gtk_drill_expose;
	widget_class->motion_notify_event = xy_movement;
	widget_class->button_press_event = clicked;
	widget_class->button_release_event = unclicked;
	widget_class->scroll_event = handle_mouse_scroll_event;


	object_class->destroy = gtk_drill_destroy;
	/*
	g_object_class_install_property (gobject_class, PROP_HADJUSTMENT, g_param_spec_object ("hadjustment", 
				//P_("Horizontal adjustment"),
				NULL,
				//P_("The GtkAdjustment that determines the values of the horizontal position for this viewport"),
				NULL,
				GTK_TYPE_ADJUSTMENT,
				G_PARAM_CONSTRUCT));
	//GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property (gobject_class,
			PROP_VADJUSTMENT,
			g_param_spec_object ("vadjustment",
				//P_("Vertical adjustment"),
				NULL,
				//P_("The GtkAdjustment that determines the values of the vertical position for this viewport"),
				NULL,
				GTK_TYPE_ADJUSTMENT,
				G_PARAM_CONSTRUCT));

	widget_class->set_scroll_adjustments_signal =
		g_signal_new (("set-scroll-adjustments"),
				G_OBJECT_CLASS_TYPE (gobject_class),
				G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
				G_STRUCT_OFFSET (GtkViewportClass, set_scroll_adjustments),
				NULL, NULL,
				NULL,
				G_TYPE_NONE, 2,
				GTK_TYPE_ADJUSTMENT,
				GTK_TYPE_ADJUSTMENT);
		// */

}



static void gtk_drill_init (GtkDrill *drill)
{
	//printf("ping drill init\n");
	drill->sel = 0;
}



static void gtk_drill_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
	//printf("gtk_drill request\n");
	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_DRILL(widget));
	g_return_if_fail(requisition != NULL);
	//printf("ping size request\n");

}



static void gtk_drill_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	//printf("gtk_drill size allocate\n");
	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_DRILL(widget));
	g_return_if_fail(allocation != NULL);

	widget->allocation = *allocation;

	if (gtk_widget_get_realized(widget))
	{
		gdk_window_move_resize(
				widget->window,
				allocation->x, allocation->y,
				allocation->width, allocation->height
				);
	}
}



static void gtk_drill_realize(GtkWidget *widget)
{
	//printf("ping realize\n");
	//GtkDrill *drill;
	GdkWindowAttr attributes;
	guint attributes_mask;

	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_DRILL(widget));

	GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);
	//drill = GTK_DRILL(widget);

	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.window_type = GDK_WINDOW_CHILD;

	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events (widget) 
		| GDK_EXPOSURE_MASK
		| GDK_POINTER_MOTION_MASK
		| GDK_BUTTON_PRESS_MASK
		| GDK_BUTTON_RELEASE_MASK
		| GDK_SCROLL_MASK;

	attributes_mask = GDK_WA_X | GDK_WA_Y;

	widget->window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);
	widget->style = gtk_style_attach(widget->style, widget->window);

	gdk_window_set_user_data(widget->window, widget);

	gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);
}



static gboolean gtk_drill_expose(GtkWidget *widget, GdkEventExpose *event)
{
	//printf("ping expose\n");
	g_return_val_if_fail(widget != NULL, FALSE);
	g_return_val_if_fail(GTK_IS_DRILL(widget), FALSE);
	g_return_val_if_fail(event != NULL, FALSE);

	//gtk_drill_paint(widget);
	/*
	   if (pstate.first_time)
	   field_init();
	   */
	draw_field(widget);
	// Never EVER explicitly redraw the field from here.
	// It'll kill performance.
	// I leave it here as a message of what not to do
	//do_field=1;
	return FALSE;
}



static void gtk_drill_paint(GtkWidget *widget)
{
	//printf("ping paint\n");

	//cairo_t *cr;
	//cr = gdk_cairo_create(widget->window);
	//cairo_set_source_rgb(cr, 0, 0, 0);
	//cairo_paint(cr);
	//cairo_destroy(cr);
}



static void gtk_drill_destroy(GtkObject *object)
{
	//GtkDrill *drill;
	GtkDrillClass *klass;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_DRILL(object));

	//drill = GTK_DRILL(object);
	klass = gtk_type_class(gtk_widget_get_type());

	if (GTK_OBJECT_CLASS(klass)->destroy) {
		(* GTK_OBJECT_CLASS(klass)->destroy) (object);
	}
}


extern int set_tot;		// total amount of sets
extern int setnum;
extern int do_field;
extern int playing;
extern int set_step;
extern GTimer * timer;
double xstep, yheight;	



void force_redraw(GtkWidget *widget)
{	// Refresh the field
	do_field=1;
	gtk_widget_queue_draw_area(window, drill->allocation.x, drill->allocation.y, drill->allocation.width, drill->allocation.height);
}



int field_init(void)
{
	// initialize the field
	// currently not used
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, fldstate.width, fldstate.height);
	field = cairo_create (surface);
	gaks = cairo_create (surface);
	fnums = cairo_create (surface);
	dots = gdk_cairo_create(drill->window);
	selected = gdk_cairo_create(drill->window);
	pstate.first_time = 0;
	//do_field = 1;
	return 0;
}



void def_canvas (GtkWidget *widget)
{
	// define the canvas sizing

	double c_width;
	double c_height;
	// use default values for width and height
	if (fldstate.width != widget->allocation.width || fldstate.height != widget->allocation.height)
	{
		do_field=1;
	}
	// update width and height
	fldstate.width = widget->allocation.width;	// Get the width
	fldstate.height = widget->allocation.height;	// Get the height
	c_width = fldstate.width;
	c_height = fldstate.height;
	fldstate.xoff = (int)c_width % 160; 		// extra margin for the width
	if (!fldstate.xoff)	// need some margin
		fldstate.xoff = (int)(c_width-1) % 160;
	fldstate.xo2 = fldstate.xoff / 2;			// half of the offset
	fldstate.canv_step = (c_width-fldstate.xoff) / 160;	// length of one 8:5 step
	yheight = fldstate.canv_step * 85;		// height of the field
	if (yheight > c_height)
	{	// limiting factor is height, adjust
		fldstate.yoff = (int)c_height % 85;
		fldstate.yo2 = fldstate.yoff / 2;
		fldstate.canv_step = (c_height - fldstate.yoff) / 85;
		yheight = fldstate.canv_step * 85;
		fldstate.xoff = c_width - (160*fldstate.canv_step);
		fldstate.xo2 = fldstate.xoff / 2;
	}
	else
	{
		fldstate.yoff = c_height - yheight;	// y offset
		fldstate.yo2 = fldstate.yoff / 2;			// half of the offset
	}
	fldstate.xo2 = fldstate.xoff / 2;
	fldstate.yo2 = fldstate.yoff / 2;

	return;
}



void drawing_method(cairo_t *cdots, double x, double y)
{
	// dot drawing
	double size = 0.5;
	double oset = size*fldstate.canv_step;
	cairo_move_to(cdots, x-oset, y-oset);
	cairo_rel_line_to(cdots, 2*oset, 2*oset);
	cairo_move_to(cdots, x-oset, y+oset);
	cairo_rel_line_to(cdots, 2*oset, -2*oset);
	//cairo_new_sub_path(cdots);
	//cairo_arc(cdots, x, y, 2*(double)fldstate.canv_step/3, 0, 360);
	return;
}



int pixel_to_field(double *x_r, double *y_r)
{
	// convert event coords on screen to
	// field coordinates for dots
	double x, y;
	x = *x_r;
	y = *y_r;

	x = x / fldstate.zoom_amnt;
	y = y / fldstate.zoom_amnt;

	x = x + fldstate.fieldx;
	y = y + fldstate.fieldy;


	x = (x-fldstate.xo2) / fldstate.canv_step;
	y = (y-fldstate.yo2) / fldstate.canv_step;


	*x_r = x;
	*y_r = y;
	return 0;
}



int field_to_pixel(double *x_r, double *y_r)
{
	// convert field coordinates to 
	// locations to draw on a screen
	double x, y;
	x = *x_r;
	y = *y_r;

	x = fldstate.xo2 + fldstate.canv_step * x;
	y = fldstate.yo2 + fldstate.canv_step * y;

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
	double xmin = -1, xmax = -1;
	double ymin = -1, ymax = -1;
	double xfield, yfield;
	double offsetx, offsety;
	double stepoff;

	double sel_xmin, sel_xmax;
	double sel_ymin, sel_ymax;

	cairo_destroy(selected);
	selected = gdk_cairo_create(widget->window);
	canvas_apply(selected);
	cairo_set_line_width(selected, 1.5);
	cairo_set_source_rgb(selected, 1, 0, 0);


	cairo_destroy(select_drag);
	select_drag = gdk_cairo_create(widget->window);
	canvas_apply(select_drag);
	cairo_set_line_width(select_drag, 1.5);
	cairo_set_source_rgba(select_drag, 1, 0, 0, 0.5);


	cairo_destroy(selected_area);
	selected_area = gdk_cairo_create(widget->window);
	canvas_apply(selected_area);
	cairo_set_line_width(selected_area, 1.0);
	cairo_set_source_rgba(selected_area, 0, 0, 1, 0.1);


	// get set information
	sets = pshow->sets;
	currset = sets->currset;
	// get select information
	select = pstate.select;
	if ((fldstate.mouse_clicked & 0x1) == 0x1)
	{
		// show mouse drag select
		sel_xmin = fldstate.mouse_clickx;
		sel_ymin = fldstate.mouse_clicky;
		sel_xmax = fldstate.mousex;
		sel_ymax = fldstate.mousey;
	}
	else
	{
		sel_xmin = sel_xmax = 0;
		sel_ymin = sel_ymax = 0;
	}
	while (select != NULL)
	{
		index = select->index;
		retr_midset(currset, index, &x, &y);
		xfield = x;
		yfield = y;
		field_to_pixel(&x, &y);
		drawing_method(selected, x, y);
		select = select->next;
		if ((fldstate.mouse_clicked & 0x2) == 0x2)
		{
			// show dots being moved
			offsetx = fldstate.mouse_clickx - fldstate.mousex;
			offsety = fldstate.mouse_clicky - fldstate.mousey;
			xfield -= offsetx;
			yfield -= offsety;
			field_to_pixel(&xfield, &yfield);
			drawing_method(select_drag, xfield, yfield);
			x = xfield;
			y = yfield;
		}
		if (xmin == -1 || x < xmin)
			xmin = x;
		if (xmax == -1 || x > xmax)
			xmax = x;
		if (ymin == -1 || y < ymin)
			ymin = y;
		if (ymax == -1 || y > ymax)
			ymax = y;

	}
	if (xmin != xmax || ymin != ymax)
	{
		// show border for selected dots
		stepoff = 0.75*fldstate.canv_step;
		cairo_rectangle(selected_area, xmin-stepoff, ymin-stepoff, (xmax-xmin)+2*stepoff, (ymax-ymin)+2*stepoff);
		cairo_fill(selected_area);
		cairo_set_source_rgba(selected_area, 0, 0, 1, 0.5);
		cairo_rectangle(selected_area, xmin-stepoff, ymin-stepoff, (xmax-xmin)+2*stepoff, (ymax-ymin)+2*stepoff);
		cairo_stroke(selected_area);
	}
	if (fldstate.mouse_clicked == 0x1)
	{
		// show selection box
		cairo_set_source_rgba(selected_area, 0, 0, 1, 0.1);
		field_to_pixel(&sel_xmin, &sel_ymin);
		field_to_pixel(&sel_xmax, &sel_ymax);
		cairo_rectangle(selected_area, sel_xmin, sel_ymin, (sel_xmax-sel_xmin), (sel_ymax-sel_ymin));
		cairo_fill(selected_area);
		cairo_set_source_rgba(selected_area, 0, 0, 1, 0.5);
		cairo_rectangle(selected_area, sel_xmin, sel_ymin, (sel_xmax-sel_xmin), (sel_ymax-sel_ymin));
		cairo_stroke(selected_area);
	}
	cairo_stroke(selected);
	//cairo_fill (selected);
	cairo_stroke(select_drag);
	//cairo_fill (select_drag);
	do_selected = 0;
	return 0;
}



int draw_dots (GtkWidget *widget)
{
	int i;		// loop variable
	// set containers
	struct set_proto *currset;	
	struct set_proto *lastset;
	// performer container
	struct perf_proto **perf;
	// coordinates
	double x, y;	// x and y location
	// canvases
	struct select_proto *selects;
	int was_selected;

	if (do_dots)
	{
		// Draw the field
		cairo_destroy(surface_write);
		surface_write = gdk_cairo_create(widget->window);

		cairo_set_source_surface(surface_write, surface, 1, 1);
		cairo_paint (surface_write);


		// Define canvases
		cairo_destroy(dots);
		dots = gdk_cairo_create(widget->window);
		canvas_apply(dots);

		cairo_set_line_width(dots, 1.5);

		// grab sets from data structure
		currset = pshow->sets->currset;
		lastset = currset->next;
		perf = pshow->perfs;



		// Draw dots grayed out if stepped
		if (pshow && pstate.curr_step)
		{
			cairo_set_source_rgb(dots, 0.3, 0.3, 0.3);
		}
		else
			cairo_set_source_rgb(dots, 0, 0, 0);
		// get first selected dot
		selects = pstate.select;
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
			if (pstate.curr_step >= lastset->counts)
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
		{
			pstate.first_time = 0;
			fldstate.zoom_amnt = 1;
			fldstate.fieldx = 0;
			fldstate.fieldy = 0;
			fldstate.mouse_clicked = 0;
			fldstate.mouse_selection = NULL;
		}
		surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, fldstate.width, fldstate.height);
		field = cairo_create (surface);
		canvas_apply(field);
		gaks = cairo_create (surface);
		canvas_apply(gaks);
		fnums = cairo_create (surface);
		canvas_apply(fnums);
		cairo_set_source_rgb(fnums, .7, .7, .7);
		cairo_set_font_size(fnums, 20);

		cairo_set_source_rgb(field, 1, 1, 1);	
		cairo_paint (field);

		for (i=fldstate.xo2; i<=fldstate.width-(int)fldstate.xo2; i+=(fldstate.width-(int)fldstate.xoff)/20)
		{	// Yardlines
			cairo_set_line_width(field, 1);
			cairo_set_source_rgb(field, 0, 0, 0);
			cairo_move_to (field, i, fldstate.height-yheight-fldstate.yo2);
			cairo_line_to (field, i, yheight+fldstate.yo2);
			// Yardline Numbers
			sprintf(text, "%i", ynum);
			cairo_text_extents (fnums, text, &te);
			//cairo_get_font_options(fnums, fopts);
			//cairo_font_options_set_height(fopts, 4*fldstate.canv_step);
			x_bear = te.x_bearing + te.width / 2;
			//y_bear = te.y_bearing + te.height / 2;
			cairo_move_to (fnums, i - x_bear, fldstate.height-fldstate.yo2-fldstate.canv_step*12);
			cairo_show_text(fnums, text);
			cairo_move_to (fnums, i - x_bear, fldstate.height-fldstate.yo2-fldstate.canv_step*73);
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
			cairo_move_to (field, i-2*fldstate.canv_step, fldstate.height-fldstate.yo2-fldstate.canv_step*32);
			cairo_line_to (field, i+2*fldstate.canv_step, fldstate.height-fldstate.yo2-fldstate.canv_step*32);
			// Back Hash
			cairo_move_to (field, i-2*fldstate.canv_step, fldstate.height-fldstate.yo2-fldstate.canv_step*53);
			cairo_line_to (field, i+2*fldstate.canv_step, fldstate.height-fldstate.yo2-fldstate.canv_step*53);
			cairo_stroke (field);


			// Split Yardlines
			if (i<((int)fldstate.xo2+(int)fldstate.canv_step*160))
			{
				// Light Stroke
				// only draw if window is large enough
				cairo_set_line_width (gaks, 0.5);
				//cairo_set_source_rgb(gaks, 0.9, 0.9, 0.9);
				cairo_set_source_rgb(gaks, 0.9, 0.9, 1);
				if (fldstate.width-fldstate.xo2 > 800)
				{
					for (j=i; j<i+(int)fldstate.canv_step*4; j+=(int)fldstate.canv_step)
					{	// 1-step yardlines
						cairo_move_to (gaks, j, fldstate.height-yheight-fldstate.yo2);
						cairo_line_to (gaks, j, yheight+fldstate.yo2);

						cairo_move_to (gaks, j+fldstate.canv_step*5, fldstate.height-yheight-fldstate.yo2);
						cairo_line_to (gaks, j+fldstate.canv_step*5, yheight+fldstate.yo2);
					}
					for (j=fldstate.yo2+yheight; j>=fldstate.yo2; j-=(int)fldstate.canv_step)
					{	// 1-step y-grid
						if (((int)(j-fldstate.yo2-fldstate.canv_step)%(int)(fldstate.canv_step*4)) == 0)
							continue;
						cairo_move_to (gaks, i, j);
						cairo_line_to (gaks, i+fldstate.canv_step*8, j);
					}
					// Light Stroke Draw
					cairo_stroke (gaks);
				}

				// Med Stroke
				// only draw if window is large enough
				if (fldstate.width-fldstate.xo2 > 600)
				{
					// 4-step X
					cairo_set_source_rgb(gaks, 0.5, 0.5, 0.9);
					cairo_move_to (gaks, i+(int)fldstate.canv_step*4, fldstate.height-yheight-fldstate.yo2);
					cairo_line_to (gaks, i+(int)fldstate.canv_step*4, yheight+fldstate.yo2);

					for (j=fldstate.yo2+yheight; j>=fldstate.yo2; j-=4*(int)fldstate.canv_step)
					{	// 4-Step Gaks
						cairo_move_to (gaks, i, j);
						cairo_line_to (gaks, i+(int)fldstate.canv_step*8, j);
					}
					// Med Stroke Draw
					cairo_stroke(gaks);
				}
			}
		}
	}
	// Cleanup
	// Show the dots
	draw_dots(widget);

	// Make sure field won't be redrawn by default
	do_field = 0;
	update_entries();
	dr_sidebar_update((DrSidebar*)sidebar);


}
