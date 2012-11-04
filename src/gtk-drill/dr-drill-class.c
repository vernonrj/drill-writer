// drill.c

#include "drillwriter-gtk.h"
#include "dr-drill-class.h"
#include "dr-sidebar.h"
#include "dr-perfbar.h"
#include "../dr_forms.h"
#include "../dr_select.h"
#include "../dr_fieldrel.h"

static void gtk_drill_get_preferred_width(GtkWidget *widget, gint *minimal_width, gint *natural_width);
static void gtk_drill_get_preferred_height(GtkWidget *widget, gint *minimal_height, gint *natural_height);
static void gtk_drill_class_init(GtkDrillClass *klass);
static void gtk_drill_init(GtkDrill *drill);
static void gtk_drill_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_drill_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_drill_realize(GtkWidget *widget);
static gboolean gtk_drill_expose(GtkWidget *widget, cairo_t *cr);
static void gtk_drill_paint(GtkWidget *widget);
static void gtk_drill_destroy(GtkWidget *widget);





void dr_canvas_refresh(GtkWidget *widget)
{
	// Redraw the canvas from the widget
	GtkAllocation allc;
	int x, y;
	int newx, newy;

	gtk_widget_get_allocation(widget, &allc);
	x = allc.x;
	y = allc.y;
	gtk_widget_translate_coordinates(widget, window, x, y, &newx, &newy);
	gtk_widget_queue_draw_area(window, newx, newy, allc.width, allc.height);

	return;
}



