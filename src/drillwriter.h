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
//#include "selection/selectcon.h"
#include "structures.h"
//#include "forms/dr_forms.h"



int do_field;			// whether or not to redraw field
int do_dots;
int do_selected;
int set_step;
//int expose_flag;
int perf_cur;	// current selected performer
// check to make sure void surface isn't destroyed


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



