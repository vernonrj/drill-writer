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


int setnum;			// current set
//int set_tot;			// Total amount of sets
int do_field;			// whether or not to redraw field
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
char tempo_buf[5];
char perf_buf[5];
char perf_buf_x[5];
char perf_buf_y[5];
int tempo; // tempo
GtkWidget *entry_sets;
GtkWidget *entry_counts;
GtkWidget *entry_tempo;
GtkWidget *entry_perf;
GtkWidget *entry_perf_x;
GtkWidget *entry_perf_y;
//double perf[50][50][2];	// Set, performer, dot
int perf_cur;	// current selected performer
//int perfnum;

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



// Dot LLL nodes
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
	// count of the current set
	int step;
	// link to previous set
	struct set_proto *prevset;
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
void absolute_dot (void);
void relative_dot (void);
void func_relative(void);
void show_gen(GtkWidget *widget);

// main.c
void move_up(GtkWidget *widget);
void move_down(GtkWidget *widget);
void move_left(GtkWidget *widget);
void move_right(GtkWidget *widget);
void next_perf(GtkWidget *widget);
void prev_perf(GtkWidget *widget);
static void not_implemented ();
void force_redraw(GtkWidget *widget);
static void quit_action ();
gboolean clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void calc_stepsize (GtkWidget *widget);


// Start toolkit of choice
int startTk(int argc, char *argv[]);
int main (int argc, char *argv[]);

// media.c
gboolean play_show (GtkWidget *widget);
void stop_show (GtkWidget *widget);
void queue_show (GtkWidget *widget);
void play_show_from_start (GtkWidget *widget);

// perf-con.c
void goto_perf (GtkWidget *widget);
void xperf_change (GtkWidget *widget);
void yperf_change (GtkWidget *widget);
void add_perf (GtkWidget *widget);
void delete_perf(GtkWidget *widget);
// create container for show
int show_construct(struct headset_proto **dshow_r, int perfs);
// create a set with a given amount of performers
int set_construct(struct set_proto **sets_r, int perfs);
int newset_create(struct set_proto *curr);
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

// set-controls.c
int isLastSet(void);
void set_first(GtkWidget *widget);
void set_next(GtkWidget *widget);
void set_next_count(GtkWidget *widget);
void set_prev(GtkWidget *widget);
void set_last (GtkWidget *widget);
void goto_set (GtkWidget *widget);
void add_set (GtkWidget *widget);
void delete_set (GtkWidget *widget);
void change_tempo (GtkWidget *widget);




