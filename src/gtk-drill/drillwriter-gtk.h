#include "drillwriter.h"		// field class
#include "dr-media.h"		// mediabar class
#include "dr-drill-class.h"
#include "dr-setbar.h"		// setbar class
#include "dr-perfbar.h"
#include "dr-sidebar.h"
#include "dr-sidebar-sets.h"
#include "dr-sidebar-groups.h"
#include "dr-group-cell.h"

#include <gtk-3.0/gtk/gtk.h>
#include <glib.h>
#include <cairo.h>
//#include <glib-object.h>
//#include <gdk/gdkkeysyms.h>
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
	int xoff, yoff;		// field offset
	int xo2, yo2;		// field centering
	int width, height;	// allocated size
	int canv_step;		// 8:5 step in pixels
	double fieldx, fieldy;	// adjustment location
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
void coords_change_ss_entry_gtk(GtkWidget *widget);
void coords_toggle_ssYdRel_gtk(GtkWidget *widget);
void coords_toggle_ssSide_gtk(GtkWidget *widget);
void coords_toggle_ssYdLine_gtk(GtkWidget *widget);
void coords_change_fb_entry_gtk(GtkWidget *widget);
void coords_toggle_fbHashRel_gtk(GtkWidget *widget);
void coords_toggle_fbFrontBack_gtk(GtkWidget *widget);
void coords_toggle_HashSide_gtk(GtkWidget *widget);
void coords_expand_form_gtk(GtkWidget *widget);
void coords_contract_form_gtk(GtkWidget *widget);
void coords_rot_cw_gtk(GtkWidget *widget);
void coords_rot_countercw_gtk(GtkWidget *widget);
void coords_rot_cw_small_gtk(GtkWidget *widget);
void coords_rot_countercw_small_gtk(GtkWidget *widget);
void coords_dot_align_to_grid_gtk(GtkWidget *widget);
void coords_move_selected_up(GtkWidget *widget);
void coords_move_selected_down(GtkWidget *widget);
void coords_move_selected_left(GtkWidget *widget);
void coords_move_selected_right(GtkWidget *widget);
void coords_move_selected_up_small(GtkWidget *widget);
void coords_move_selected_down_small(GtkWidget *widget);
void coords_move_selected_left_small(GtkWidget *widget);
void coords_move_selected_right_small(GtkWidget *widget);

// mouse_gtk.c
int mouse_click_find_close_dot(GtkWidget *widget, GdkEventButton *event);
gboolean mouse_xy_movement(GtkWidget *widget, GdkEventMotion *event);
gboolean mouse_unclicked(GtkWidget *widget, GdkEventButton *event);
gboolean mouse_clicked(GtkWidget *widget, GdkEventButton *event);
gboolean mouse_handle_scroll_event(GtkWidget *widget, GdkEventScroll *event);



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


// perf-con-gtk.c
void view_stepsize_gtk(GtkWidget *widget);
//void goto_perf (GtkWidget *widget);
int add_perf_gtk(GtkWidget *widget);
void revert_perf_gtk(GtkWidget *widget);
void delete_perf_gtk(GtkWidget *widget);
//void xperf_change (GtkWidget *widget);
//void yperf_change (GtkWidget *widget);
//void next_perf(GtkWidget *widget);
//void prev_perf(GtkWidget *widget);
int select_all_gtk (GtkWidget *widget);
int select_form_gtk(GtkWidget *widget, form_t *form);
int select_group_gtk(GtkWidget *widget, group_t *group);
int add_group_gtk (GtkWidget *widget);
int select_mode_gtk (GtkWidget *widget);
int isSelected(int index);
form_t *dr_check_form_endpoints(GtkWidget *widget, GdkEventButton *event);
form_t *dr_check_form(GtkWidget *widget, GdkEventButton *event);
form_t *check_endpoints(double coordx, double coordy, form_t *form);
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
