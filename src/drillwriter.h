/* drill.h */
//TODO: add copy for dots
// TODO: encapsulate all set structs & info into a "class"
// TODO: free gtk widgets

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "bsd-queue.h"

typedef struct undo_proto undo_t;
typedef struct select_proto select_t;
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


int do_field;			// whether or not to redraw field
int do_dots;
int do_selected;
int set_step;
//int expose_flag;
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
// Forms
typedef struct _form_container_t form_container_t;
struct _form_container_t
{
	form_parent_t **forms;
	int size;
	int size_alloc;
};

struct _form_parent_t
{
	// container for forms
	//LIST_ENTRY(_form_parent_t) formlist_entries;
	//int* set_index;
	form_child_t **forms;
	int index;
	//int size;
	//int size_alloc;
};
//LIST_HEAD(form_list_proto, _form_parent_t);
//typedef struct form_list_proto form_list_t;

typedef struct form_coord_proto
{
	// info about coordinate conained
	// in form
	int dot;		// performer
	int form_num;		// number of mapped forms (>=1)
	int form_alloc;		// memory allocated for forms
	form_child_t **forms;		// forms this dot is mapped to
	int *dot_type;		// types for forms
	coord_t *coord;		// coordinate
} form_coord_t;

struct  _form_child_t // form_child_t
{
	char *name;		// name of form
	// form type:
	// 	0:	no form
	// 	1:	straight
	// 	2:	curve
	// 	3:	circular?
	// 	4:	ovoid?
	int type;		
	int dot_num;		// number of dots
	double endpoints[2][2];	
	form_coord_t **fcoords;	// coord data
	form_child_t *next;
	form_parent_t *parent;
};


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



struct coord_proto
{
	// stores dot for one performer on one set
	int type;	// manual/managed/endpoint (0/1/2)
	// location info
	double x;
	double y;

