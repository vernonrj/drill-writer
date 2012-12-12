#include "../dr_forms.h"
#include "../drillwriter.h"
#include "../dr_select.h"
#include "../dr_fieldrel.h"
#include "../coords.h"
#include <string.h>



struct _form_child_t // form_child_t
{
	char *name;		// name of form
	// form type:
	// 	0:	no form
	// 	1:	straight
	// 	2:	curve
	// 	3:	circular?
	// 	4:	ovoid?
	int type;		
	int dot_alloc;
	//double endpoints[2][2];	
	//form_coord_t **fcoords;	// coord data
	int *dotgrid;
	coord_t **coords;

	form_child_t *next;
	form_parent_t *parent;
};


/*
form_coord_t *fcoord_construct(void)
{
	form_coord_t *fcoord;
	fcoord = (form_coord_t*)malloc(sizeof(form_coord_t));
	fcoord->dot = -1;
	fcoord->coord = coord_construct();
	fcoord->forms = (form_child_t**)malloc(sizeof(form_child_t*));
	fcoord->forms[0] = NULL;
	fcoord->dot_type = (int*)malloc(sizeof(int));
	fcoord->dot_type[0] = 1;
	fcoord->form_num = 1;
	fcoord->form_alloc = 1;
	return fcoord;
}
*/


form_child_t *form_child_construct(form_parent_t *parent)
{
	return form_child_construct_with_size(parent, 3);
}



form_child_t *form_child_construct_with_size(form_parent_t *parent, int dot_alloc)
{
	form_child_t *form;

	form = (form_child_t*)malloc(sizeof(form_child_t));
	if (dot_alloc < 3)
		dot_alloc = 3;

	// initialize
	form->next = NULL;
	form->type = 0;
	form->name = (char*)malloc(sizeof(char));
	form->name[0] = '\0';
	form->parent = parent;
	/*
	for (i=0; i<2; i++)
		form->endpoints[i][0] = form->endpoints[i][1] = 0;
		*/
	form->dot_alloc = dot_alloc;
	form->coords = coords_construct(dot_alloc);
	form->dotgrid = (int*)malloc(dot_alloc*sizeof(int));
	//form->fcoords = (form_coord_t**)malloc(index*sizeof(form_coord_t*));
	/*
	for (i=0; i<index; i++)
	{
		form->fcoords[i] = fcoord_construct();
		form->fcoords[i]->forms[0] = form;
	}
	form->fcoords[0]->dot_type[0] = 2;
	if (index > 0)
		form->fcoords[i-1]->dot_type[0] = 2;
		*/
	return form;
}





form_child_t *form_child_destruct(form_child_t *form)
{
	int i;
	int dot_alloc;
	form_child_t *last;
	//int *dots;
	bool is_selected = form_is_selected(form, pstate.select);
	int dot;
	//form_coord_t **fcoords;
	coord_t **coords = pshow->sets->currset->coords;
	coord_t **form_coords = form->coords;
	if (is_selected)
		pstate.select = form_flatten(form, pstate.select);


	last = form->next;
	//fcoords = form->fcoords;
	if (form->name)
		free(form->name);
	dot_alloc = form->dot_alloc;
	for (i=0; i<dot_alloc; i++)
	{
		dot = form->dotgrid[i];
		if (dot != -1)
		{
			if (is_selected)
			{
				//select_add_dot(pstate.select, fcoords[i]->dot);
				select_add_dot(pstate.select, dot);
			}
			//coords_set_managed_by_index(fcoords[i]->dot, 0);
			coords_set_managed(coords[dot], 0);
		}
		//free(form->fcoords[i]->forms);
		//free(form->fcoords[i]->dot_type);
		//free(form->fcoords[i]->coord);
		//free(form->fcoords[i]);
	}
	coords_destruct(form_coords, form->dot_alloc);
	free(form->dotgrid);
	//free(form->fcoords);
	free(form);
	return last;
}


int form_child_get_index(form_child_t *form)
{
	return form->parent->index;
}


form_child_t *form_child_get_next(form_child_t *form)
{
	if (form)
		return form->next;
	else
		return NULL;
}

int form_child_get_attr(form_child_t *form, form_coord_attr_t *formattr)
{
	formattr->dot_alloc = form->dot_alloc;
	return 0;
}


form_parent_t *form_child_get_parent(form_child_t *form)
{
	return form->parent;
}

int form_child_get_coord(form_child_t *form, int index, 
		form_coord_attr_t *formattr, double *x, double *y)
{
	if (index < form->dot_alloc)
	{
		coords_retrieve(form->coords[index], x, y);
		formattr->dot_alloc = form->dot_alloc;
		formattr->formindex = index;
		formattr->realindex = form->dotgrid[index];
		formattr->exists = (form->dotgrid[index] >= 0);
		return 0;
	}
	return -1;
}


