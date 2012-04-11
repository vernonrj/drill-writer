/* drill.h */
//TODO: add copy for dots
// TODO: encapsulate all set structs & info into a "class"
// TODO: put currently global variables into a settings
// 		struct to pass around
// TODO: free gtk widgets

//#ifndef __DRILL_H
//#define __DRILL_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>


int setnum;			// current set
int do_field;			// whether or not to redraw field
double xoff, yoff;		// x offset
double xo2, yo2;		// y offset
double width, height;		// window width, height
int playing;
int set_step;
double step;			// size of 8:5 step in window
int expose_flag;
float zoom_x;
float zoom_y;
time_t undo_timer;
int perf_cur;	// current selected performer
// check to make sure void surface isn't destroyed
int first_time;

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
		struct set_proto *set;		// if set was deleted
		int tempo;			// if tempo was changed
		int counts;			// if count was changed
		struct perf_proto *sperf;	// if perf was changed (not coord)
		int pindex;			// if perf location changed
	} ud;
	struct coord_proto **coords;		// if perf is to be deleted
	float x, y;				// relative/absolute coords
	int done;				// whether or not cascade should stop here

	struct undo_proto *next;
};

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



struct coord_proto
{
	// stores dot for one performer on one set
	// abs/rel dot 
	bool type;
	// location info
	float x;
	float y;
};



// Performer LLL nodes
struct perf_proto
{
	// node with performer information
	char *name;	// performer name
	int index;	// index number for dots
	char *symbol;	// symbol on field
	int valid;	// should be used by program
	// last valid coordinate
	struct coord_proto *vdot;

	//struct perf_proto *next;
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
	// undo/redo branch structs
	struct undo_proto *undobr;
	struct undo_proto *redobr;
};

// main show container
struct headset_proto *pshow;

// Functions
// coords.c
// create container for dots
int coords_construct(struct coord_proto *** coords_r, int perfs);
int coord_construct(struct coord_proto **coord_r, float x, float y);
// set/retrieve coordinates from coord struct
int set_coord(struct headset_proto *dshow, int index, float x, float y);
int set_coord_valid(struct coord_proto **curr, int index, float x, float y);
int retr_coord(struct coord_proto *curr, float *x, float *y);
int retr_midset(struct set_proto *currset, int index, float *x_r, float *y_r);
int movexy(float xoff, float yoff);
int align_dots(void);
int movexy_grid(float xoff, float yoff);
void scale_form(float s_step);
void rot_form(float s_step);

// fieldrel.c
int isInsideYard(float *x, float *y, int *field_side);
int getYardline(float *x, float *y);
float getSidetoSide(float *x, float *y);
float getFronttoBack(float *x, float *y, char **inorout_r, char **frontback_r, char **hashorside_r);
void xy_to_relation(float *x, float *y, char **buffer_r);

// file-ops.c
int file_getline(FILE *fp, char **buffer_r);
void open_file(void);
void save_file(void);

// main.c
int show_construct(struct headset_proto **dshow_r, int perfs);
int show_destroy(struct headset_proto **dshow_r);
int main (int argc, char *argv[]);

// perf-con.c
// create container for performers
int perf_construct(struct perf_proto **perf_ref);
int add_perf(void);
void revert_perf_selected(struct headset_proto *dshow);
void revert_perf(struct headset_proto *dshow, int index);
void delete_perf_selected(void);
void delete_perf(struct perf_proto *perf);

// select-con.c
void update_sel_center(void);
void add_sel_center(struct coord_proto *coord);
void rem_sel_center(struct coord_proto *coord);
void select_discard(void);
int select_add(int index);
int select_all(void);

// set-controls.c
// create a set with a given amount of performers
int set_construct(struct set_proto **sets_r, int perfs);
int newset_create(struct set_proto *curr);
int set_cldestroy(struct set_proto **setcurr_r, int perfnum);
int set_destroy(void);
void goto_set(int set_buffer);
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

// tempo.c
int tempo_construct(struct tempo_proto **tempo_r, int anchorpoint);
void change_tempo(int tempo, struct tempo_proto **currtempo_r);
void update_tempo(void);

// undo.c
int undo_destroy(struct undo_proto **undlast_r, struct headset_proto *dshow);
int pushToStack(struct undo_proto *unredo, struct undo_proto **stack_r);
int pushSetMk(struct undo_proto **stack_r);
int pushSetDel(struct undo_proto **stack_r, struct set_proto *oldset);
int pushPerfMk(struct undo_proto **stack_r, int index, int done);
int pushPerfDel(struct undo_proto **stack_r, struct perf_proto **oldperf, 
		struct set_proto *firstset, int done);
int pushPerfmv(struct undo_proto **stack_r, int index, float x, float y, int done);
int pushTempo(struct undo_proto **stack_r, int tempo);
int pushCounts(struct undo_proto **stack_r, int set_num, int counts, int done);
int sourcePop(struct undo_proto **sourcebr_r);
int popFromStack(struct headset_proto *dshow, struct undo_proto **sourcebr_r,
		struct undo_proto **destbr_r);
void undo_tclose(void);



