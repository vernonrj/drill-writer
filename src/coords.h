#include "structures.h"
// coords.c
// create container for dots
coord_t **coords_construct(int perfs);
coord_t *coord_construct(void);
coord_t *coord_construct_with_data(double x, double y);
coord_t **coords_destruct(coord_t **coords, int perfs);
// set/retrieve coordinates from coord struct
int coords_set(coord_t *coord, double x, double y);
int coords_check_managed(coord_t *coord);
int coords_set_managed(coord_t *coord, int state);
int coords_retrieve(coord_t *curr, double *x, double *y);
int coords_retrieve_midset(int setnum, int index, double *x_r, double *y_r);//__attribute__((deprecated));		// use coords_get_midset instead
int coords_selected_movexy(coord_t **coords, select_t *selects, double xoff, double yoff);
int coords_align_selected_to_grid(coord_t **coords, select_t *select);
int coords_selected_movexy_grid(coord_t **coords, select_t *select, double xoff, double yoff);
void coords_constrained_resize_selection_from(set_t *set, select_t *last, double cx, double cy, double s_step);
void coords_scale_coords_from_center(double s_step, double *x_ref, double *y_ref, double xc, double yc);
void coords_rot_selection_around(set_t *set, select_t *last, double cx, double cy, double s_step);