form_child_t *form_add_hole_around_index(form_child_t *form, int index, bool after)
{
	int i;
	int piv = 0;
	//form_coord_t **fcoords;
	//form_coord_t **fcoords_new;
	coord_t **coords = form->coords;
	coord_t **coords_new;
	int dot_alloc;

	//fcoords = form->fcoords;
	dot_alloc = form->dot_alloc;

	//fcoords_new = (form_coord_t**)malloc((dot_num+1)*sizeof(form_coord_t*));
	//coords_new = coords_construct(dot_alloc+1);
	coords_new = (coord_t**)malloc((dot_alloc)*sizeof(coord_t*));
	for(i=0; i<dot_alloc; i++)
	{
		//if (fcoords[i]->dot == index)
		if (form->dotgrid[i] == index)
		{
			//fcoords_new[i+after] = fcoord_construct();
			coords_new[i] = coord_construct();
			piv++;
			//fcoords_new[i+piv-after] = fcoords[i];
			coords_new[i+piv-after] = coords[i];
		}
		else
			coords_new[i+piv] = coords[i];
	}
	form->dot_alloc++;
	free(coords);
	form->coords = coords_new;

	return form;
}




form_child_t *form_remove_index(form_child_t *form, int index)
{
	int i;
	int ii = 0;
	int piv_index = -1;
	//form_coord_t **fcoords;
	//form_coord_t **fcoords_new;
	coord_t **coords, **coords_new;
	int dot_alloc;

	//fcoords = form->fcoords;
	dot_alloc = form->dot_alloc;
	coords = form->coords;

	//fcoords_new = (form_coord_t**)malloc((dot_num-1)*sizeof(form_coord_t*));
	coords_new = (coord_t**)malloc((dot_alloc-1)*sizeof(coord_t*));
	for(i=0; i<dot_alloc; i++)
	{
		//if (fcoords[i]->dot != index)
		if (form->dotgrid[i] != index)
		{
			coords_new[ii] = coords[i];
			ii++;
		}
		else
			piv_index = i;
	}
	form->dot_alloc--;
	//free(fcoords[piv_index]->forms);
	//free(fcoords[piv_index]->dot_type);
	//free(fcoords[piv_index]->coord);
	//free(fcoords[piv_index]);
	//free(fcoords);
	//form->fcoords = fcoords_new;
	free(coords[piv_index]);
	free(coords);
	form->coords = coords_new;

	return form;
}


form_child_t *form_remove_from(form_child_t *curr, form_child_t *form_head)
{
	// remove form from formlist
	form_child_t *last;
	if (!curr)
		return NULL;
	if (!form_head || curr == form_head)
	{
		return curr->next;
	}
	last = form_head;
	while (last && last->next != curr)
		last = last->next;
	if (!last)
	{
		printf("WARNING: Form to be removed not in scope!\n");
		return form_head;
	}
	last->next = curr->next;
	return form_head;
}


bool form_is_selected(form_child_t *form, select_t *select)
{
	int index;
	select_head(select);
	//while (select)
	while ((index = select_get_form_advance(select)) != -1)
	{
		if (form_container_get_form_child(pshow->topforms, index) == form)
		{
			select_head(select);
			return true;
		}
	}
	select_head(select);
	return false;
}



bool form_endpoint_contains_coords(form_child_t *form, double x, double y)
{
	int formindex;
	int dot_alloc;

	if (!form)
		return false;

	dot_alloc = form->dot_alloc;
	form_coord_attr_t form_attr;
	if (form_contains_coords(form, &form_attr, x, y))
	{
		formindex = form_attr.formindex;
		return (formindex == 0 || formindex == (dot_alloc - 1));
	}
	return false;
}


/*
bool form_endpoint_contains_coords(form_child_t *form, double x, double y)
{
	if(!form)
		return NULL;
	if (fieldrel_check_dots_within_range(form->endpoints[0][0], form->endpoints[0][1], x, y))
		return true;
	else if (fieldrel_check_dots_within_range(form->endpoints[1][0], form->endpoints[1][1], x, y))
		return true;
	return false;
}



bool form_endpoint_hole_contains_coords(form_child_t *form, double x, double y)
{
	double coordx, coordy;
	form_coord_t **fcoords;
	int dot_num;
	if (!form)
		return NULL;
	dot_num = form->dot_num;
	fcoords = form->fcoords;
	coordx = form->endpoints[0][0];
	coordy = form->endpoints[0][1];
	if (fcoords[0]->dot == -1 && fieldrel_check_dots_within_range(coordx, coordy, x, y))
		return true;
	coordx = form->endpoints[dot_num-1][0];
	coordy = form->endpoints[dot_num-1][1];
	if (fcoords[dot_num-1]->dot == -1 && fieldrel_check_dots_within_range(coordx, coordy, x, y))
		return true;
	return false;
}


bool form_contains_coords(form_child_t *form, double x, double y)
{
	int i;
	int index;
	double coordx, coordy;
	form_coord_t **fcoords;
	if (!form)
		return false;
	index = form->dot_num;
	fcoords = form->fcoords;
	x -= form->endpoints[0][0];
	y -= form->endpoints[0][1];
	for (i=0; i<index; i++)
	{
		coords_retrieve(fcoords[i]->coord, &coordx, &coordy);
		//coord = fcoords[i]->coord;
		//if (fieldrel_check_dots_within_range(coord->x, coord->y, x, y))
		if (fieldrel_check_dots_within_range(coordx, coordy, x, y))
			return true;
	}
	return false;
}



bool form_hole_contains_coords(form_child_t *form, double x, double y)
{
	int i;
	double coordx, coordy;
	int dot_num;
	form_coord_t **fcoords;

	fcoords = form->fcoords;
	x = x - form->endpoints[0][0];
	y = y - form->endpoints[0][1];
	dot_num = form->dot_num;
	for(i=0; i<dot_num; i++)
	{
		coords_retrieve(fcoords[i]->coord, &coordx, &coordy);
		//coord = fcoords[i]->coord;
		//if (fcoords[i]->dot == -1 && fieldrel_check_dots_within_range(coord->x, coord->y, x, y))
		if (fcoords[i]->dot == -1 && fieldrel_check_dots_within_range(coordx, coordy, x, y))
			return true;
	}
	return false;
}
*/