void canvas_apply(cairo_t *cr)
{
	// Scale and move canvas to correct amounts

	// scale canvas to correct zoom amount
	cairo_scale(cr, fldstate.zoom_amnt, fldstate.zoom_amnt);
	// move canvas to highlight section of field
	// note: for some reason, (x,y) must be negated to translate correctly
	cairo_translate(cr, -1*gtk_adjustment_get_value(hscroll), -1*gtk_adjustment_get_value(vscroll));
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
	// Check horizontal bounds
	if (fieldx < 0)
		fieldx = 0;
	if (fieldx + gtk_adjustment_get_page_size(hscroll) > gtk_adjustment_get_upper(hscroll))
		fieldx = gtk_adjustment_get_upper(hscroll) - gtk_adjustment_get_page_size(hscroll);
	// check vertical bounds
	if (fieldy < 0)
		fieldy = 0;
	if (fieldy + gtk_adjustment_get_page_size(vscroll) > gtk_adjustment_get_upper(vscroll))
		fieldy = gtk_adjustment_get_upper(vscroll) - gtk_adjustment_get_page_size(vscroll);

	// apply new position
	gtk_adjustment_set_value(hscroll, fieldx);
	gtk_adjustment_set_value(vscroll, fieldy);
	// redraw canvas
	dr_canvas_refresh(mybox);
	fldstate.fieldx = gtk_adjustment_get_value(hscroll);
	fldstate.fieldy = gtk_adjustment_get_value(vscroll);

	return;
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


static void gtk_drill_get_preferred_width(GtkWidget *widget, gint *minimal_width, gint *natural_width)
{
	GtkRequisition requisition;
	gtk_drill_size_request(widget, &requisition);
	*minimal_width = *natural_width = requisition.width;
}


static void gtk_drill_get_preferred_height(GtkWidget *widget, gint *minimal_height, gint *natural_height)
{
	GtkRequisition requisition;
	gtk_drill_size_request(widget, &requisition);
	*minimal_height = *natural_height = requisition.height;
}


static void gtk_drill_class_init(GtkDrillClass *class)
{
	//printf("ping drill_class_init\n");
	GtkWidgetClass *widget_class;
	//GtkObjectClass *object_class;
	//GtkBindingSet *binding_set;
	//gchar *binder;

	widget_class = (GtkWidgetClass *) class;
	//object_class = (GtkObjectClass *) class;
	//binding_set = gtk_binding_set_new(binder);

	widget_class->realize = gtk_drill_realize;
	//widget_class->size_request = gtk_drill_size_request;
	widget_class->get_preferred_width = gtk_drill_get_preferred_width;
	widget_class->get_preferred_height = gtk_drill_get_preferred_height;
	widget_class->size_allocate = gtk_drill_size_allocate;
	widget_class->draw = gtk_drill_expose;
	widget_class->motion_notify_event = mouse_xy_movement;
	widget_class->button_press_event = mouse_clicked;
	widget_class->button_release_event = mouse_unclicked;
	widget_class->scroll_event = mouse_handle_scroll_event;
	widget_class->destroy = gtk_drill_destroy;


	//object_class->destroy = gtk_drill_destroy;
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

	//widget->allocation = *allocation;
	gtk_widget_set_allocation(widget, allocation);

	if (gtk_widget_get_realized(widget))
	{
		gdk_window_move_resize(
				//widget->window,
				gtk_widget_get_window(widget),
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
	GtkAllocation allc;

	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_DRILL(widget));

	//GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);
	gtk_widget_set_realized(widget, TRUE);
	//drill = GTK_DRILL(widget);

	gtk_widget_get_allocation(widget, &allc);

	/*
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	*/
	attributes.x = allc.x;
	attributes.y = allc.y;
	attributes.width = allc.width;
	attributes.height = allc.height;
	attributes.window_type = GDK_WINDOW_CHILD;

	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events (widget) 
		| GDK_EXPOSURE_MASK
		| GDK_POINTER_MOTION_MASK
		| GDK_BUTTON_PRESS_MASK
		| GDK_BUTTON_RELEASE_MASK
		| GDK_SCROLL_MASK;

	attributes_mask = GDK_WA_X | GDK_WA_Y;

	//widget->window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);
	//widget->style = gtk_style_attach(widget->style, widget->window);
	gtk_widget_set_window(widget, gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask));
	//gtk_widget_set_style(widget, gtk_style_attach(gtk_widget_get_style(widget), gtk_widget_get_window(widget)));

	//gdk_window_set_user_data(widget->window, widget);
	gdk_window_set_user_data(gtk_widget_get_window(widget), widget);

	//gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);
	//gtk_style_set_background(gtk_widget_get_style(widget), gtk_widget_get_window(widget), GTK_STATE_NORMAL);
	gtk_style_context_set_background(gtk_widget_get_style_context(widget), gtk_widget_get_window(widget));
}



static gboolean gtk_drill_expose(GtkWidget *widget, cairo_t *cr)
{
	//printf("ping expose\n");
	g_return_val_if_fail(widget != NULL, FALSE);
	g_return_val_if_fail(GTK_IS_DRILL(widget), FALSE);
	g_return_val_if_fail(cr != NULL, FALSE);

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



static void gtk_drill_destroy(GtkWidget *widget)
{
	//GtkDrill *drill;
	GtkDrillClass *klass;

	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_DRILL(widget));

	//drill = GTK_DRILL(object);
	//klass = gtk_type_class(gtk_widget_get_type());
	klass = g_type_class_ref(gtk_widget_get_type());

	if (GTK_WIDGET_CLASS(klass)->destroy) {
		(* GTK_WIDGET_CLASS(klass)->destroy) (widget);
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
	dr_canvas_refresh(drill);
	//gtk_widget_queue_draw_area(window, drill->allocation.x, drill->allocation.y, drill->allocation.width, drill->allocation.height);
}



/*
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
*/


void def_canvas (GtkWidget *widget)
{
	// define the canvas sizing

	//double c_width;
	//double c_height;
	int c_step;
	const int x_off = 16;
	const int y_off = 16;
	GtkAllocation allocation;
	gtk_widget_get_allocation(widget, &allocation);

	// use default values for width and height
	if (fldstate.width != allocation.width || fldstate.height != allocation.height)
	{
		do_field=1;
	}
	// update width and height
	fldstate.width = allocation.width;	// Get the width
	fldstate.height = allocation.height;	// Get the height
	
	// set initial canvas step
	c_step = (int)(fldstate.width / (160 + x_off));
	// check to see if canvas step will work
	if (c_step * (85+y_off) > fldstate.height)
	{
		// vertical size is a constraint.
		// limit size accordingly
		c_step = (int)(fldstate.height / (85+y_off));
		fldstate.xoff = fldstate.width - (c_step * 160);
		fldstate.yoff = fldstate.height - (c_step * 85);
	}
	else
	{
		// vertical limit is not a problem.
		// Get offset sizes
		fldstate.xoff = (int)(fldstate.width - (c_step * 160));
		fldstate.yoff = (int)(fldstate.height - (c_step * 85));
	}
	// store canvas step
	fldstate.canv_step = c_step;
	// get convenience variables: offsets on either side
	fldstate.xo2 = (int)(fldstate.xoff / 2);
	fldstate.yo2 = (int)(fldstate.yoff / 2);
	// use deprecated variable (still needed)
	yheight = c_step * 85;

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



cairo_t *draw_selected_form(cairo_t *cr, form_child_t *form)
{
	double offsetx, offsety;
	double x1, y1, x2, y2;
	double x1rel, y1rel, x2rel, y2rel;
	double x, y;
	double xnext, ynext, xstep, ystep;
	int curr_step, counts;
	int dot_num;
	int i;
	double slopex, slopey;
	//form_list_t *flist;
	form_container_t *fcont;
	form_parent_t *fparent;
	bool form_animate = false;
	form_child_t *next_form;

	//flist = pshow->topforms;
	fcont = pshow->topforms;
	fparent = form_parent_find_with_form(fcont, form);
	curr_step = pstate.curr_step;
	if (form_parent_contiguous(fparent, pstate.setnum))
	{
		// animate
		form_animate = true;
		next_form = form_parent_find_form_at_index(fparent, (pstate.setnum+1));
		//counts = pshow->sets->currset->next->counts;
		counts = (set_get_next(pshow->sets, pstate.setnum))->counts;
	}
	else
		form_animate = false;
	// show form being moved
	offsetx = fldstate.mouse_clickx - fldstate.mousex;
	offsety = fldstate.mouse_clicky - fldstate.mousey;
	x1 = form->endpoints[0][0];
	y1 = form->endpoints[0][1];
	x2 = form->endpoints[1][0];
	y2 = form->endpoints[1][1];
	if (!fieldrel_check_dots_within_range(x1, y1, fldstate.mouse_clickx, fldstate.mouse_clicky))
	{
		x2rel = x2 - offsetx;
		y2rel = y2 - offsety;
	}
	else
	{
		x2rel = x2;
		y2rel = y2;
	}
	if (!fieldrel_check_dots_within_range(x2, y2, fldstate.mouse_clickx, fldstate.mouse_clicky))
	{
		x1rel = x1 - offsetx;
		y1rel = y1 - offsety;
	}
	else
	{
		x1rel = x1;
		y1rel = y1;
	}
	x = x1rel;
	y = y1rel;
	if (form_animate)
	{
		xnext = next_form->endpoints[0][0];
		ynext = next_form->endpoints[0][1];
		xstep = (x-xnext) / counts;
		ystep = (y-ynext) / counts;
		x -= (xstep*curr_step);
		y -= (ystep*curr_step);
	}
	field_to_pixel(&x, &y);
	cairo_move_to(cr, x, y);
	x = x2rel;
	y = y2rel;
	if (form_animate)
	{
		xnext = next_form->endpoints[0][0];
		ynext = next_form->endpoints[0][1];
		xstep = (x-xnext) / counts;
		ystep = (y-ynext) / counts;
		x -= (xstep*curr_step);
		y -= (ystep*curr_step);
	}
	field_to_pixel(&x, &y);
	cairo_line_to(cr, x, y);

	dot_num = form->dot_num;
	slopex = (x2rel - x1rel) / (dot_num - 1);
	slopey = (y2rel - y1rel) / (dot_num - 1);
	for(i=0; i<dot_num; i++)
	{
		x = i*slopex+x1rel;
		y = i*slopey+y1rel;
		field_to_pixel(&x, &y);
		if (form->fcoords[i]->dot != -1)
			drawing_method(cr, x, y);
		else
		{
			cairo_new_sub_path(cr);
			cairo_arc(cr, x, y, 2*fldstate.canv_step/3, 0, 360);
		}

	}

	return cr;
}



int draw_forms(GtkWidget *widget)
{
	form_child_t *form, *next_form;
	//form_list_t *flist;
	form_container_t *fcont;
	form_parent_t *fparent;
	//fblock_t *block;
	//farc_t *arc;
	//select_t *select;
	double x, y;
	double x2, y2;
	double xstep, ystep;
	int i;
	int dot_num;
	//int *dots;
	//double **coords;
	cairo_t *cr;
	form_coord_t **fcoords;
	coord_t *coord;
	bool form_animate = false;
	int counts = 1;
	int curr_step = pstate.curr_step;

	canv_form = cairo_create(surface);
	canvas_apply(canv_form);
	cairo_set_line_width(canv_form, 2.0);
	cairo_set_source_rgb(canv_form, 1, 0.5, 1);

	canv_form_select = cairo_create(surface);
	canvas_apply(canv_form_select);
	cairo_set_line_width(canv_form_select, 2.0);
	cairo_set_source_rgb(canv_form_select, 1, 0.5, 0);


	form = pshow->sets->currset->forms;
	//flist = pshow->topforms;
	fcont = pshow->topforms;
	while (form)
	{
		fparent = form_parent_find_with_form(fcont, form);
		if (form_parent_contiguous(fparent, pstate.setnum))
		{
			// animate
			form_animate = true;
			next_form = form_parent_find_form_at_index(fparent, (pstate.setnum+1));
			//counts = pshow->sets->currset->next->counts;
			counts = (set_get_next(pshow->sets, pstate.setnum))->counts;
		}
		else
			form_animate = false;
		fcoords = form->fcoords;
		if (form_is_selected(form, pstate.select))
		{
			cr = canv_form_select;
			if ((fldstate.mouse_clicked & 0x2) == 0x2)
			{
				canv_form_select = draw_selected_form(canv_form_select, form);
				form = form->next;
				continue;
			}
		}
		else
			cr = canv_form;
		if (form_animate || !curr_step)
		{
			dot_num = form->dot_num;
			//coords = form->coords;
			//dots = form->dots;
			x = form->endpoints[0][0];
			y = form->endpoints[0][1];
			if (form_animate)
			{
				x2 = next_form->endpoints[0][0];
				y2 = next_form->endpoints[0][1];
				xstep = (x-x2) / counts;
				ystep = (y-y2) / counts;
				x -= (xstep*curr_step);
				y -= (ystep*curr_step);
			}
			field_to_pixel(&x, &y);
			cairo_move_to(cr, x, y);
			x = form->endpoints[1][0];
			y = form->endpoints[1][1];
			if (form_animate)
			{
				x2 = next_form->endpoints[1][0];
				y2 = next_form->endpoints[1][1];
				xstep = (x-x2) / counts;
				ystep = (y-y2) / counts;
				x -= (xstep*curr_step);
				y -= (ystep*curr_step);
			}
			field_to_pixel(&x, &y);
			cairo_line_to(cr, x, y);
			for(i=0; i<dot_num; i++)
			{
				coord = fcoords[i]->coord;
				if (fcoords[i]->dot == -1)
				{
					// hole in the form
					x = coord->x + form->endpoints[0][0];
					y = coord->y + form->endpoints[0][1];
					field_to_pixel(&x, &y);
					cairo_new_sub_path(cr);
					cairo_arc(cr, x, y, 2*fldstate.canv_step/3, 0, 360);
				}
			}
		}
		form = form->next;
	}

	cairo_stroke(canv_form);
	cairo_destroy(canv_form);
	cairo_stroke(canv_form_select);
	cairo_destroy(canv_form_select);
	return 0;
}



int draw_selected(GtkWidget *widget)
{
	// draw selected dots, not normal dots
	select_t *select;
	select_t *form_select;
	//set_container_t *sets;
	//set_t *currset;
	form_child_t *form;
	int index;
	double x, y;
	double xmin = -1, xmax = -1;
	double ymin = -1, ymax = -1;
	double xfield, yfield;
	double offsetx, offsety;
	double stepoff;

	double sel_xmin, sel_xmax;
	double sel_ymin, sel_ymax;

	// context for selected dots
	selected = cairo_create(surface);
	canvas_apply(selected);
	cairo_set_line_width(selected, 1.5);
	cairo_set_source_rgb(selected, 1, 0, 0);

	// contents for selected dots (while dragging)
	select_drag = cairo_create(surface);
	canvas_apply(select_drag);
	cairo_set_line_width(select_drag, 1.5);
	cairo_set_source_rgba(select_drag, 1, 0, 0, 0.5);

	// contents for a selected area
	selected_area = cairo_create(surface);
	canvas_apply(selected_area);
	cairo_set_line_width(selected_area, 1.0);
	cairo_set_source_rgba(selected_area, 0, 0, 1, 0.1);

	// get set information
	//sets = pshow->sets;
	//currset = sets->currset;
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
		// don't show mouse drag select
		sel_xmin = sel_xmax = 0;
		sel_ymin = sel_ymax = 0;
	}
	form_select = NULL;
	select_head(select);
	while (!select_at_end(select))
	{
		//if (!form_select)
		if (!form_select && select_at_dot_end(select))
		{
			//index = select->index;
			//form = select->form;
			//index = select_get_dot(select);
			//form = select_get_form(select);
			//if (form)
			if ((index = select_get_form_advance(select)) != -1)
			{
				form = form_container_get_form_child(pshow->topforms, index);
				form_select = form_get_contained_dots(form);
				select_head(form_select);
			}
			else
				break;
		}
		if (form_select)
		{
			if (select_at_dot_end(form_select) || select_dot_empty(form_select))
			{
				form_select = select_destroy(form_select);
				continue;
			}
			index = select_get_dot_advance(form_select);
		}
		else
		{
			if (select_at_dot_end(select))
				continue;
			index = select_get_dot_advance(select);
		}
		if (index == -1)
			continue;

		coords_retrieve_midset(pstate.setnum, index, &x, &y);
		xfield = x;
		yfield = y;
		field_to_pixel(&x, &y);
		drawing_method(selected, x, y);

		if ((fldstate.mouse_clicked & 0x2) == 0x2 && !coords_check_managed_by_index(index))
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
	}
	if (fldstate.mouse_clicked == 0x1 && mouse_currentMode == SELECTONE)
	{
		// show selection box
		cairo_set_source_rgba(selected_area, 0, 0, 1, 0.1);
		field_to_pixel(&sel_xmin, &sel_ymin);
		field_to_pixel(&sel_xmax, &sel_ymax);
		cairo_rectangle(selected_area, sel_xmin, sel_ymin, (sel_xmax-sel_xmin), (sel_ymax-sel_ymin));
		cairo_fill(selected_area);
		cairo_set_source_rgba(selected_area, 0, 0, 1, 0.5);
		cairo_rectangle(selected_area, sel_xmin, sel_ymin, (sel_xmax-sel_xmin), (sel_ymax-sel_ymin));
	}
	// apply to canvas
	cairo_stroke(selected_area);
	cairo_stroke(selected);
	cairo_stroke(select_drag);

	// destroy canvas
	cairo_destroy(selected);
	cairo_destroy(select_drag);
	cairo_destroy(selected_area);

	return 0;
}



int draw_dots (GtkWidget *widget)
{
	int i;		// loop variable
	// set containers
	//set_t *currset;	
	set_t *lastset;
	// performer container
	perf_t **perf;
	// coordinates
	double x, y;	// x and y location
	// canvases
	select_t *selects;
	int was_selected;

	//if (do_dots)
	{
		// Draw the field
		//cairo_destroy(surface_write);
		//surface_write = gdk_cairo_create(widget->window);
		//surface_write = gdk_cairo_create(gtk_widget_get_window(widget));
		//surface_write = cairo_create(surface);

		//cairo_set_source_surface(surface_write, surface, 1, 1);
		//cairo_paint (surface_write);


		// Define canvases
		//cairo_destroy(dots);
		//dots = gdk_cairo_create(gtk_widget_get_window(widget));
		dots = cairo_create(surface);
		canvas_apply(dots);

		cairo_set_line_width(dots, 1.5);

		// grab sets from data structure
		//currset = pshow->sets->currset;
		//lastset = currset->next;
		lastset = set_get_next(pshow->sets, pstate.setnum);
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
		select_head(selects);
		// draw performers at certain point
		for (i=0; i<pshow->perfnum; i++)
		{	// Draw performers only if they have valid dots
			if (!perf[i])
				return -1;
			if (perf[i]->valid)
			{
				// performer should be drawn
				// Get dots for current set and next set
				coords_retrieve_midset(pstate.setnum, i, &x, &y);
				field_to_pixel(&x, &y);
				// print selection if dot is selected
				was_selected = select_check_dot(selects, i);
				/*
				was_selected = 0;
				if (selects)
				{
					// check to see if dot is selected
					//if (selects->index == i)
					if (select_get_dot(selects) == i)
					{
						// dot is selected
						//selects = selects->next;
						selects = select_get_next(selects);
						was_selected = 1;
					}
				}
				*/
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
				//lastset = pshow->sets->currset->next;
				lastset = set_get_next(pshow->sets, pstate.setnum);
			}
		}
		//if (pshow->sets->currset->next == NULL)
		if (set_get_next(pshow->sets, pstate.setnum) == NULL)
		{
			// At last set, playing is done
			pstate.playing = 0;
		}
		// Show all the dots
		cairo_stroke(dots);
		cairo_destroy(dots);

		draw_selected(widget);
	}
	return 0;
}



void draw_field (GtkWidget *widget)
{	// This function will draw the actual football field
	int i, j;		// loop vars
	char text[3];		// text for yardline names
	int ynum = 0;		// yardline numbering
	int past_fifty = 0;	// check to see if past 50 yardline
	double x_bear;		// text centering
	cairo_text_extents_t te;

	def_canvas(widget);	// Refresh dimensions and such
	if (fldstate.canv_step == 0)
		return;
	// Set up to redraw the field

	if (pstate.first_time)
	{
		pstate.first_time = 0;
		fldstate.zoom_amnt = 1;
		fldstate.fieldx = 0;
		fldstate.fieldy = 0;
		fldstate.mouse_clicked = 0;
		fldstate.mouse_selection = select_create();
	}

	// create the surface
	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, fldstate.width, fldstate.height);


	// yardlines, hashes
	field = cairo_create (surface);
	canvas_apply(field);
	cairo_set_source_rgb(field, 1, 1, 1);	
	cairo_paint (field);

	// 4-step gaks
	gaks = cairo_create (surface);
	canvas_apply(gaks);

	// text
	fnums = cairo_create (surface);
	canvas_apply(fnums);
	cairo_set_source_rgb(fnums, .7, .7, .7);
	cairo_set_font_size(fnums, 20);


	// draw 4-step gaks
	for (i=fldstate.xo2; i<fldstate.xo2+fldstate.canv_step*160; i+=fldstate.canv_step*8)
	{	// Split Yardlines
		// Light Stroke
		// only draw if window is large enough
		cairo_set_line_width (gaks, 0.5);
		//cairo_set_source_rgb(gaks, 0.9, 0.9, 0.9);
		cairo_set_source_rgb(gaks, 0.9, 0.9, 1);
		for(j=i; j<i+fldstate.canv_step*4; j+=(int)fldstate.canv_step)
		{	// 1-step yardlines
			cairo_move_to (gaks, j, fldstate.height-yheight-fldstate.yo2);
			cairo_line_to (gaks, j, yheight+fldstate.yo2);

			cairo_move_to (gaks, j+fldstate.canv_step*5, fldstate.height-yheight-fldstate.yo2);
			cairo_line_to (gaks, j+fldstate.canv_step*5, yheight+fldstate.yo2);
		}
		for (j=fldstate.yo2+yheight; j>=fldstate.yo2; j-=(int)fldstate.canv_step)
		{	// 1-step y-grid
			cairo_move_to (gaks, i, j);
			cairo_line_to (gaks, i+fldstate.canv_step*8, j);
		}
		// Light Stroke Draw
		cairo_stroke (gaks);


		// Med Stroke
		// 4-step X
		cairo_set_source_rgb(gaks, 0.5, 0.5, 0.9);
		cairo_move_to (gaks, i+(int)fldstate.canv_step*4, fldstate.yo2);
		cairo_line_to (gaks, i+(int)fldstate.canv_step*4, yheight+fldstate.yo2);

		for (j=fldstate.yo2+yheight; j>=fldstate.yo2; j-=4*(int)fldstate.canv_step)
		{	// 4-Step Gaks
			cairo_move_to (gaks, i, j);
			cairo_line_to (gaks, i+(int)fldstate.canv_step*8, j);
		}
		// Med Stroke Draw
		cairo_stroke(gaks);
	}


	// draw yardlines, yard markers
	cairo_set_line_width(field, 1);
	cairo_set_source_rgb(field, 0, 0, 0);
	for (i=fldstate.xo2; i<=fldstate.xo2+fldstate.canv_step*160; i+=fldstate.canv_step*8)
	{	// Yardlines
		cairo_move_to (field, i, fldstate.yo2);
		cairo_line_to (field, i, fldstate.height-fldstate.yo2);
		// Yardline Numbers
		sprintf(text, "%i", ynum);
		cairo_text_extents (fnums, text, &te);
		x_bear = te.x_bearing + te.width / 2;
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
		//cairo_move_to (field, i-2*fldstate.canv_step, fldstate.height-fldstate.yo2-fldstate.canv_step*32);
		//cairo_line_to (field, i+2*fldstate.canv_step, fldstate.height-fldstate.yo2-fldstate.canv_step*32);
		cairo_move_to (field, i-2*fldstate.canv_step, fldstate.yo2 + yheight - fldstate.canv_step*32);
		cairo_line_to (field, i+2*fldstate.canv_step, fldstate.yo2 + yheight - fldstate.canv_step*32);
		// Back Hash
		//cairo_move_to (field, i-2*fldstate.canv_step, fldstate.height-fldstate.yo2-fldstate.canv_step*53);
		//cairo_line_to (field, i+2*fldstate.canv_step, fldstate.height-fldstate.yo2-fldstate.canv_step*53);
		cairo_move_to (field, i-2*fldstate.canv_step, fldstate.yo2 + yheight -fldstate.canv_step*53);
		cairo_line_to (field, i+2*fldstate.canv_step, fldstate.yo2 + yheight -fldstate.canv_step*53);
		cairo_stroke (field);
	}

	// Cleanup
	cairo_destroy(field);
	cairo_destroy(gaks);
	cairo_destroy(fnums);

	// Draw selections
	draw_dots(widget);
	// Draw forms
	draw_forms(widget);

	update_entries();
	dr_sidebar_update((DrSidebar*)sidebar);

	// draw cairo surface to widget window
	surface_write = gdk_cairo_create(gtk_widget_get_window(widget));
	//cairo_set_source_surface(surface_write, surface, 1, 1);
	cairo_set_source_surface(surface_write, surface, 0, 0);
	cairo_paint (surface_write);
	cairo_destroy(surface_write);

	// destroy the surface
	cairo_surface_destroy(surface);

	return;
}
