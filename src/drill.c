// drill.c

#include "drill.h"

static void gtk_drill_class_init(GtkDrillClass *klass);
static void gtk_drill_init(GtkDrill *drill);
static void gtk_drill_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_drill_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_drill_realize(GtkWidget *widget);
static gboolean gtk_drill_expose(GtkWidget *widget, GdkEventExpose *event);
static gboolean timer_exec(GtkWidget *window, GtkWidget *widget);
static void gtk_drill_paint(GtkWidget *widget);
static void gtk_drill_destroy(GtkObject *object);

//extern int do_field;
extern double width, height;
extern int expose_flag;

GtkType gtk_drill_get_type(void)
{
	static GtkType gtk_drill_type = 0;


	if (!gtk_drill_type) {
		static const GtkTypeInfo gtk_drill_info = {
			"GtkDrill",
			sizeof(GtkDrill),
			sizeof(GtkDrillClass),
			(GtkClassInitFunc) gtk_drill_class_init,
			(GtkObjectInitFunc) gtk_drill_init,
			NULL,
			NULL,
			(GtkClassInitFunc) NULL
		};
		gtk_drill_type = gtk_type_unique(GTK_TYPE_WIDGET, &gtk_drill_info);
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
	return GTK_WIDGET(gtk_type_new(gtk_drill_get_type()));
}

//G_DEFINE_TYPE (GtkDrill, gtk_drill, G_TYPE_NONE);
enum {
	DOT_RIGHT,
	DOT_LEFT,
	DOT_UP,
	DOT_DOWN,
	LAST_SIGNAL
};

static guint drill_signals[LAST_SIGNAL] = {0};

static void gtk_drill_class_init(GtkDrillClass *klass)
{
	GtkWidgetClass *widget_class;
	GtkObjectClass *object_class;
	GtkBindingSet *binding_set;
	gchar *binder;

	widget_class = (GtkWidgetClass *) klass;
	object_class = (GtkObjectClass *) klass;
	binding_set = gtk_binding_set_new(binder);

	widget_class->realize = gtk_drill_realize;
	widget_class->size_request = gtk_drill_size_request;
	widget_class->size_allocate = gtk_drill_size_allocate;
	widget_class->expose_event = gtk_drill_expose;
	
	object_class->destroy = gtk_drill_destroy;
	drill_signals[DOT_RIGHT] =
		g_signal_new ("dotright",
				G_OBJECT_CLASS_TYPE (object_class),
				G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
				G_STRUCT_OFFSET (GtkDrillClass, dotright),
				NULL, NULL,
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE, 0);
	gtk_binding_entry_add_signal(binding_set, GDK_KEY_D, 0, "dotright", 0);
	
	binding_set = gtk_binding_set_by_class(klass);
}

static void gtk_drill_init (GtkDrill *drill)
{
	drill->sel = 0;
}

static void gtk_drill_size_request(GtkWidget *widget, GtkRequisition *requisition)
{
	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_DRILL(widget));
	g_return_if_fail(requisition != NULL);

	requisition->width = 80;
	requisition->height = 100;
}

static void gtk_drill_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
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
	GdkWindowAttr attributes;
	guint attributes_mask;

	g_return_if_fail(widget != NULL);
	g_return_if_fail(GTK_IS_DRILL(widget));

	GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = 80;
	attributes.height = 100;

	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;

	attributes_mask = GDK_WA_X | GDK_WA_Y;

	widget->window = gdk_window_new(
			gtk_widget_get_parent_window(widget),
			& attributes, attributes_mask
			);

	gdk_window_set_user_data(widget->window, widget);

	widget->style = gtk_style_attach(widget->style, widget->window);
	gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);
	g_signal_connect(widget, "dotright", G_CALLBACK (gtk_main_quit), NULL);
}

static gboolean gtk_drill_expose(GtkWidget *widget, GdkEventExpose *event)
{
	g_return_val_if_fail(widget != NULL, FALSE);
	g_return_val_if_fail(GTK_IS_DRILL(widget), FALSE);
	g_return_val_if_fail(event != NULL, FALSE);

	//gtk_drill_paint(widget);
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
	GtkDrill *drill;
	GtkDrillClass *klass;
	
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_DRILL(object));

	drill = GTK_DRILL(object);
	klass = gtk_type_class(gtk_widget_get_type());

	if (GTK_OBJECT_CLASS(klass)->destroy) {
		(* GTK_OBJECT_CLASS(klass)->destroy) (object);
	}
}

