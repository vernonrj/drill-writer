/* drill.h */

#ifndef __DRILL_H
#define __DRILL_H

#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <cairo.h>
#include <math.h>
#include <gdk/gdkkeysyms.h>


GtkWidget *drill;		// custom cairo widget
GtkWidget *scrolled_window;	// for the field
int setnum;			// current set
//int set_tot;			// Total amount of sets
int do_field;			// whether or not to redraw field
double xoff, yoff;		// x offset
double xo2, yo2;		// y offset
double width, height;		// window width, height
GtkWidget *window;		// Everything goes in here
int playing;
int set_step;
double step;			// size of 8:5 step in window
int expose_flag;
float zoom_x;
float zoom_y;
//int tempo; // tempo
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
//double perf[50][50][2];	// Set, performer, dot
int perf_cur;	// current selected performer
//int perfnum;
// field surface
cairo_surface_t *surface;
cairo_t *field;
cairo_t *gaks;
cairo_t *fnums;
cairo_t *dots;
// check to make sure void surface isn't destroyed
int first_time;

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


// selection LLL node
struct select_proto
{
	// node with selection information
	int index;

	struct select_proto *next;
};


// tempo LLL node
struct tempo_proto
{
	// node with tempo information
	// speed
	int tempo;
	// Set anchor point
	int anchorpoint;

	struct tempo_proto *prev;
	struct tempo_proto *next;
};


// Performer LLL nodes
struct perf_proto
{
	// node with performer information
	char *name;	// performer name
	int index;	// index number for dots
	char *symbol;	// symbol on field
	int valid;	// should be used by program

	//struct perf_proto *next;
};


struct coord_proto
{
	// stores dot for one performer on one set
	float x;
	float y;
};
	
struct set_proto
{
	// node with set information
	char *name;	// set name
	char *info;	// set info
	struct coord_proto **coords;
	int counts;
	int midset;
	struct set_proto *next;
};

struct headset_proto
{
	// first node in sets,
	// contains extended information
	// that doesn't need to be duplicated
	
	// name of the show
	char *showname;
	// info about the show
	char *showinfo;
	// number of performers
	int perfnum;
	// links to performers
	struct perf_proto **perfs;
	// link to first set
	struct set_proto *firstset;
	// link to current set
	struct set_proto *currset;
	// link to previous set
	struct set_proto *prevset;
	// count of the current set
	int step;
	// Tempo control
	struct tempo_proto *currtempo;
	// selection list
	struct select_proto *select;
	// dot moments
	struct coord_proto *center;
	int selnum;
};

// main show container
struct headset_proto *pshow;

// Functions
// count-con.c
void goto_count (GtkWidget *widget);
void change_counts (GtkWidget *widget);

// drawfield.c
void def_canvas (GtkWidget *widget);
int draw_dots (GtkWidget *widget);
void draw_field (GtkWidget *widget);

// drill.c
GtkType gtk_drill_get_type(void);
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

// entry-con.c
static void entry_toggle_editable(GtkWidget *checkbutton, GtkWidget *entry);
static void entry_toggle_visibility(GtkWidget *checkbutton, GtkWidget *entry);

// file-ops.c
void open_file(void);
void save_file(GtkWidget *widget);
int wrap_load_dep(GtkWidget *widget);
// TODO: Deprecated commands
void absolute_dot (void);
void relative_dot (void);
void func_relative(void);
void show_gen(struct tempo_proto **stempo_r);
// end deprecated commands

// main.c
int movexy(float xoff, float yoff);
int movexy_grid(float xoff, float yoff);
void dot_align_to_grid(GtkWidget *widget);
int align_dots(void);
void move_up(GtkWidget *widget);
void move_down(GtkWidget *widget);
void move_left(GtkWidget *widget);
void move_right(GtkWidget *widget);
void move_up_small(GtkWidget *widget);
void move_down_small(GtkWidget *widget);
void move_left_small(GtkWidget *widget);
void move_right_small(GtkWidget *widget);
void next_perf(GtkWidget *widget);
void prev_perf(GtkWidget *widget);
static void not_implemented ();
void force_redraw(GtkWidget *widget);
static void quit_action ();
int getYardline(float *x, float *y);
int isInsideYard(float *x, float *y, int *field_side);
float getSidetoSide(float *x, float *y);
float getFronttoBack(float *x, float *y, gchar **inorout_r, gchar **frontback_r, gchar **hashorside_r);
gboolean xy_movement(GtkWidget *widget, GdkEventMotion *event);
gboolean clicked(GtkWidget *widget, GdkEventButton *event);
void calc_stepsize (GtkWidget *widget);