bool form_contains_coords(form_child_t *form, form_coord_attr_t *formattr,
		double x, double y)
{
	int i;
	double coordx, coordy;
	int dot_alloc = form->dot_alloc;
	formattr->dot_alloc = dot_alloc;
	formattr->exists = 0;
	formattr->realindex = -1;

	for (i=0; i<dot_alloc; i++)
	{
		coords_retrieve(form->coords[i], &coordx, &coordy);
		if (fieldrel_check_dots_within_range(coordx, coordy, x, y))
		{
			formattr->formindex = i;
			formattr->realindex = form->dotgrid[i];
			formattr->exists = (form->dotgrid[i] >= 0);
			return true;
		}
	}
	return false;
}




/*
int form_find_index_with_coords(form_child_t *form, double x, double y)
{
	// return an index that matches (x, y)
	int i;
	int dot_num;
	double coordx, coordy;
	form_coord_t **fcoords;
	int dot;
	if (!form)
		return -1;
	dot_num = form->dot_num;
	fcoords = form->fcoords;
	x -= form->endpoints[0][0];
	y -= form->endpoints[0][1];
	for(i=0; i<dot_num; i++)
	{
		dot = fcoords[i]->dot;
		//coord = fcoords[i]->coord;
		//coordx = coord->x;
		//coordy = coord->y;
		coords_retrieve(fcoords[i]->coord, &coordx, &coordy);
		if (fieldrel_check_dots_within_range(coordx, coordy, x, y))
			return dot;
	}
	return -1;
}
*/



form_child_t *form_find_form_with_index(form_child_t *form, int index)
{
	int i;
	int dot_alloc;
	//form_coord_t **fcoords;
	//coord_t **coords;
	int *dotgrid;
	while (form)
	{
		dotgrid = form->dotgrid;
		//coords = form->coords;
		//fcoords = form->fcoords;
		dot_alloc = form->dot_alloc;
		for (i=0; i<dot_alloc; i++)
		{
			//if (fcoords[i]->dot == index)
			if (dotgrid[i] == index)
				return form;
		}
		form = form->next;
	}
	return NULL;
}



bool form_find_with_coords(select_t *select, int set_index, double x, double y)
{
	bool found_form = false;
	form_coord_attr_t form_attr;
	form_child_t *form = pshow->sets->setlist[set_index]->forms;
	while (form)
	{
		if (form_contains_coords(form, &form_attr, x, y))
		{
			select_add_form(select, form->parent->index);
			found_form = true;
		}
		form = form->next;
	}
	return found_form;
}




bool form_find_with_hole(select_t *select, int set_index, double x, double y)
{
	// find a form with a hole at coords
	bool found_form = false;
	form_coord_attr_t form_attr;
	form_child_t *form = pshow->sets->setlist[set_index]->forms;
	while (form)
	{
		if (form_contains_coords(form, &form_attr, x, y)
				&& form_attr.exists == 0)
		{
			select_add_form(select, form->parent->index);
			found_form = true;
		}
		form = form->next;
	}
	return found_form;
}



/*
form_child_t *form_find_with_endpoint(form_child_t *form, double x, double y)
{
	// find a form with an endpoint that matches coords
	while (form)
	{
		if (form_endpoint_contains_coords(form, x, y))
			return form;
		form = form->next;
	}
	return NULL;
}



form_child_t *form_find_with_endpoint_hole(form_child_t *form, double x, double y)
{
	// find a form with an endpoint hole that matches the coords
	while (form)
	{
		if (form_endpoint_hole_contains_coords(form, x, y))
			return form;
		form = form->next;
	}
	return NULL;
}
*/


form_child_t *form_find_with_attr(select_t *select, double x, double y, bool (*fptr)(form_child_t*,double,double))
{
	// use comparison fptr to find a selected form with certain attributes
	int index;
	form_child_t *form;
	//while(select)
	select_head(select);
	while((index = select_get_form_advance(select)) != -1)
	{
		/*
		//if (!select->form)
		if (!select_get_form(select))
		{
			//select = select->next;
			select = select_get_next(select);
			continue;
		}
		*/
		//form = select->form;
		//form = select_get_form(select);
		form = form_container_get_form_child(pshow->topforms, index);
		if (fptr(form, x, y))
			return form;
		//select = select_get_next(select);
	}
	select_head(select);
	return NULL;
}


