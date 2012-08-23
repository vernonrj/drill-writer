#ifndef __DRILL_H
#define __DRILL_H

#include <unistd.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_DRILL_TYPE		(gtk_drill_get_type())
#define GTK_DRILL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_DRILL_TYPE, GtkDrill))
#define GTK_DRILL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTK_DRILL_TYPE, GtkDrillClass))
#define GTK_IS_DRILL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_DRILL_TYPE))
#define GTK_IS_DRILL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_DRILL_TYPE))


typedef struct _GtkDrill GtkDrill;
typedef struct _GtkDrillClass GtkDrillClass;

struct _GtkDrill {
	GtkWidget widget;

	gint sel;
};

struct _GtkDrillClass {
	GtkWidgetClass parent_class;
	void (* dotright) (GtkDrill *drill);
	void (* dotleft) (void);
	
};


GType gtk_drill_get_type(void);

GtkWidget * gtk_drill_new();

G_END_DECLS

#endif // __DRILL_H

cairo_surface_t *surface;
cairo_t *field;
cairo_t *gaks;
cairo_t *fnums;
cairo_t *dots;		// regular dots
cairo_t *selected; 	// selected dots
cairo_t *surface_write; // field to draw to

// drawfield.c
void force_redraw(GtkWidget *widget);
int field_init(void);
void def_canvas (GtkWidget *widget);
int pixel_to_field(double *x_r, double *y_r);
int field_to_pixel(double *x_r, double *y_r);
void drawing_method(cairo_t *cdots, double x, double y);
int draw_dots (GtkWidget *widget);
void draw_field (GtkWidget *widget);

// drill.c
void zoom_amnt(double invalue);
void canvas_apply(cairo_t *cr);
void canvas_move(GtkWidget *widget, double valuex, double valuey);
gboolean handle_mouse_scroll_event(GtkWidget *widget, GdkEventScroll *event);
void zoom_in(GtkWidget *widget);
void zoom_out(GtkWidget *widget);
void zoom_standard(GtkWidget *widget);
void zoom_fit(GtkWidget *widget);
GType gtk_drill_get_type(void);
void gtk_drill_set_state(GtkDrill *drill, gint num);
GtkWidget * gtk_drill_new(void);
/*
static void gtk_drill_class_init(GtkDrillClass *klass);
static void gtk_drill_init (GtkDrill *drill);
static void gtk_drill_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_drill_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_drill_realize(GtkWidget *widget);
static gboolean gtk_drill_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_drill_paint(GtkWidget *widget);
static void gtk_drill_destroy(GtkObject *object);
*/