// Start toolkit of choice
int startTk(int argc, char *argv[]);
int update_entries(void);
int buildIfacegtk(void);
int main (int argc, char *argv[]);

// media.c
gboolean play_show (GtkWidget *widget);
void stop_show (GtkWidget *widget);
void queue_show (GtkWidget *widget);
void play_show_from_start (GtkWidget *widget);

// perf-con.c
void change_ss_entry(GtkWidget *widget);
void toggle_ssYdRel(GtkWidget *widget);
void toggle_ssSide(GtkWidget *widget);
void toggle_ssYdLine(GtkWidget *widget);
void change_fb_entry(GtkWidget *widget);
void toggle_fbHashRel(GtkWidget *widget);
void toggle_fbFrontBack(GtkWidget *widget);
void toggle_HashSide(GtkWidget *widget);
void scale_form(float s_step);
void rot_form(float s_step);
void expand_form(GtkWidget *widget);
void contract_form(GtkWidget *widget);
void rot_cw(GtkWidget *widget);
void rot_countercw(GtkWidget *widget);
void rot_cw_small(GtkWidget *widget);
void rot_countercw_small(GtkWidget *widget);
void goto_perf (GtkWidget *widget);
void xperf_change (GtkWidget *widget);
void yperf_change (GtkWidget *widget);
int select_all_gtk (GtkWidget *widget);
int add_perf (GtkWidget *widget);
void delete_perf(GtkWidget *widget);
// create container for show
int show_construct(struct headset_proto **dshow_r, int perfs);
int tempo_construct(struct tempo_proto **tempo_r, int anchorpoint);
// create a set with a given amount of performers
int set_construct(struct set_proto **sets_r, int perfs);
int newset_create(struct set_proto *curr);
int set_destroy(void);
// create container for dots
int coord_construct(struct coord_proto *** coords_r, int perfs);
// create container for performers
int perf_construct(struct perf_proto **dots_r);
// set/retrieve coordinates from coord struct
int set_coord(struct coord_proto *curr, float x, float y);
int set_coord_valid(struct coord_proto **curr, int index, float x, float y);
int retr_coord(struct coord_proto *curr, float *x, float *y);
// not used yet
int dot_realloc(struct perf_proto ***dots_r, int oldsize, int newsize);
int dot_new_set(struct perf_proto ***dots_r, int setnum);
int dot_destroy(struct perf_proto ***dots_r, int size);
void update_sel_center(void);
void add_sel_center(struct coord_proto *coord);
void rem_sel_center(struct coord_proto *coord);
void select_discard(void);
int select_add(int index);
int select_all(void);

// set-controls.c
void zoom_amnt(float x, float y);
gboolean zoom_scroll(GtkWidget *widget, GdkEventScroll *event);
void zoom_in(GtkWidget *widget);
void zoom_out(GtkWidget *widget);
void zoom_standard(GtkWidget *widget);
void add_set_gtk(GtkWidget *widget);
void delete_set_gtk(GtkWidget *widget);
void set_first_gtk(GtkWidget *widget);
void set_last_gtk(GtkWidget *widget);
void set_next_gtk(GtkWidget *widget);
void set_next_count_gtk(GtkWidget *widget);
void set_prev_count_gtk(GtkWidget *widget);
void set_prev_gtk(GtkWidget *widget);
void set_set_name_gtk(GtkWidget *widget);
void goto_set (GtkWidget *widget);
void change_tempo(int tempo, struct tempo_proto **currtempo_r);
void change_tempo_gtk (GtkWidget *widget);

int isLastSet(void);
int isFirstSet(void);
void add_set(void);
void delete_set(void);
void set_first(void);
void set_last(void);
void set_next(void);
void set_next_count(void);
void set_prev_count(void);
void set_prev(void);
void update_tempo(void);