form_child_t *form_find_selected_with_endpoint(select_t *select, double x, double y)
{
	// find and return a selected form
	// that contains an endpoint (x, y)
	int index;
	form_child_t *form;
	form_coord_attr_t form_attr;
	int formindex;
	int dot_alloc;

	//return form_find_with_attr(select, x, y, &form_endpoint_contains_coords);
	select_head(select);
	while ((index = select_get_form_advance(select)) != -1)
	{
		form = form_container_get_form_child(pshow->topforms, index);
		dot_alloc = form->dot_alloc;
		if (form_contains_coords(form, &form_attr, x, y))
		{
			formindex = form_attr.formindex;
			if (formindex == 0 
					|| formindex == (dot_alloc - 1))
				return form;
		}
	}
	return NULL;
}


form_child_t *form_find_selected_with_endpoint_hole(select_t *select, double x, double y)
{
	// find and return a selected form
	// that contains an endpoint hole (x, y)
	//return form_find_with_attr(select, x, y, &form_endpoint_hole_contains_coords); 
	int index;
	form_child_t *form;
	form_coord_attr_t form_attr;
	int formindex;
	int dot_alloc;

	select_head(select);
	while ((index = select_get_form_advance(select)) != -1)
	{
		form = form_container_get_form_child(pshow->topforms, index);
		dot_alloc = form->dot_alloc;
		if (form_contains_coords(form, &form_attr, x, y))
		{
			formindex = form_attr.formindex;
			if ((formindex == 0 
					|| formindex == (dot_alloc - 1))
					&& form_attr.exists == 0)
				return form;
		}
	}
	return NULL;
}




form_child_t *form_add_index_to_hole_with_coords(form_child_t *form, int index, double x, double y)
{
	int dot_alloc;
	form_coord_attr_t form_attr;
	//form_coord_t **fcoords;
	coord_t **fcoords;
	double coordx, coordy;
	coord_t **coords = pshow->sets->currset->coords;
	int formindex;
	if (!form)
		return NULL;
	dot_alloc = form->dot_alloc;
	fcoords = form->coords;
	//x -= form->endpoints[0][0];
	//y -= form->endpoints[0][1];
	if (form_contains_coords(form, &form_attr, x, y)
			&& form_attr.exists == 0)
	{
		formindex = form_attr.formindex;
		coords_retrieve(fcoords[formindex], 
				&coordx, &coordy);
		coords_set(coords[formindex], coordx, coordy);
		if (formindex == 0 || formindex == (dot_alloc - 1))
		{
			// endpoint
			coords_set_managed(coords[formindex], 2);
		}
		else
		{
			// managed
			coords_set_managed(coords[formindex], 1);
		}
		form_update_line(form);
	}


	/*
	for(i=0; i<dot_alloc; i++)
	{
		//dot = fcoords[i]->dot;
		dot = form->dotgrid[i];
		coords_retrieve(fcoords[i], &coordx, &coordy);
		if (dot == -1 && fieldrel_check_dots_within_range(coordx, coordy, x, y))
		{
			//fcoords[i]->dot = index;
			form->dotgrid[i] = index;
			if (i == 0 || i == dot_alloc - 1)
			{
				//if (coords_check_managed_by_index(index) == 2)
				if (coords_check_managed(coords[index]) == 2)
				{
					// pairing endpoints together
					coords_retrieve_midset(pstate.setnum, index, &coordx, &coordy);
					form_set_endpoint(form, form->endpoints[0][0], form->endpoints[0][1], coordx, coordy);
				}
				else
				{
					//coords_set_managed_by_index(index, 2);
					coords_set_managed(coords[index], 2);
				}
			}
			else
			{
				//coords_set_managed_by_index(index, 1);
				coords_set_managed(coords[index], 1);
			}
			form_update_line(form);
			return form;
		}
	}
	*/
	return NULL;
}





form_child_t *form_build_line(form_parent_t *form_parent, select_t *select)
{
	// build a line
	
	int i;
	int index = 0;
	int size = 0;
	//form_coord_t **fcoords;
	//coord_t **coords;
	form_child_t *form;
	coord_t **coords = pshow->sets->currset->coords;

	// get number of selects
	// for allocation purposes
	select_head(select);
	//while (select)
	while ((index = select_get_dot_advance(select)) != -1)
	{
		size++;
	}
	select_head(select);
	form = form_child_construct_with_size(form_parent, size);
	form->type = 1;

	i = 0;
	while ((index = select_get_dot_advance(select)) != -1)
	{
		form->dotgrid[i] = index;
		if (i != 0 && i != size - 1)
		{
			//coords_set_managed_by_index(index, 0x1);
			coords_set_managed(coords[index], 0x1);
		}
		else
		{
			//coords_set_managed_by_index(index, 0x2);
			coords_set_managed(coords[index], 0x2);
			//coords_retrieve(coords[index], &coordx, &coordy);
			//coords_set(form->coords[i], coordx, coordy);
		}
		i++;
	}
	form_update_line(form);
	return form;
}



