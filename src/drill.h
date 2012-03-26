/* drill.h */

#ifndef __DRILL_H
#define __DRILL_H

#include <gtk/gtk.h>
#include <glib.h>
#include <cairo.h>
#include <math.h>
#include <gdk/gdkkeysyms.h>


int setnum;	// current set
int set_tot;	// Total amount of sets
int do_field;	// whether or not to redraw field
double xoff, yoff;		// x offset
double xo2, yo2;		// y offset
double width, height;
GtkWidget *window;		// Everything goes in here
int playing;
int set_step;
double step;
int expose_flag;
int counts[50];
char set_buf[5];
char count_buf[5];
char perf_buf[5];
char perf_buf_x[5];
char perf_buf_y[5];
int tempo; // tempo
GtkWidget *entry_sets;
GtkWidget *entry_counts;
GtkWidget *entry_perf;
GtkWidget *entry_perf_x;
GtkWidget *entry_perf_y;
double perf[50][50][2];	// Set, performer, dot
int perf_cur;	// current selected performer
int perfnum;

gdouble time_elapsed;
GTimer *timer;

G_BEGIN_DECLS

#define GTK_DRILL(obj) GTK_CHECK_CAST(obj, gtk_drill_get_type (), GtkDrill)
#define GTK_DRILL_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_drill_get_type(), GtkDrillClass)
#define GTK_IS_DRILL(obj) GTK_CHECK_TYPE(obj, gtk_drill_get_type())


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


GtkType gtk_drill_get_type(void);

GtkWidget * gtk_drill_new();

G_END_DECLS

#endif // __DRILL_H
