#include "structures.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#ifndef __DR_FORMS_HEADER__
#define __DR_FORMS_HEADER__
// Forms
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

#endif
