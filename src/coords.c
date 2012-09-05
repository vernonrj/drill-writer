#include "drillwriter.h"


coord_t **coords_construct(int perfs)
{
	// Build the coordinates for the set
	
	// loop var
	int i, j;
	// 2d coordinate array
	coord_t **coords;
	// Piece of coordinate array

	// Make the root pointer
	coords = (coord_t**) malloc(perfs*sizeof(coord_t*));
	if (coords == NULL)
	{
		// coordinate allocation error
		return NULL;
	}

	for (i=0; i<perfs; i++)
	{
		// Make the first set of dots
		coords[i] = coord_construct();
		if (coords[i] == NULL)
		{
			for(j=0; j<i; j++)
				free(coords[j]);
			free(coords);
			return NULL;
		}
	}

	return coords;
}


coord_t *coord_construct_with_data(double x, double y)
{
	// build coord for just 1 dot
	coord_t *coord;
	coord = (coord_t*)malloc(sizeof(coord_t));
	if (coord != NULL)
	{
		coord->type = 0;
		coord->x = x;
		coord->y = y;

	}
	return coord;
}


coord_t *coord_construct(void)
{
	// wrapper coordinate constructor
	return coord_construct_with_data(0,0);
}


coord_t **coords_destruct(coord_t **coords, int perfs)
{
	// free up coord array
	int i;
	for (i=0; i<perfs; i++)
		free(coords[i]);
	free(coords);
	return coords;
}

int coords_set_coord(headset_t *dshow, int index, double x, double y)
{
	// set coordinates from the coord struct
	coord_t *coord;
	perf_t *perf;
	coord = dshow->sets->currset->coords[index];
	perf = dshow->perfs[index];
	if (perf->valid == 0 || perf->valid == -1)
		perf->valid = 1;
	coord->x = x;
	coord->y = y;
	return 0;
}


int coords_set_coord_valid(coord_t **curr, int index, double x, double y)
{
	// set coordinates and set performer valid
	coords_set_coord(pshow, index, x, y);
	return 0;
}



int coords_check_managed(coord_t *coord)
{
	if (!coord)
		return -1;
	return coord->type;
}


int coords_set_managed(coord_t *coord, int state)
{
	if (!coord)
		return -1;
	coord->type = state;
	return 0;
}


int coords_check_managed_by_index(int index)
{
	if (index >= 0)
		return coords_check_managed(pshow->sets->currset->coords[index]);
	else
		return -1;
}


int coords_set_managed_by_index(int index, int state)
{
	if (index >= 0)
		return coords_set_managed(pshow->sets->currset->coords[index], state);
	else
		return -1;
}




int coords_retrieve(coord_t *curr, double *x, double *y)
{
	// retrieve coordinates from the coord struct
	*x = curr->x;
	*y = curr->y;

	return 0;
}



int coords_retrieve_midset(set_t *currset, int index, double *x_r, double *y_r)
{
	// retrieve midset coordinates from set struct
	double xcurr, ycurr;
	double xnext, ynext;
	double xbias, ybias;
	int cstep;
	int countnum;
	set_t *nextset;

	coords_retrieve(currset->coords[index], &xcurr, &ycurr);
	if (currset->next != NULL)
	{
		// not last set,
		// need to check to see if midset should be found
		nextset = currset->next;
		coords_retrieve(nextset->coords[index], &xnext, &ynext);
		cstep = pstate.curr_step;
		if (cstep != 0)
		{
			// there is a bias that should be calculated
			countnum = nextset->counts;
			xbias = (xnext - xcurr) / countnum;
			ybias = (ynext - ycurr) / countnum;
			xbias = xbias * cstep;
			ybias = ybias * cstep;
			// find new locations based on bias
			xcurr = xcurr + xbias;
			ycurr = ycurr + ybias;
		}
	}

	// store reference
	*x_r = xcurr;
	*y_r = ycurr;

	return 0;
}



int coords_movexy(double xoff, double yoff)
{
	// move selected dots by xoff and yoff
	int i;
	int index;
	double x, y;
	group_t *group = NULL;
	coord_t **coords = pshow->sets->currset->coords;
	select_t *selects = pstate.select;
	select_t *group_selects = NULL;
	fline_t *line;
	int done = 0;
	while(selects != NULL)
	{
		if (!group_selects && selects->group)
		{
			group = selects->group;
			group_selects = selects->group->selects;
			if (group->forms)
			{
				line = group->forms->form->line;
				line->endpoints[0][0] += xoff;
				line->endpoints[0][1] += yoff;
				line->endpoints[1][0] += xoff;
				line->endpoints[1][1] += yoff;
				index = group->forms->dot_num;
				for (i=0; i<index; i++)
				{
					line->coords[i][0] += xoff;
					line->coords[i][1] += yoff;
					coords_set_coord(pshow, line->dots[i], line->coords[i][0], line->coords[i][1]);
				}
				group_selects = NULL;
				selects = selects->next;
				continue;
			}
		}
		if(group_selects)
		{
			coords_retrieve(coords[group_selects->index], &x, &y);
			pushPerfmv(&pstate.undobr, group_selects->index, x, y, done);
		}
		else
		{
			coords_retrieve(coords[selects->index], &x, &y);
			pushPerfmv(&pstate.undobr, selects->index, x, y, done);
		}

		x = x + xoff;
		y = y + yoff;
		if (group_selects)
		{
			coords_set_coord(pshow, group_selects->index, x, y);
			group_selects = group_selects->next;
			if (!group_selects)
				selects = selects->next;
		}
		else
		{
			coords_set_coord(pshow, selects->index, x, y);
			selects = selects->next;
		}
	}
	// move center of selection
	x = pstate.center->x;
	y = pstate.center->y;
	x = x + xoff;
	y = y + yoff;
	pstate.center->x = x;
	pstate.center->y = y;
	return 0;
}