bool form_contained_in_rectangle(form_child_t *form, double x1, double y1, double x2, double y2)
{
	int i;
	int index;
	double coordx, coordy;
	//form_coord_t **fcoords;
	coord_t **coords;

	index = form->dot_alloc;
	//fcoords = form->fcoords;
	coords = form->coords;
	for (i=0; i<index; i++)
	{
		coords_retrieve(coords[i], &coordx, &coordy);
		if (field_check_dot_in_rectangle(coordx, coordy, x1, y1, x2, y2))
			return true;
	}
	return false;
}




int form_update_line(form_child_t *form)
{
	int i;
	//int index;
	double slopex, slopey;
	double endpointx0, endpointy0;
	double endpointx1, endpointy1;
	double coordx, coordy;
	//form_coord_t *fcoord;
	int dot_alloc = form->dot_alloc;
	coord_t **fcoords = form->coords;
	coord_t **coords = pshow->sets->currset->coords;
	int *dotgrid = form->dotgrid;

	//index = form->dot_num;
	coords_retrieve(fcoords[0], &endpointx0, &endpointy0);
	coords_retrieve(fcoords[dot_alloc-1], &endpointx1, &endpointy1);
	//slopex = (form->endpoints[1][0] - form->endpoints[0][0]) / (index - 1);
	//slopey = (form->endpoints[1][1] - form->endpoints[0][1]) / (index - 1);
	//endpointx = form->endpoints[0][0];
	//endpointy = form->endpoints[0][1];
	slopex = (endpointx1 - endpointx0) / (dot_alloc - 1);
	slopey = (endpointy1 - endpointy0) / (dot_alloc - 1);

	for(i=0; i<dot_alloc; i++)
	{
		//fcoord = form->fcoords[i];
		coordx = i*slopex + endpointx0;
		coordy = i*slopey + endpointy0;
		coords_set(fcoords[i], coordx, coordy);
		//fcoord->coord->x = i*slopex;
		//fcoord->coord->y = i*slopey;
		if (dotgrid[i] != -1)
		{
			coords_set(coords[dotgrid[i]], coordx, coordy);
		}
	}
	return 0;
}


int form_set_endpoint(form_child_t *form, double x1, double y1, 
		double x2, double y2, double granularity)
{
	//int i;
	//double *endpoint;
	coord_t **fcoords = form->coords;
	form_coord_attr_t form_attr;
	//int *dotgrid = form->dotgrid;
	int dot_alloc = form->dot_alloc;
	int formindex;

	if (granularity == 1)
	{
		x2 = round(x2);
		y2 = round(y2);
	}

	if (form_contains_coords(form, &form_attr, x1, y1))
	{
		formindex = form_attr.formindex;
		if (formindex == 0 || formindex == (dot_alloc-1))
		{
			coords_set(fcoords[formindex], x2, y2);
		}
	}
	form_update_line(form);
	return 0;
	/*

			
	fcoords = form->coords;
	for (i=0; i<2; i++)
	{
		//endpoint = form->endpoints[i];
		if (fieldrel_check_dots_within_range(endpoint[0], endpoint[1], x1, y1))
		{
			endpoint[0] = x2;
			endpoint[1] = y2;
			break;
		}
	}
	form_update_line(form);
	return 0;
	*/
}



/*
int form_set_endpoint_grid(form_child_t *form, double x1, double y1, double x2, double y2)
{
	int i;
	double *endpoint;
	if (!form)
		return -1;
	for (i=0; i<2; i++)
	{
		endpoint = form->endpoints[i];
		if (fieldrel_check_dots_within_range(endpoint[0], endpoint[1], x1, y1))
		{
			endpoint[0] = round(x2);
			endpoint[1] = round(y2);
			break;
		}
	}
	form_update_line(form);
	return 0;
}
*/




int form_move_endpoint(form_child_t *form, double x1, double y1, double x2, double y2, double granularity)
{
	//int i;
	int dot_alloc = form->dot_alloc;
	//int *dotgrid = form->dotgrid;
	form_coord_attr_t form_attr;
	int formindex;
	double coordx, coordy;
	coord_t **fcoords = form->coords;
	//double *endpoint;
	
	x2 -= x1;
	y2 -= y1;
	if (granularity == 1)
	{
		x2 = round(x2);
		y2 = round(y2);
	}
	if (form_contains_coords(form, &form_attr, x1, y1))
	{
		formindex = form_attr.formindex;
		if (formindex == 0 || formindex == dot_alloc-1)
		{
			coords_retrieve(fcoords[formindex],
					&coordx, &coordy);
			coordx += x2;
			coordy += y2;
			coords_set(fcoords[formindex], coordx, coordy);
			form_update_line(form);
		}
		/*
		for (i=0; i<2; i++)
		{
			endpoint = form->endpoints[i];
			if (fieldrel_check_dots_within_range(endpoint[0], 
						endpoint[1], x1, y1))
			{
				endpoint[0] += (x2-x1);
				endpoint[1] += (y2-y1);
				form_update_line(form);
				return 0;
			}
		}
		*/
		//form_set_endpoint(form, x1, y1, x2, y2);
		//return 0;
	}
	return 0;
}