	int form_num;
	int form_alloc;
	form_child_t **forms;
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

// Functions
// gtk functions
int startTk(int argc, char *argv[]);
int buildIfacegtk(void);
int runTk(void);
// c++ functions
//int menuIface(void);

// coords.c
// create container for dots
coord_t **coords_construct(int perfs);
coord_t *coord_construct(void);
coord_t *coord_construct_with_data(double x, double y);
coord_t **coords_destruct(coord_t **coords, int perfs);
// set/retrieve coordinates from coord struct
int coords_set_coord(struct headset_proto *dshow, int index, double x, double y);
int coords_set_coord_valid(coord_t **curr, int index, double x, double y);
int coords_check_managed(coord_t *coord);
int coords_set_managed(coord_t *coord, int state);
int coords_check_managed_by_index(int index);
int coords_set_managed_by_index(int index, int state);
int coords_track_form(int index, form_child_t *form);
int coords_untrack_form(int index, form_child_t *form);
int coords_retrieve(coord_t *curr, double *x, double *y);
int coords_retrieve_midset(int setnum, int index, double *x_r, double *y_r);
int coords_movexy(double xoff, double yoff);
int coords_align_dots_to_grid(void);
int coords_movexy_grid(double xoff, double yoff);
void coords_box_scale_form_from_center(double s_step);
void coords_scale_coords_from_center(double s_step, double *x_ref, double *y_ref, double xc, double yc);
void coords_scale_form_from_center(double s_step);
void coords_rot_selected_around_center(double s_step);

// forms.c
form_coord_t *fcoord_construct(void);
form_child_t *form_child_construct(form_parent_t*);
form_child_t *form_child_construct_with_size(form_parent_t*, int index);
form_child_t *form_child_destruct(form_child_t *form);
form_child_t *form_add_hole_around_index(form_child_t *form, int index, bool after);
form_child_t *form_remove_index(form_child_t *form, int index);
form_child_t *form_remove_from(form_child_t *curr, form_child_t *form_head);
bool form_is_selected(form_child_t *form, select_t *select);
bool form_endpoint_contains_coords(form_child_t *form, double x, double y);
bool form_endpoint_hole_contains_coords(form_child_t *form, double x, double y);
bool form_contains_coords(form_child_t *form, double x, double y);
bool form_hole_contains_coords(form_child_t *form, double x, double y);
int form_find_index_with_coords(form_child_t *form, double x, double y);
form_child_t *form_find_form_with_index(form_child_t *form, int index);
form_child_t *form_find_with_coords(form_child_t *form, double x, double y);
form_child_t *form_find_with_hole(form_child_t *form, double x, double y);
form_child_t *form_find_with_endpoint(form_child_t *form, double x, double y);
form_child_t *form_find_with_endpoint_hole(form_child_t *form, double x, double y);
form_child_t *form_find_with_attr(select_t *select, double x, double y, bool (*fptr)(form_child_t*,double,double));
form_child_t *form_find_selected_with_endpoint(select_t *select, double x, double y);
form_child_t *form_find_selected_with_endpoint_hole(select_t *select, double x, double y);
form_child_t *form_add_index_to_hole_with_coords(form_child_t *form, int index, double x, double y);
form_child_t *form_build_line(form_parent_t *form_parent, select_t *select_head);
bool form_contained_in_rectangle(form_child_t *form, double x1, double y1, double x2, double y2);
int form_update_line(form_child_t *form);
int form_set_endpoint(form_child_t *form, double x1, double y1, double x2, double y2);
int form_set_endpoint_grid(form_child_t *form, double x1, double y1, double x2, double y2);
int form_move_endpoint(form_child_t *form, double x1, double y1, double x2, double y2);
int form_move_endpoint_grid(form_child_t *form, double x1, double y1, double x2, double y2);
int form_movexy(form_child_t *form, double x, double y);
int form_unmanage_dot(form_child_t *form, int index);
select_t *form_get_contained_dots(form_child_t *form);
void form_add_to_set(form_child_t *form, set_t *currset);
void form_add_to_current_set(form_child_t *form);
coord_t **form_get_coords(form_child_t *form);
coord_t *form_get_coord_near(form_child_t *form, double x, double y);
void form_scale_from_center(form_child_t *form, double s_step);
void form_rotate_around_center(form_child_t *form, double s_step);
select_t *form_flatten(form_child_t *form, select_t *select_head);
form_child_t *form_copy(form_child_t *form);
form_parent_t *form_parent_construct(void);
form_parent_t *form_parent_construct_with_form(form_child_t *form, int index);
form_parent_t *form_parent_realloc(form_parent_t *form, int size, int size_realloc);
form_parent_t *form_parent_destruct(form_parent_t *last);
bool form_parent_contiguous(form_parent_t *fparent, int set_num);
form_container_t *form_parent_add_form(form_container_t *head, form_child_t *form, int index);
int form_parent_add_set(form_container_t *head, form_parent_t *last, int index);
int form_parent_copy_to(form_parent_t *last, int index, int transpose);
int form_parent_remove_set(form_container_t *head, form_parent_t *last, int index);
form_parent_t *form_parent_find_with_form(form_container_t *head, form_child_t *form);
form_child_t *form_parent_find_form_at_index(form_parent_t *last, int index);
bool form_parent_mapped_at_set(form_parent_t *last, int setnum);
form_container_t *form_container_construct(void);
form_container_t *form_container_destruct(form_container_t *fcont);
form_container_t *form_container_realloc(form_container_t *fcont, size_t size_alloc);
void form_container_resize_parents(form_container_t *fcont, size_t oldsize, size_t newsize);
void form_container_move_children_including(form_container_t *fcont, int setnum);
form_container_t *form_container_insert_head(form_container_t *fcont, form_parent_t *last);
form_parent_t *form_container_get_form_parent(form_container_t *fcont, int index);
form_child_t *form_container_get_form_child_at_set(form_container_t *fcont, int index, int setnum);
form_child_t *form_container_get_form_child(form_container_t *fcont, int index);

// fieldrel.c
bool fieldrel_check_dots_within_range(double x1, double y1, double x2, double y2);
int fieldrel_check_is_inside_yard(double *x, double *y, int *field_side);
int fieldrel_get_yardline_number(double *x, double *y);
double fieldrel_get_side_to_side(double *x, double *y);
double fieldrel_get_front_to_back(double *x, double *y, char **inorout_r, char **frontback_r, char **hashorside_r);
int fieldrel_convert_xy_to_relation(double *x, double *y, char **buffer_r);
select_t *field_get_in_area(double x, double y);
bool field_check_dot_in_rectangle(double x, double y, double x1, double y1, double x2, double y2);
select_t *field_select_in_rectangle(select_t*, double, double, double, double, bool);

// file-ops.c
int open_file(char *filename);
int save_file(char *filename);

// group.c
// grouping functions
group_t *group_construct(void);
group_t *group_add_selects(group_t *group, select_t *newsels);
group_t *group_remove_selects(group_t *group, select_t *select);
group_t *group_remove_from(group_t *group, group_t *curr);
group_t *group_pop_from(group_t *group, group_t *curr);
void group_add_to_set(group_t *group);
void group_add_global(group_t *group);
//bool group_is_selected(group_t *group, select_t *select);

// drillwriter.c
int show_construct(struct headset_proto **dshow_r, int perfs);
int show_destroy(struct headset_proto **dshow_r);
int main (int argc, char *argv[]);

// perf-con.c
// create container for performers
perf_t *perf_construct(void);
int perf_destroy(perf_t **perf_r);
int perf_add(void);
void perf_revert_selected(struct headset_proto *dshow);
void perf_revert(struct headset_proto *dshow, int index);
void perf_delete_selected(void);
void perf_delete(perf_t *perf);
double perf_average_stepsize_selected(struct headset_proto *dshow);
int perf_max_stepsize_selected(struct headset_proto *dshow, double *stepsize_r);



// select-con.c
// Selection control functions
/*
select_t *select_construct(void);
select_t *select_construct_with_index(int index);
int select_has_next(select_t *select);
select_t *select_get_next(select_t *select);
void select_set_next(select_t *select, select_t *last);
int select_has_index(select_t *select);
int select_get_dot(select_t *select);
int select_has_form(select_t *select);
form_child_t *select_get_form(select_t *select);
bool select_check_index_selected(int index, select_t *selects);
select_t *select_add_index(select_t*, int index, bool toggle);
void select_dots_add_index(int index);
select_t *select_add_group(select_t *select, group_t *group);
select_t *select_add_form(select_t*, form_child_t*, bool);
void select_add_multiple(select_t **mainlist_r, select_t **modifier_r, bool toggle);
select_t *select_drop_multiple(select_t *mainlist, select_t *modifier);
select_t *select_discard(select_t*);
void select_dots_discard(void);
select_t *select_push(select_t *mainlist, select_t **modifier_r, bool toggle);
void select_push_all(select_t **mainlist_r, select_t **modifier_r, bool toggle);
select_t *select_all(select_t*, perf_t **perfs, int perfnum);
int select_all_dots(void);
void select_update_center(select_t *last);
select_t *select_update_scope_set1_set2(select_t *select_head, set_t *currset, set_t *nextset);
*/

int select_get_dot_advance(select_t *select);
int select_get_form_advance(select_t *select);
int select_head(select_t *select);
int select_head_dot(select_t *select);
int select_head_form(select_t *select);
bool select_at_dot_end(select_t *select);
bool select_at_form_end(select_t *select);
bool select_at_end(select_t *select);
bool select_dot_empty(select_t *select);
bool select_form_empty(select_t *select);
bool select_empty(select_t *select);
void select_clear_dots(select_t *select);
void select_clear_forms(select_t *select);
void select_clear(select_t *select);
int select_get_dot(select_t *select);
int select_get_form(select_t *select);
int select_check_dot(select_t *select, int x);
int select_check_form(select_t *select, int x);
int select_add_dot(select_t *select, int x);
int select_add_form(select_t *select, int x);
int select_toggle_dot(select_t *select, int x);
int select_toggle_form(select_t *select, int x);
int select_remove_dot(select_t *select, int x);
int select_remove_form(select_t *select, int x);
int select_add_multiple(select_t *select, select_t *modifier);
int select_add_multiple_dots(select_t *select, select_t *modifier);
int select_add_multiple_forms(select_t *select, select_t *modifier);
int select_toggle_multiple(select_t *select, select_t *modifier);
int select_toggle_multiple_dots(select_t *select, select_t *modifier);
int select_toggle_multiple_forms(select_t *select, select_t *modifier);
int select_remove_multiple(select_t *select, select_t *modifier);
int select_remove_multiple_dots(select_t *select, select_t *modifier);
int select_remove_multiple_forms(select_t *select, select_t *modifier);
select_t *select_create_with_size(size_t dot_size, size_t form_size);
select_t *select_create(void);
int select_init_with_size(select_t *select, int dot_size, int form_size);
int select_init(select_t *select);
select_t *select_destroy(select_t *select);
int select_update_center(select_t *select);
void select_all_dots(void);



// set-controls.c
// create a set with a given amount of performers
set_container_t *set_container_construct(int perfs);
set_container_t *set_container_add_set_before(set_container_t *set_container, set_t *newset, int setnum);
set_container_t *set_container_add_before(set_container_t *set_container, int setnum);
set_container_t *set_container_add_after(set_container_t *set_container, int setnum);
set_container_t *set_container_add_set_after(set_container_t *set_container, set_t *newset, int setnum);
set_container_t *set_container_destruct(set_container_t *set_container);
set_t *set_construct_before(set_t *sets, int perfs);
set_t *set_construct_after(set_t *sets, int perfs);
int set_construct(set_t **sets_r, int perfs);
int newset_create(set_container_t *sets);
int set_cldestroy(set_t **setcurr_r, int perfnum);
void set_destroy(int set_index);
void goto_set(int set_buffer);
int isLastSet(void);
int isFirstSet(void);
int set_add_before_current(void);
int set_add_after_current(void);
//int add_set(void);
void delete_set(void);
set_t *set_get_first(set_container_t *set_container);
set_t *set_get_last(set_container_t *set_container);
set_t *set_get_next(set_container_t *set_container, int index);
set_t *set_get_prev(set_container_t *set_container, int setnum);
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