int coords_align_dots_to_grid(void)
{
	// align selected dots to 8:5 grid
	coord_t **coords = pshow->sets->currset->coords;
	select_t *select = pstate.select;
	double x, y;
	int done = 0;
	while (select != NULL)
	{
		coords_retrieve(coords[select->index], &x, &y);
		pushPerfmv(&pstate.undobr, select->index, x, y, done);
		x = round(x);
		y = round(y);
		coords_set_coord(pshow, select->index, x, y);
		select = select->next;
	}
	// move center of selection
	select_update_center(pstate.select);
	return 0;
}



int coords_movexy_grid(double xoff, double yoff)
{
	// move selected dots by xoff and yoff on 1-step intervals
	double x, y;
	coord_t **coords = pshow->sets->currset->coords;
	select_t *selects = pstate.select;
	while(selects != NULL)
	{
		coords_retrieve(coords[selects->index], &x, &y);
		x = round(x + xoff);
		y = round(y + yoff);
		coords_set_coord(pshow, selects->index, x, y);
		selects = selects->next;
	}
	// move center of selection
	x = pstate.center->x;
	y = pstate.center->y;
	x = x + xoff;
	y = y + yoff;
	pstate.center->x = x;
	pstate.center->y = y;
	return 0;
}


void coords_box_scale_form_from_center(double s_step)
{
	// move furthest dot (s_step, s_step) toward/away from center
	// move other dots a ratio of
	// (dotPLD / maxPLD) * s_step * (1,1)
	// forms should retain shape with this
	
	// center
	double cx, cy;
	double x, y;
	double distx, disty;
	double maxdx, maxdy;
	// selection
	select_t *last;
	// coords
	coord_t **coords;
	coord_t *coord;
	// index
	int index;

	last = pstate.select;
	coords = pshow->sets->currset->coords;
	cx = pstate.center->x;
	cy = pstate.center->y;
	undo_tclose();
	maxdx = 0;
	maxdy = 0;
	if (last == NULL)
		return;
	while (last != NULL)
	{
		// get maxes separately
		index = last->index;
		coord = coords[index];
		coords_retrieve(coord, &x, &y);
		distx = fabs(x - cx);
		disty = fabs(y - cy);
		if (distx > maxdx)
		{
			maxdx = distx;
		}
		if (disty > maxdy)
		{
			maxdy = disty;
		}
		last = last->next;
	}
	last = pstate.select;

	// change all dots based on distance ratio
	// stored in maxdx and maxdy
	
	while (last != NULL)
	{
		// get coords for selected dot
		index = last->index;
		coord = coords[index];
		coords_retrieve(coord, &x, &y);
		pushPerfmv(&pstate.undobr, index, x, y, 0);
		if (maxdx != 0)
		{
			x = (x - cx) / maxdx * s_step;
			coord->x = coord->x + x;
		}
		if (maxdy != 0)
		{
			y = (y - cy) / maxdy * s_step;
			coord->y = coord->y + y;
		}
		// next dot
		last = last->next;
	}
	undo_tclose();
	return;

}



void coords_scale_form_from_center(double s_step)
{
	// move every dot s_step toward/away from center
	// basic expansion or contraction of form
	
	// center
	double cx, cy;
	// distance
	double distx, disty;
	// sign
	int signx, signy;
	// hypotenuse
	double hypo;
	// angle
	double angle;
	// selection
	select_t *last;
	// coordinates
	coord_t **coords;
	coord_t *coord;
	// index
	int index;

	last = pstate.select;
	coords = pshow->sets->currset->coords;
	cx = pstate.center->x;
	cy = pstate.center->y;
	undo_tclose();
	while (last != NULL)
	{
		// get coords for selected dot
		index = last->index;
		coord = coords[index];
		pushPerfmv(&pstate.undobr, index, coord->x, coord->y, 0);
		distx = cx - coord->x;
		disty = cy - coord->y;
		signx = distx < 0;
		signy = disty < 0;
		angle = atan(disty / distx);
		if (angle < 0)
			angle = -1 * angle;
		hypo = powf(distx, 2) + powf(disty, 2);
		hypo = sqrtf(hypo);
		// don't contract if too close to center
		if (s_step > 0 || hypo > -1 * s_step)
		{
			// expand or contract
			distx = (hypo+s_step)*cos(angle);
			disty = (hypo+s_step)*sin(angle);
			if (signx)
				distx = -1 * distx;
			if (signy)
				disty = -1 * disty;
			// return to cartesian space
			coord->x = cx - distx;
			coord->y = cy - disty;
			// expand next dot
		}
		last = last->next;
	}
	return;
}



void coords_rot_selected_around_center(double s_step)
{
	// basic rotation around center
	// center
	double cx, cy;
	// distance
	double distx, disty;
	// sign
	// hypotenuse
	double hypo;
	// angle
	double angle;
	// selection
	select_t *last;
	// coordinates
	coord_t **coords;
	coord_t *coord;
	// index
	int index;
	last = pstate.select;
	coords = pshow->sets->currset->coords;
	cx = pstate.center->x;
	cy = pstate.center->y;
	undo_tclose();
	while (last != NULL)
	{
		// get coords for selected dot
		index = last->index;
		coord = coords[index];
		distx = cx - coord->x;
		disty = cy - coord->y;
		pushPerfmv(&pstate.undobr, index, coord->x, coord->y, 0);
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
			coord->x = cx - distx;
			coord->y = cy - disty;
		}
		last = last->next;
	}
	return;
}