/*
int form_move_endpoint_grid(form_child_t *form, double x1, double y1, double x2, double y2)
{
	while (form)
	{
		if (form_endpoint_contains_coords(form, x1, y1))
		{
			form_set_endpoint_grid(form, x1, y1, x2, y2);
			return 0;
		}
		form = form->next;
	}
	return -1;
}
*/


int form_movexy(form_child_t *form, double xoff, double yoff)
{
	int i, dot_alloc;
	//int dot;
	//double endpointx, endpointy;
	double coordx, coordy;
	coord_t **fcoords = form->coords;

	if (!form)
		return -1;
	//form->endpoints[0][0] += xoff;
	//form->endpoints[1][0] += xoff;
	//form->endpoints[0][1] += yoff;
	//form->endpoints[1][1] += yoff;
	//endpointx = form->endpoints[0][0];
	//endpointy = form->endpoints[0][1];
	dot_alloc = form->dot_alloc;
	for(i=0; i<dot_alloc; i++)
	{
		coords_retrieve(fcoords[i], &coordx, &coordy);
		coordx += xoff;
		coordy += yoff;
		coords_set(fcoords[i], coordx, coordy);
	}
	form_update_line(form);
	return 0;
}


int form_unmanage_dot(form_child_t *form, int index)
{
	int i;
	int dot_alloc = form->dot_alloc;
	int *dotgrid = form->dotgrid;
	//form_coord_t **fcoords;
	

	for(i=0; i<dot_alloc; i++)
	{
		if (dotgrid[i] == index)
		{
			// found dot
			//fcoords[i]->dot = -1;
			form->dotgrid[i] = -1;
			coords_set_managed(pshow->sets->currset->coords[index], 0);
			break;
		}
	}
	return 0;
}


int form_get_dimensions(form_child_t *form, double *xmin, double *ymin, double *xmax, double *ymax)
{
	//double endx0 = form->endpoints[0][0], endx1 = form->endpoints[1][0], 
	//       endy0 = form->endpoints[0][1], endy1 = form->endpoints[1][1];
	double endx0, endx1, endy0, endy1;
	int dot_alloc = form->dot_alloc;
	coord_t **fcoords = form->coords;

	coords_retrieve(fcoords[0], &endx0, &endy0);
	coords_retrieve(fcoords[dot_alloc-1], &endx1, &endy1);
	
	*xmin = (endx0 < endx1 ? endx0 : endx1);
	*xmax = (endx0 > endx1 ? endx0 : endx1);
	*ymin = (endy0 < endy1 ? endy0 : endy1);
	*ymax = (endy0 > endy1 ? endy0 : endy1);

	return 0;
}


select_t *form_get_contained_dots(form_child_t *form)
{
	int i;
	int dot_alloc = form->dot_alloc;
	int *dotgrid = form->dotgrid;
	select_t *select = select_create();

	for(i=0; i<dot_alloc; i++)
	{
		if (dotgrid[i] != -1)
			select_add_dot(select, dotgrid[i]);
		//dot = form->fcoords[i]->dot;
		//if (dot != -1)
		//	select_add_dot(select, dot);
	}
	return select;
}


void form_add_to_set(form_child_t *form, set_t *currset)
{
	form_child_t *setform = currset->forms;
	form_child_t *curr = NULL;
	if (!setform)
	{
		currset->forms = form;
		return;
	}
	while (setform)
	{
		if (setform == form)
			return;
		curr = setform;
		setform = setform->next;
	}
	curr->next = form;
	return;
}

void form_add_to_current_set(form_child_t *form)
{
	form_add_to_set(form, pshow->sets->currset);
	return;
}


coord_t **form_get_coords(form_child_t *form)
{
	int i;
	int dot_alloc = form->dot_alloc;
	double coordx, coordy;
	coord_t **coords = coords_construct(dot_alloc);
	coord_t **fcoords = form->coords;

	for(i=0; i<dot_alloc; i++)
	{
		coords_retrieve(fcoords[i], &coordx, &coordy);
		coords_set(coords[i], coordx, coordy);
		if (form->dotgrid[i] == -1)
		{
			//coords[i]->type = 0;
			coords_set_managed(coords[i], 0);
		}
		else
		{
			//coords[i]->type = 1;
			coords_set_managed(coords[i], 1);
		}
	}
	return coords;
}	



coord_t *form_get_coord_near(form_child_t *form, double x, double y)
{
	int i;
	int min_index = -1;
	double min_distance = -1;
	double distance;
	int dot_alloc = form->dot_alloc;
	coord_t *coord = NULL;
	coord_t **fcoords = form->coords;
	//form_coord_t **fcoords;
	double coordx, coordy;

	for(i=0; i<dot_alloc; i++)
	{
		//coord = fcoords[i]->coord;
		coords_retrieve(fcoords[i], &coordx, &coordy);
		//if (fieldrel_check_dots_within_range(coord->x, coord->y, x, y))
		if (fieldrel_check_dots_within_range(coordx, coordy, x, y))
		{
			distance = pow(coordx,2) + pow(coordy,2);
			if (distance < min_distance || min_distance == -1)
			{
				min_index = i;
				min_distance = distance;
			}
		}
	}
	if (min_index != -1)
	{
		coords_retrieve(fcoords[min_index], &coordx, &coordy);
		coord = coord_construct_with_data(coordx, coordy);
	}
	return coord;
}



