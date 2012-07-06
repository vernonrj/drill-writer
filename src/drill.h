/* drill.h */
//TODO: add copy for dots
// TODO: encapsulate all set structs & info into a "class"
// TODO: free gtk widgets
// TODO: pstate.height, when being passed to
// 		gtk_widget_queue_drawing_area(),
// 		is too small. This causes the artifacts
// 		on the bottom of the screen. I don't know
// 		why it's wrong, it just is. Fix it fix it
// 		fix it fix it.

//#ifndef __DRILL_H
//#define __DRILL_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

typedef struct undo_proto undo_t;
typedef struct select_proto select_t;
// typedef struct [] fblock_t
// typedef struct [] farc_t
// typedef union [] formType_t
typedef struct form_proto form_t;
typedef struct group_proto group_t;
typedef struct tempo_proto tempo_t;
typedef struct coord_proto coord_t;
typedef struct perf_proto perf_t;
typedef struct set_proto set_t;
typedef struct set_container set_container_t;
typedef struct headset_proto headset_t;

// mouse mode (what executes when user clicks on field)
enum ENUM_MOUSE_MODE
{
	SELECTONE,	// select one performer
	SELECTDRAG,	// drag-select performers
	ADDPERF,	// add performer
	MVPERF,		// move performer
};

int mouse_currentMode;
int mouse_discarded;

// current state of show
struct showstate_proto
{
	// file ops
	char *filename;		// opened file
	// show variables
	int setnum;		// current set
	// selection list
	select_t *select;
	int selnum;
	// dot moments
	coord_t *center;
	// current count of the current set
	int curr_step;
	int playing;		// play flag
	int first_time;		// dealloc flag
	// undo state
	time_t undo_timer;	// time since last undo
	undo_t *undobr;
	undo_t *redobr;
} pstate;

/*
int setnum;			// current set
int playing;
int first_time;
double xoff, yoff;		// x offset
double xo2, yo2;		// y offset
double width, height;		// window width, height
double step;			// size of 8:5 step in window
double zoom_x;
double zoom_y;
time_t undo_timer;
*/

int do_field;			// whether or not to redraw field
int do_dots;
int do_selected;
int set_step;
int expose_flag;
int perf_cur;	// current selected performer
// check to make sure void surface isn't destroyed

// undo/redo LLL stack node
struct undo_proto
{
	// node with undo information
	// operations:
	// 0:	set created
	// 1:	set deleted
	// 2:	perf added
	// 3:	perf removed
	// 4:	perf moved
	// 5:	tempo changed
	// 6:	counts changed
	int set_num;
	int operation;
	union undo_union
	{
		set_t *set;		// if set was deleted
		int tempo;			// if tempo was changed
		int counts;			// if count was changed
		perf_t *sperf;	// if perf was changed (not coord)
		int pindex;			// if perf location changed
	} ud;
	coord_t **coords;		// if perf is to be deleted
	double x, y;				// relative/absolute coords
	int done;				// whether or not cascade should stop here

	//struct undo_proto *next;
	undo_t *next;
};

/* Selection / Groups / Forms */
// selection LLL node
struct select_proto
{
	// node with selection information
	int index;

	select_t *next;
	select_t *prev;
};




// Forms
typedef struct // fblock_t
{
	bool points[4];	// 1 0
			// 2 3
	double coords[4];
	int hFiles;	// # of horizontal files
	int vFiles;	// # of vertical files
	int **dots;	// Grid for the dots (perf nums)
			// allocation uses hFiles and vFiles
} fblock_t;

typedef struct // farc_t
{
	int points[2];	// endpoints
	double coords[2];
	double *midpts;	// midpoints
	int degree;	// number of midpoints
	int *dots;	// list of the dots (perf nums)
			// this is very intense computation,
			// so an array will be needed
} farc_t;


typedef union // formType_t
{
	fblock_t *block;
	farc_t *arc;
} formType_t;

struct  form_proto // form_t
{
	int type;
	formType_t *form;
	form_t *next;
};


// Groups
struct group_proto
{
	// node with grouping information
	select_t *selects;

	form_t *forms;
	group_t *next;
};



// tempo LLL node
struct tempo_proto
{
	// node with tempo information
	// speed
	int tempo;
	// Set anchor point
	int anchorpoint;

	tempo_t *prev;
	tempo_t *next;
};



struct coord_proto
{
	// stores dot for one performer on one set
	// Manual/Managed Dot (Used for Forms)
	bool type;
	// location info
	double x;
	double y;
};



// Performer LLL nodes
struct perf_proto
{
	// node with performer information
	//char *name;	// performer name
	int index;	// index number for dots
	char *symbol;	// symbol on field
	int valid;	// should be used by program
	// last valid coordinate
	//struct coord_proto *vdot;

	//struct perf_proto *next;
};



