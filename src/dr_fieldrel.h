#include <stdbool.h>
// fieldrel.c
void fieldrel_get_midpoint(double x1, double y1, double x2, double y2, double *mx, double *my);
void fieldrel_get_mid_coords(coord_t *coord1, coord_t *coord2, double *mx, double *my);
bool fieldrel_check_dots_within_range(double x1, double y1, double x2, double y2);
int fieldrel_check_is_inside_yard(double *x, double *y, int *field_side);
int fieldrel_get_yardline_number(double *x, double *y);
double fieldrel_get_side_to_side(double *x, double *y);
double fieldrel_get_front_to_back(double *x, double *y, char **inorout_r, char **frontback_r, char **hashorside_r);
int fieldrel_convert_xy_to_relation(double *x, double *y, char **buffer_r);
select_t *field_get_in_area(double x, double y);
bool field_check_dot_in_rectangle(double x, double y, double x1, double y1, double x2, double y2);
select_t *field_select_in_rectangle(select_t*, double, double, double, double, bool);