void form_scale_from_center(form_child_t *form, double s_step)
{
	int i;
	double cx, cy;
	int dot_alloc = form->dot_alloc;
	coord_t **fcoords = form->coords;
	double coordx, coordy;

	coords_retrieve(pstate.center, &cx, &cy);

	for (i=0; i<dot_alloc; i++)
	{
		coords_retrieve(fcoords[i], &coordx, &coordy);
		coords_scale_coords_from_center(s_step, &coordx, &coordy, cx, cy);
		coords_set(fcoords[i], coordx, coordy);
	}
	/*
	for(i=0; i<2; i++)
		coords_scale_coords_from_center(s_step, &form->endpoints[i][0], &form->endpoints[i][1], cx, cy);
		*/
	form_update_line(form);
	return;
}


void form_rotate_around_center(form_child_t *form, double s_step)
{
	int i;
	double cx, cy;
	double distx, disty;
	double angle, hypo;
	double endx, endy;
	int dot_alloc = form->dot_alloc;
	coord_t **fcoords = form->coords;

	coords_retrieve(pstate.center, &cx, &cy);
	for(i=0; i<2; i++)
	{
		coords_retrieve(fcoords[i*(dot_alloc-1)], &endx, &endy);
		distx = cx - endx;
		disty = cy - endy;
		if (distx != 0 || disty != 0)
		{
			angle = atan(disty / distx);
			hypo = pow(distx, 2) + pow(disty, 2);
			hypo = sqrt(hypo);
			if (distx < 0)
				angle = angle + M_PI;
			angle = angle + s_step;
			distx = hypo*cos(angle);
			disty = hypo*sin(angle);
			//form->endpoints[i][0] = cx - distx;
			//form->endpoints[i][1] = cy - disty;
			coords_set(fcoords[i*(dot_alloc-1)], cx - distx,
					cy - disty);
		}
	}
	form_update_line(form);
	return;
}



select_t *form_flatten(form_child_t *form, select_t *select)
{
	int i;
	int dot_alloc = form->dot_alloc;
	int *dotgrid = form->dotgrid;
	//form_coord_t **fcoords;
	
	
	select_head(select);
	select_remove_form(select, form->parent->index);
	//fcoords = form->fcoords;
	//dot_num = form->dot_num;
	for (i=0; i<dot_alloc; i++)
		if (dotgrid[i] != -1)
			select_add_dot(select, dotgrid[i]);
	select_head(select);
	return select;
}


form_child_t *form_copy(form_child_t *form)
{
	int i;
	int size;
	int index;
	//int type;
	form_child_t *newform;
	form_parent_t *parent = form->parent;
	int dot_alloc = form->dot_alloc;
	double coordx, coordy;
	int *dotgrid = form->dotgrid;

	//dot_num = form->dot_num;
	newform = form_child_construct_with_size(parent, dot_alloc);
	newform->type = form->type;
	size = strlen(form->name)+1;
	newform->name = (char*)malloc(size*sizeof(char));
	strncpy(newform->name, form->name, size);
	for(i=0; i<dot_alloc; i++)
	{
		//index = form->fcoords[i]->dot;
		index = dotgrid[i];
		newform->dotgrid[i] = index;
		//memcpy(newform->fcoords[i]->coord, form->fcoords[i]->coord, sizeof(coord_t));
		//coords_retrieve(form->fcoords[i]->coord, &coordx, &coordy);
		//coords_set(newform->fcoords[i]->coord, coordx, coordy);
	}
	for(i=0; i<2; i++)
	{
		coords_retrieve(form->coords[i*(dot_alloc-1)], &coordx, &coordy);
		//newform->endpoints[i][0] = form->endpoints[i][0];
		//newform->endpoints[i][1] = form->endpoints[i][1];
		coords_set(newform->coords[i*(dot_alloc-1)], coordx, coordy);
	}
	form_update_line(newform);
	return newform;
}



form_parent_t *form_parent_construct(void)
{
	int i;
	form_parent_t *last;

	last = (form_parent_t*)malloc(sizeof(form_parent_t));
	last->forms = (form_child_t**)malloc(pshow->sets->size_alloc*sizeof(form_child_t*));
	for (i=0; i<pshow->sets->size_alloc; i++)
		last->forms[i] = NULL;
	return last;
}



form_parent_t *form_parent_construct_with_form(form_child_t *form, int index)
{
	form_parent_t *last;
	last = form_parent_construct();
	
	last->forms[0] = form;

	return last;
}


form_parent_t *form_parent_realloc(form_parent_t *form, int size, int size_realloc)
{
	int i;
	int newsize = (size < size_realloc ? size : size_realloc);
	form_parent_t *newform_parent;

	newform_parent = (form_parent_t*)malloc(size_realloc*sizeof(form_parent_t));
	for (i=0; i<newsize; i++)
		newform_parent->forms[i] = form->forms[i];
	free(form);
	return newform_parent;
}





