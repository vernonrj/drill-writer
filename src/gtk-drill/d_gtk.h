#include "drill.h"		// field class
#include "dr-media.h"		// mediabar class
#include "dr-drill.h"
#include "dr-setbar.h"		// setbar class
#include "dr-perfbar.h"
#include "dr-sidebar.h"
#include "dr-sidebar-sets.h"
#include "dr-sidebar-groups.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <cairo.h>
#include <glib-object.h>
#include <gdk/gdkkeysyms.h>
GtkWidget *mybox;		// container for drill and scrollbars

GtkWidget *window;		// container for a show
GtkWidget *drill;		// custom cairo widget
GtkWidget *scrolled_window;	// for the field
// class holding info about show/set
GtkWidget *setbar;
GtkWidget *perfbar;
GtkWidget *sidebar;

// performer info
GtkWidget *entry_perf_x;
GtkWidget *entry_perf_y;

// the statusbar
GtkWidget *statusbar;
GtkWidget *menuButton;

GtkAdjustment *hscroll, *vscroll;
gulong hscroll_id, vscroll_id;

double mouse_clickx, mouse_clicky;
struct fieldstate_proto
{
	double xoff, yoff;	// field offset
	double xo2, yo2;	// field centering
	double width, height;	// allocated size
	double canv_step;	// 8:5 step in pixels
	double fieldx, fieldy;
	//double zoom_x, zoom_y;	// zoom size
	double zoom_amnt;
	int mouse_clicked;
	double mousex, mousey;
	double mouse_clickx, mouse_clicky;
	select_t *mouse_selection;
} fldstate;





gint context_id;


gdouble time_elapsed;
GTimer *timer;



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



// file-ops-gtk.c
int new_file_gtk(GtkWidget *widget);
int open_file_gtk(GtkWidget *widget);
int save_file_gtk(GtkWidget *widget);
int save_file_as_gtk(GtkWidget *widget);
int wrap_load_dep(GtkWidget *widget);
void absolute_dot (void);
void relative_dot (void);
void func_relative(void);
void show_gen(struct tempo_proto **stempo_r);

// main-gtk.c
int startTk(int argc, char *argv[]);
int runTk(void);
//static void not_implemented ();
//static void quit_action ();
void button_upifCh(GtkWidget *button, char *newinfo);
void entry_upifCh(GtkWidget *entry, char *newinfo);
int buildIfacegtk(void);
void do_undo_gtk(GtkWidget *widget);
void do_redo_gtk(GtkWidget *widget);

/*
// media-gtk.c
gboolean play_show (GtkWidget *widget);
void stop_show (GtkWidget *widget);
void queue_show (GtkWidget *widget);
void play_show_from_start (GtkWidget *widget);
*/

// perf-con-gtk.c
void view_stepsize_gtk(GtkWidget *widget);
//void goto_perf (GtkWidget *widget);
int add_perf_gtk(GtkWidget *widget);
void revert_perf_gtk(GtkWidget *widget);
void delete_perf_gtk(GtkWidget *widget);
//void xperf_change (GtkWidget *widget);
//void yperf_change (GtkWidget *widget);
void next_perf(GtkWidget *widget);
void prev_perf(GtkWidget *widget);
int select_all_gtk (GtkWidget *widget);
int select_group_gtk(GtkWidget *widget, gpointer *callback_data);
int add_group_gtk (GtkWidget *widget);
int select_mode_gtk (GtkWidget *widget);
gboolean unclicked(GtkWidget *widget, GdkEventButton *event);
gboolean clicked(GtkWidget *widget, GdkEventButton *event);
int isSelected(int index);
int checkSelected(GtkWidget *widget, GdkEventButton *event);
//int select_oneperf_gtk(GtkWidget *widget, GdkEventButton *event);

/*
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
*/
