#include "drill.h"
#ifndef __DRILL_H
#define __DRILL_H

#include <gtk/gtk.h>
#include <glib.h>
#include <cairo.h>
#include <gdk/gdkkeysyms.h>

GtkWidget *window;		// Everything goes in here
GtkWidget *drill;		// custom cairo widget
GtkWidget *scrolled_window;	// for the field
GtkWidget *entry_sets;
GtkWidget *entry_counts;
GtkWidget *entry_tempo;
GtkWidget *entry_perf;
GtkWidget *entry_perf_x;
GtkWidget *entry_perf_y;
GtkWidget *statusbar;
// Side-to-Side relation buttons
struct gtk_ssRel
{
	// steps from yardline entry
	GtkWidget *ssStepEntry;		
	// inside or outside yardline toggle button
	GtkWidget *ssYdRelButton;	
	// side 1 or side 2 toggle button
	GtkWidget *ssSide;		
	// Yardline button
	GtkWidget *ssYdLine;		
} sidesideBtns;
// Front-to-Back relations buttons
struct gtk_fbRel
{
	// Steps from hash/sideline entry
	GtkWidget *fbStepEntry;
	// inside or outside hash/sideline toggle button
	GtkWidget *fbHashRelButton;
	// Front/Back toggle button
	GtkWidget *fbToggleButton;
	// Hash/Sideline
	GtkWidget *HashSideButton;
} frbkBtns;
gint context_id;

cairo_surface_t *surface;
cairo_t *field;
cairo_t *gaks;
cairo_t *fnums;
cairo_t *dots;

gdouble time_elapsed;
GTimer *timer;

G_BEGIN_DECLS

#define GTK_DRILL_TYPE		(gtk_drill_get_type())
#define GTK_DRILL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_DRILL_TYPE, GtkDrill))
#define GTK_DRILL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTK_DRILL_TYPE, GtkDrillClass))
#define GTK_IS_DRILL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_DRILL_TYPE))
#define GTK_IS_DRILL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_DRILL_TYPE))


//#define GTK_DRILL_CLASS(klass) GTK_CHECK_CLASS_CAST(klass, gtk_drill_get_type(), GtkDrillClass)
//#define GTK_IS_DRILL(obj) GTK_CHECK_TYPE(obj, gtk_drill_get_type())
//#define GTK_DRILL(obj) 		GTK_CHECK_CAST(obj, gtk_drill_get_type (), GtkDrill)


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


// Functions
// coords-gtk.c
void change_ss_entry(GtkWidget *widget);
void toggle_ssYdRel(GtkWidget *widget);
void toggle_ssSide(GtkWidget *widget);
void toggle_ssYdLine(GtkWidget *widget);
void change_fb_entry(GtkWidget *widget);
void toggle_fbHashRel(GtkWidget *widget);
void toggle_fbFrontBack(GtkWidget *widget);
void toggle_HashSide(GtkWidget *widget);
void expand_form(GtkWidget *widget);
void contract_form(GtkWidget *widget);
void rot_cw(GtkWidget *widget);
void rot_countercw(GtkWidget *widget);
void rot_cw_small(GtkWidget *widget);
void rot_countercw_small(GtkWidget *widget);
void dot_align_to_grid(GtkWidget *widget);
void move_up(GtkWidget *widget);
void move_down(GtkWidget *widget);
void move_left(GtkWidget *widget);
void move_right(GtkWidget *widget);
void move_up_small(GtkWidget *widget);
void move_down_small(GtkWidget *widget);
void move_left_small(GtkWidget *widget);
void move_right_small(GtkWidget *widget);
gboolean xy_movement(GtkWidget *widget, GdkEventMotion *event);

// drawfield.c
void force_redraw(GtkWidget *widget);
void def_canvas (GtkWidget *widget);
void drawing_method(cairo_t *cdots, double x, double y);
int draw_dots (GtkWidget *widget);
void draw_field (GtkWidget *widget);

// drill.c
void zoom_amnt(double x, double y);
gboolean zoom_scroll(GtkWidget *widget, GdkEventScroll *event);
void zoom_in(GtkWidget *widget);
void zoom_out(GtkWidget *widget);
void zoom_standard(GtkWidget *widget);
void zoom_fit(GtkWidget *widget);
GType gtk_drill_get_type(void);
void gtk_drill_set_state(GtkDrill *drill, gint num);
GtkWidget * gtk_drill_new(void);
static void gtk_drill_class_init(GtkDrillClass *klass);
static void gtk_drill_init (GtkDrill *drill);
static void gtk_drill_size_request(GtkWidget *widget, GtkRequisition *requisition);
static void gtk_drill_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void gtk_drill_realize(GtkWidget *widget);
static gboolean gtk_drill_expose(GtkWidget *widget, GdkEventExpose *event);
static void gtk_drill_paint(GtkWidget *widget);
static void gtk_drill_destroy(GtkObject *object);


// file-ops-gtk.c
void save_file_gtk(GtkWidget *widget);
int wrap_load_dep(GtkWidget *widget);
void absolute_dot (void);
void relative_dot (void);
void func_relative(void);
void show_gen(struct tempo_proto **stempo_r);

// main-gtk.c
int startTk(int argc, char *argv[]);
static void not_implemented ();
static void quit_action ();
void button_upifCh(GtkWidget *button, char *newinfo);
void entry_upifCh(GtkWidget *entry, char *newinfo);
int update_entries(void);
int buildIfacegtk(void);
void do_undo_gtk(GtkWidget *widget);
void do_redo_gtk(GtkWidget *widget);

// media-gtk.c
gboolean play_show (GtkWidget *widget);
void stop_show (GtkWidget *widget);
void queue_show (GtkWidget *widget);
void play_show_from_start (GtkWidget *widget);

// perf-con-gtk.c
void view_stepsize_gtk(GtkWidget *widget);
void goto_perf (GtkWidget *widget);
int add_perf_gtk(GtkWidget *widget);
void revert_perf_gtk(GtkWidget *widget);
void delete_perf_gtk(GtkWidget *widget);
void xperf_change (GtkWidget *widget);
void yperf_change (GtkWidget *widget);
void next_perf(GtkWidget *widget);
void prev_perf(GtkWidget *widget);
int select_all_gtk (GtkWidget *widget);
gboolean clicked(GtkWidget *widget, GdkEventButton *event);

// set-controls-gtk.c
void add_set_gtk(GtkWidget *widget);
void delete_set_gtk(GtkWidget *widget);
void set_first_gtk(GtkWidget *widget);
void set_last_gtk(GtkWidget *widget);
void set_next_gtk(GtkWidget *widget);
void set_next_count_gtk(GtkWidget *widget);
void set_prev_count_gtk(GtkWidget *widget);
void set_prev_gtk(GtkWidget *widget);
void set_set_name_gtk(GtkWidget *widget);
void goto_set_gtk (GtkWidget *widget);
void goto_count (GtkWidget *widget);
void change_counts (GtkWidget *widget);
void change_tempo_gtk (GtkWidget *widget);