struct set_proto
{
	// node with set information
	char *name;	// set name
	char *info;	// set info
	coord_t **coords;
	group_t *groups;
	int counts;
	int midset;
	set_t *next;
	set_t *prev;
};


struct set_container
{
	// node with info on the sets
	// in a show
	set_t *firstset;
	set_t *currset;
	//set_t *prevset;
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
	perf_t **perfs;
	// set structure
	set_container_t *sets;
	// Tempo control
	tempo_t *currtempo;
	// Toplevel groups
	group_t *topgroups;
};

// main show container
struct headset_proto *pshow;

// Functions
// gtk functions
int startTk(int argc, char *argv[]);
int buildIfacegtk(void);
int runTk(void);
// c++ functions
int menuIface(void);

// coords.c
// create container for dots
int coords_construct(coord_t *** coords_r, int perfs);
int coord_construct(coord_t **coord_r, double x, double y);
// set/retrieve coordinates from coord struct
int set_coord(struct headset_proto *dshow, int index, double x, double y);
int set_coord_valid(coord_t **curr, int index, double x, double y);
int retr_coord(coord_t *curr, double *x, double *y);
int retr_midset(set_t *currset, int index, double *x_r, double *y_r);
//int retr_midset(set_container_t *sets, int index, double *x_r, double *y_r);
int movexy(double xoff, double yoff);
//int movexy(set_container_t *sets, struct select_proto *selects, double xoff, double yoff);
int align_dots(void);
//int align_dots(set_container_t *sets, struct select_proto *select);
int movexy_grid(double xoff, double yoff);
//int movexy_grid(set_container_t *sets, struct select_proto *selects, double xoff, double yoff);
void box_scale_form(double s_step);
void scale_form(double s_step);
void rot_form(double s_step);

// fieldrel.c
int isInsideYard(double *x, double *y, int *field_side);
int getYardline(double *x, double *y);
double getSidetoSide(double *x, double *y);
double getFronttoBack(double *x, double *y, char **inorout_r, char **frontback_r, char **hashorside_r);
int xy_to_relation(double *x, double *y, char **buffer_r);

// file-ops.c
int open_file(char *filename);
int save_file(char *filename);


// main.c
int show_construct(struct headset_proto **dshow_r, int perfs);
int show_destroy(struct headset_proto **dshow_r);
int main (int argc, char *argv[]);

// perf-con.c
// create container for performers
int perf_construct(perf_t **perf_ref);
int perf_destroy(perf_t **perf_r);
int add_perf(void);
void revert_perf_selected(struct headset_proto *dshow);
void revert_perf(struct headset_proto *dshow, int index);
void delete_perf_selected(void);
void delete_perf(perf_t *perf);
double check_stepsize_selected(struct headset_proto *dshow);
int max_stepsize_selected(struct headset_proto *dshow, double *stepsize_r);

// select-con.c
// Selection control functions
void select_dots_discard(void);
void select_dots_add(int index);
void update_sel_center(void);
void add_sel_center(coord_t *coord);
void rem_sel_center(coord_t *coord);
select_t *select_discard(select_t*);
select_t *select_add(select_t*, int index, bool toggle);
select_t *select_add_group(select_t*, group_t*, bool);
int select_all(void);
group_t *group_construct(void);
group_t *group_add_selects(group_t *group, select_t *newsels);
bool is_in_select(int index, select_t *selects);
int add_group(void);

// set-controls.c
// create a set with a given amount of performers
set_container_t *set_container_construct(int perfs);
int set_construct(set_t **sets_r, int perfs);
int newset_create(set_container_t *sets);
int set_cldestroy(set_t **setcurr_r, int perfnum);
int set_destroy(void);
void goto_set(int set_buffer);
int isLastSet(void);
int isFirstSet(void);
int add_set(void);
void delete_set(void);
void set_first(void);
void set_last(void);
void set_next(void);
void set_next_count(void);
void set_prev_count(void);
void set_prev(void);

// tempo.c
int tempo_construct(tempo_t **tempo_r, int anchorpoint);
void change_tempo(int tempo, tempo_t **currtempo_r);
void update_tempo(void);

// undo.c
int undo_destroy(undo_t **undlast_r, struct headset_proto *dshow);
int pushToStack(undo_t *unredo, undo_t **stack_r);
int pushSetMk(undo_t **stack_r);
int pushSetDel(undo_t **stack_r, set_t *oldset);
int pushPerfMk(undo_t **stack_r, int index, int done);
int pushPerfDel(undo_t **stack_r, perf_t **oldperf, 
		set_t *firstset, int done);
int pushPerfmv(undo_t **stack_r, int index, double x, double y, int done);
int pushTempo(undo_t **stack_r, int tempo);
int pushCounts(undo_t **stack_r, int set_num, int counts, int done);
int sourcePop(undo_t **sourcebr_r);
int popFromStack(struct headset_proto *dshow, undo_t **sourcebr_r,
		undo_t **destbr_r);
void undo_tclose(void);