form_parent_t *form_parent_destruct(form_parent_t *last)
{
	free(last->forms);
	//LIST_REMOVE(last, formlist_entries);
	free(last);
	return NULL;
}

void form_child_set_name(form_child_t *form, char *name)
{
	int i;
	int size = strnlen(name, 30)+2;
	char *newname = (char*)malloc(size*sizeof(char));
	free(form->name);
	form->name = newname;
	for (i=0; i<size && name[i] != '\0'; i++)
		form->name[i] = name[i];
	form->name[i] = '\0';
	return;
}


char *form_child_get_name(form_child_t *form)
{
	return form->name;
}


bool form_parent_contiguous(form_parent_t *fparent, int set_num)
{
	int size;
	size = pshow->sets->size;
	if (set_num < size-1 && fparent->forms[set_num] && fparent->forms[set_num+1])
		return true;
	return false;
}



form_container_t *form_parent_add_form(form_container_t *head, form_child_t *form, int index)
{
	form_parent_t *last;

	if (!head)
	{
		head = form_container_construct();
		//head = (form_list_t*)malloc(sizeof(form_list_t));
		//LIST_INIT(head);
	}
	
	last = form_parent_construct_with_form(form, index);
	form->parent = last;
	form_container_insert_head(head, last);

	//LIST_INSERT_HEAD(head, last, formlist_entries);

	return head;
}



int form_parent_add_set(form_container_t *head, form_parent_t *last, int index)
{
	form_child_t **forms;
	int excode;
	if (!last)
		return -1;
	forms = last->forms;
	if (index > 0)
		forms[index] = form_copy(forms[index-1]);
	else
		forms[index] = form_child_construct(last);
	return 0;
	return excode;
}



int form_parent_copy_to(form_parent_t *last, int index, int transpose)
{
	form_child_t **forms;
	if (!last)
		return -1;
	forms = last->forms;
	forms[transpose] = form_copy(forms[index]);
	return 0;
}



int form_parent_remove_set(form_container_t *head, form_parent_t *last, int index)
{
	form_child_t **forms;

	if (!last)
		return -1;
	forms = last->forms;
	if (forms[index])
	{
		form_child_destruct(forms[index]);
		forms[index] = NULL;
	}
	return 0;
}





form_child_t *form_parent_find_form_at_index(form_parent_t *last, int index)
{
	return last->forms[index];
}



bool form_parent_mapped_at_set(form_parent_t *last, int setnum)
{
	return (last->forms[setnum] != NULL);
}


form_container_t *form_container_construct(void)
{
	form_container_t *fcont;

	fcont = (form_container_t*)malloc(sizeof(form_container_t));
	fcont = form_container_realloc(fcont, 5);
	return fcont;
}


form_container_t *form_container_destruct(form_container_t *fcont)
{
	free(fcont->forms);
	free(fcont);
	return NULL;
}


form_container_t *form_container_realloc(form_container_t *fcont, size_t size_alloc)
{
	int i;
	form_parent_t **forms;
	size_t size;

	forms = (form_parent_t**)malloc(size_alloc*sizeof(form_parent_t*));
	size = fcont->size;
	for (i = 0; i < size; i++)
	{
		forms[i] = fcont->forms[i];
		if (forms[i])
			forms[i]->index = i;
	}
	free(fcont->forms);
	fcont->forms = forms;
	fcont->size_alloc = size_alloc;
	return fcont;
}


void form_container_resize_parents(form_container_t *fcont, size_t oldsize, size_t newsize)
{
	int i;
	int parentnum = fcont->size;
	form_parent_t **parent = fcont->forms, **newparent;
	newparent = (form_parent_t**)malloc(newsize*sizeof(form_parent_t*));
	for (i=0; i < parentnum; i++)
		newparent[i] = form_parent_realloc(parent[i], oldsize, newsize);
	return;
}


void form_container_move_children_including(form_container_t *fcont, int setnum)
{
	int i, j;
	form_parent_t *parent;
	for(i=0; i<fcont->size; i++)
	{
		parent = fcont->forms[i];
		for (j=pshow->sets->size; j>=setnum; j--)
			parent->forms[i+1] = parent->forms[i];
	}
	return;
}




form_container_t *form_container_insert_head(form_container_t *fcont, form_parent_t *last)
{
	int i;
	int size;
	form_parent_t **forms;

	forms = fcont->forms;
	size = fcont->size;

	if (size+1 >= fcont->size_alloc)
		fcont = form_container_realloc(fcont, fcont->size_alloc+5);
	for (i=size-1; i>=0; i--)
	{
		forms[i+1] = forms[i];
		if (forms[i+1])
			forms[i+1]->index = i+1;
	}
	last->index = 0;
	forms[0] = last;
	fcont->size++;
	return fcont;
}

form_parent_t *form_container_get_form_parent(form_container_t *fcont, int index)
{
	return fcont->forms[index];
}

form_child_t *form_container_get_form_child_at_set(form_container_t *fcont, int index, int setnum)
{
	return form_container_get_form_parent(fcont, index)->forms[setnum];
}

form_child_t *form_container_get_form_child(form_container_t *fcont, int index)
{
	return form_container_get_form_parent(fcont, index)->forms[pstate.setnum];
}
