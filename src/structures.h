#ifndef __DRILL_STRUCTURES__
#define __DRILL_STRUCTURES__
#include <stdbool.h>
#include <time.h>
typedef struct undo_proto undo_t;
typedef struct select_proto select_t;
typedef struct _form_container_t form_container_t;
typedef struct _form_parent_t form_parent_t;
typedef struct _form_child_t form_child_t;
typedef struct group_proto group_t;
typedef struct tempo_proto tempo_t;
typedef struct coord_proto coord_t;
typedef struct perf_proto perf_t;
typedef struct set_proto set_t;
typedef struct set_container_proto set_container_t;
typedef struct headset_proto headset_t;

// mouse mode (what executes when user clicks on field)
enum ENUM_MOUSE_MODE
{
	SELECTONE,	// select one performer
	ADDPERF,	// add performer
	ADDFORM
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
		int tempo;		// if tempo was changed
		int counts;		// if count was changed
		perf_t *sperf;		// if perf was changed (not coord)
		int pindex;		// if perf location changed
	} ud;
	coord_t **coords;		// if perf is to be deleted
	double x, y;			// relative/absolute coords
	int done;			// whether or not cascade should stop here

	undo_t *next;
};

/* Selection / Groups / Forms */

// Groups
struct group_proto
{
	// node with grouping information
	char *name;		// name of group
	select_t *selects;	// performers in group
	bool local;		// toplevel/set-local status
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






// Performer LLL nodes
struct perf_proto
{
	// node with performer information
	int index;	// index number for dots
	char *symbol;	// symbol on field
	int valid;	// should be used by program
	// last valid coordinate
};



struct set_proto
{
	// node with set information
	char *name;	// set name
	char *info;	// set info
	coord_t **coords;
	group_t *groups;
	form_child_t *forms;
	int counts;
	int midset;
	//set_t *next;
	//set_t *prev;
};


struct set_container_proto
{
	// node with info on the sets
	// in a show
	int size;
	int size_alloc;
	set_t **setlist;
	set_t *firstset;
	set_t *currset;
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
	// full forms
	form_container_t *topforms;
};

// main show container
struct headset_proto *pshow;
#endif
