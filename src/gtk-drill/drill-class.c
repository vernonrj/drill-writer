// drill.c

//#include "drill.h"
#include "d_gtk.h"

static void gtk_drill_class_init(GtkDrillClass *klass);
static void gtk_drill_init(GtkDrill *drill);
static void gtk_drill_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_drill_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_drill_realize(GtkWidget *widget);
static gboolean gtk_drill_expose(GtkWidget *widget, GdkEventExpose *event);
//static gboolean timer_exec(GtkWidget *window, GtkWidget *widget);
static void gtk_drill_paint(GtkWidget *widget);
static void gtk_drill_destroy(GtkObject *object);


//extern int do_field;
extern double width, height;
extern int expose_flag;

void zoom_amnt(double x, double y)
{
	if (x == 0 && y == 0)
	{
		// zoom to 100%
		//pstate.zoom_x = width;
		//pstate.zoom_y = height;
		pstate.zoom_x = 921;
		pstate.zoom_y = 550;
	}
	else
	{
		pstate.zoom_x = pstate.zoom_x + x;
		pstate.zoom_y = pstate.zoom_y + y;
		do_field = 1;
	}
	gtk_widget_set_size_request(drill, pstate.zoom_x, pstate.zoom_y);
	return;
}
void zoom_fit(GtkWidget *widget)
{
	double s_width, s_height;
	s_width = (double)scrolled_window->allocation.width;
	s_height = (double)scrolled_window->allocation.height;
	//printf("scrolled = (%.2f, %.2f)\n", s_width, s_height);
	//printf("window = (%.2f, %.2f)\n", width, height);
	if (pstate.width > s_width || pstate.height > s_height)
	{
		pstate.zoom_x = s_width;
		pstate.zoom_y = s_height+20;
	}
	else
	{
		pstate.zoom_x = pstate.width;
		pstate.zoom_y = pstate.height+20;
	}
	do_field = 1;
	gtk_widget_set_size_request(drill, pstate.zoom_x, pstate.zoom_y);
	return;
}


gboolean zoom_scroll(GtkWidget *widget, GdkEventScroll *event)
{
	// handle zoom events
	// propagate everything except control modifier
	double width, height;
	double xoff, yoff;
	GtkAdjustment *xadj, *yadj;
	width = (double)scrolled_window->allocation.width / 2;
	height = (double)scrolled_window->allocation.height / 2;
	if (event->state == 0)
		return FALSE;
	else if (event->state != 4)
		return FALSE;
	if (event->direction == GDK_SCROLL_UP)
	{
		// zoom in
		zoom_amnt(10, 10);
		//pstate.zoom_x = pstate.zoom_x + 10;
		//pstate.zoom_y = pstate.zoom_y + 10;
		/*
		xadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
		yadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
		xoff = gtk_adjustment_get_value(GTK_ADJUSTMENT(xadj)) + 1;//width;
		yoff = gtk_adjustment_get_value(GTK_ADJUSTMENT(yadj)) + 1;//height;
		gtk_adjustment_set_value(GTK_ADJUSTMENT(xadj), xoff);
		gtk_adjustment_set_value(GTK_ADJUSTMENT(yadj), yoff);
		gtk_scrolled_window_set_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window), GTK_ADJUSTMENT(xadj));
		gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window), GTK_ADJUSTMENT(yadj));
		*/
		gtk_widget_set_size_request(widget, pstate.zoom_x, pstate.zoom_y);
	}
	else if (event->direction == GDK_SCROLL_DOWN)
	{
		// zoom out
		zoom_amnt(-10, -10);
		//pstate.zoom_x = pstate.zoom_x - 10;
		//pstate.zoom_y = pstate.zoom_y - 10;
		gtk_widget_set_size_request(widget, pstate.zoom_x, pstate.zoom_y);
	}
	return TRUE;
}

void zoom_in(GtkWidget *widget)
{
	// zoom in
	zoom_amnt(20, 20);
	gtk_widget_set_size_request(drill, pstate.zoom_x, pstate.zoom_y);
}

void zoom_out(GtkWidget *widget)
{
	// zoom out
	zoom_amnt(-20, -20);
	gtk_widget_set_size_request(drill, pstate.zoom_x, pstate.zoom_y);
}

void zoom_standard(GtkWidget *widget)
{
	// zoom to 100%
	zoom_amnt(0, 0);
	gtk_widget_set_size_request(drill, pstate.zoom_x, pstate.zoom_y);
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
	//return GTK_WIDGET(g_object_new(gtk_drill_get_type()));
	return GTK_WIDGET(g_object_new(GTK_DRILL_TYPE, NULL));
}

//G_DEFINE_TYPE (GtkDrill, gtk_drill, G_TYPE_NONE);
enum {
	DOT_RIGHT,
	DOT_LEFT,
	DOT_UP,
	DOT_DOWN,
	LAST_SIGNAL
};

//static guint drill_signals[LAST_SIGNAL] = {0};

static void gtk_drill_class_init(GtkDrillClass *class)
{
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
	widget_class->scroll_event = zoom_scroll;
	
	object_class->destroy = gtk_drill_destroy;
	/*
	drill_signals[DOT_RIGHT] =
		g_signal_new ("dotright",
				G_OBJECT_CLASS_TYPE (object_class),
				G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
				G_STRUCT_OFFSET (GtkDrillClass, dotright),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);
	gtk_binding_entry_add_signal(binding_set, GDK_KEY_D, 0, "dotright", 0);
	
	binding_set = gtk_binding_set_by_class(class);
	*/
}

static void gtk_drill_init (GtkDrill *drill)
{
	drill->sel = 0;
	pstate.zoom_x = 800;
	pstate.zoom_y = 480;
}

static void gtk_drill_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_DRILL(widget));
	g_return_if_fail(requisition != NULL);
	//printf("ping size request\n");


	//requisition->width = pstate.zoom_x;
	//requisition->height = pstate.zoom_y;

	//requisition->width = 850;
	//requisition->height = 450;
}

static void gtk_drill_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
	//printf("ping allocate\n");
	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_DRILL(widget));
	g_return_if_fail(allocation != NULL);

	widget->allocation = *allocation;

	if (GTK_WIDGET_REALIZED(widget))
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

	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;

	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events (widget) 
		| GDK_EXPOSURE_MASK
		| GDK_POINTER_MOTION_MASK
		| GDK_BUTTON_PRESS_MASK
		| GDK_BUTTON_RELEASE_MASK
		| GDK_SCROLL_MASK;

	attributes_mask = GDK_WA_X | GDK_WA_Y;

	widget->window = gdk_window_new(
			gtk_widget_get_parent_window(widget),
			& attributes, attributes_mask
			);

	gdk_window_set_user_data(widget->window, widget);

	widget->style = gtk_style_attach(widget->style, widget->window);
	gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);
	//g_signal_connect(widget, "dotright", G_CALLBACK (gtk_main_quit), NULL);
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
	expose_flag=0;
	// Never EVER explicitly redraw the field from here.
	// It'll kill performance.
	// I leave it here as a message of what not to do
	//do_field=1;
	return FALSE;
}

static void gtk_drill_paint(GtkWidget *widget)
{
	
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

