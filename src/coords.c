#include "drill.h"


int coords_construct(coord_t *** coords_r, int perfs)
{
	// Build the coordinates for the set
	
	// loop var
	int i;
	// 2d coordinate array
	coord_t **coords;
	// Piece of coordinate array
	coord_t *ccurr;

	// Make the root pointer
	coords = (coord_t**) malloc(perfs*sizeof(coord_t*));
	if (!coords)
	{
		// coordinate allocation error
		printf("Error: coords mapped to 0x%x\n", (unsigned int)coords);
		return -1;
	}

	for (i=0; i<perfs; i++)
	{
		// Make the first set of dots
		ccurr = (coord_t*) malloc(sizeof(coord_t));
		if (!ccurr)
			return -1;
		coords[i] = ccurr;
		ccurr->x = 0;
		ccurr->y = 0;
	}

	// link to reference
	*coords_r = coords;

	return 0;
}


int coord_construct(coord_t **coord_r, double x, double y)
{
	// build coord for just 1 dot
	coord_t *coord;
	coord = (coord_t*)malloc(sizeof(coord_t));
	if (coord != NULL)
	{
		coord->type = 0;
		coord->x = x;
		coord->y = y;

		// link to reference
		*coord_r = coord;
		return 0;
	}
	else
		return -1;
	return 0;
}

int set_coord(headset_t *dshow, int index, double x, double y)
{
	// set coordinates from the coord struct
	coord_t *coord;
	perf_t *perf;
	//coord = dshow->currset->coords[index];
	coord = dshow->sets->currset->coords[index];
	perf = dshow->perfs[index];
	if (perf->valid == 0 || perf->valid == -1)
		perf->valid = 1;
	/*
	perf = dshow->perfs[index];
	if (perf->valid <= setnum)
	{
		// update valid coord
		perf->valid = setnum;
	}
	*/
	coord->x = x;
	coord->y = y;
	return 0;
}


int set_coord_valid(coord_t **curr, int index, double x, double y)
{
	// set coordinates and set performer valid
	set_coord(pshow, index, x, y);
	return 0;
	/*
	curr[index]->x = x;
	curr[index]->y = y;
	pshow->perfs[index]->valid = 1;
	return 0;
	*/
}

int retr_coord(coord_t *curr, double *x, double *y)
{
	// retrieve coordinates from the coord struct
	*x = curr->x;
	*y = curr->y;

	return 0;
}



//int retr_midset(struct set_container *sets, int index, double *x_r, double *y_r)
int retr_midset(set_t *currset, int index, double *x_r, double *y_r)
{
	// retrieve midset coordinates from set struct
	//set_t *last;
	//set_t *currset = sets->currset;
	double xcurr, ycurr;
	double xnext, ynext;
	double xbias, ybias;
	int cstep;
	int countnum;

	set_t *nextset;

	retr_coord(currset->coords[index], &xcurr, &ycurr);
	if (currset->next != NULL)
	{
		// not last set,
		// need to check to see if midset should be found
		nextset = currset->next;
		retr_coord(nextset->coords[index], &xnext, &ynext);
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

//int movexy(struct set_container *sets, select_t *selects, double xoff, double yoff)
int movexy(double xoff, double yoff)
{
	// move selected dots by xoff and yoff
	double x, y;
	coord_t **coords = pshow->sets->currset->coords;
	//coord_t **coords = sets->currset->coords;
	select_t *selects = pstate.select;
	int done = 0;
	while(selects != NULL)
	{
		retr_coord(coords[selects->index], &x, &y);
		pushPerfmv(&pstate.undobr, selects->index, x, y, done);
		x = x + xoff;
		y = y + yoff;
		set_coord(pshow, selects->index, x, y);
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



//int align_dots(struct set_container *sets, select_t *select)
int align_dots(void)
{
	// align selected dots to 8:5 grid
	//coord_t **coords = pshow->currset->coords;
	coord_t **coords = pshow->sets->currset->coords;
	select_t *select = pstate.select;
	double x, y;
	int done = 0;
	while (select != NULL)
	{
		retr_coord(coords[select->index], &x, &y);
		pushPerfmv(&pstate.undobr, select->index, x, y, done);
		x = roundf(x);
		y = roundf(y);
		set_coord(pshow, select->index, x, y);
		select = select->next;
	}
	// move center of selection
	update_sel_center();
	return 0;
}




//int movexy_grid(struct set_container *sets, select_t *selects, double xoff, double yoff)
int movexy_grid(double xoff, double yoff)
{
	// move selected dots by xoff and yoff on 1-step intervals
	double x, y;
	coord_t **coords = pshow->sets->currset->coords;
	//coord_t **coords = sets->currset->coords;
	select_t *selects = pstate.select;
	while(selects != NULL)
	{
		retr_coord(coords[selects->index], &x, &y);
		x = round(x + xoff);
		y = round(y + yoff);
		set_coord(pshow, selects->index, x, y);
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




void scale_form(double s_step)
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
	//coords = pshow->currset->coords;
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
		angle = atanf(disty / distx);
		if (angle < 0)
			angle = -1 * angle;
		hypo = powf(distx, 2) + powf(disty, 2);
		hypo = sqrtf(hypo);
		// don't contract if too close to center
		if (s_step > 0 || hypo > -1 * s_step)
		{
			// expand or contract
			distx = (hypo+s_step)*cosf(angle);
			disty = (hypo+s_step)*sinf(angle);
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


void box_scale_form(double s_step)
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
	//coords = pshow->currset->coords;
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
		retr_coord(coord, &x, &y);
		distx = abs(x - cx);
		disty = abs(y - cy);
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
		retr_coord(coord, &x, &y);
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





void rot_form(double s_step)
{
	// basic rotation around center
	// center
	double cx, cy;
	// distance
	double distx, disty;
	// sign
	//int signx, signy;
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
	//coords = pshow->currset->coords;
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
			//signx = distx < 0;
			//signy = disty < 0;
			angle = atanf(disty / distx);
			hypo = powf(distx, 2) + powf(disty, 2);
			hypo = sqrtf(hypo);
			if (distx < 0)
				angle = angle + M_PI;
			angle = angle + s_step;
			distx = hypo*cosf(angle);
			disty = hypo*sinf(angle);
			coord->x = cx - distx;
			coord->y = cy - disty;
		}
		last = last->next;
	}
	return;
}


