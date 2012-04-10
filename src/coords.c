#include "drill.h"


int coord_construct(struct coord_proto *** coords_r, int perfs)
{
	// Build the coordinates for the set
	
	// loop var
	int i;
	// 2d coordinate array
	struct coord_proto **coords;
	// Piece of coordinate array
	struct coord_proto *ccurr;

	// Make the root pointer
	coords = (struct coord_proto**) malloc(perfs*sizeof(struct coord_proto*));
	if (!coords)
	{
		// coordinate allocation error
		printf("Error: coords mapped to 0x%x\n", coords);
		return -1;
	}

	for (i=0; i<perfs; i++)
	{
		// Make the first set of dots
		ccurr = (struct coord_proto*) malloc(sizeof(struct coord_proto));
		coords[i] = ccurr;
		ccurr->x = 0;
		ccurr->y = 0;
	}

	// link to reference
	*coords_r = coords;

	return 0;
}

int set_coord(struct coord_proto *curr, float x, float y)
{
	// set coordinates from the coord struct
	curr->x = x;
	curr->y = y;
	return 0;
}


int set_coord_valid(struct coord_proto **curr, int index, float x, float y)
{
	// set coordinates and set performer valid
	curr[index]->x = x;
	curr[index]->y = y;
	pshow->perfs[index]->valid = 1;
	return 0;
}

int retr_coord(struct coord_proto *curr, float *x, float *y)
{
	// retrieve coordinates from the coord struct
	*x = curr->x;
	*y = curr->y;

	return 0;
}



int retr_midset(struct set_proto *currset, int index, float *x_r, float *y_r)
{
	// retrieve midset coordinates from set struct
	struct set_proto *last;
	float xcurr, ycurr;
	float xnext, ynext;
	float xbias, ybias;
	int cstep;
	int countnum;

	struct set_proto *nextset;

	retr_coord(currset->coords[index], &xcurr, &ycurr);
	if (currset->next != NULL)
	{
		// not last set,
		// need to check to see if midset should be found
		nextset = currset->next;
		retr_coord(nextset->coords[index], &xnext, &ynext);
		cstep = pshow->step;
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

int movexy(float xoff, float yoff)
{
	// move selected dots by xoff and yoff
	float x, y;
	struct coord_proto **coords = pshow->currset->coords;
	struct select_proto *selects = pshow->select;
	int done = 0;
	while(selects != NULL)
	{
		retr_coord(coords[selects->index], &x, &y);
		pushPerfmv(&pshow->undobr, selects->index, x, y, done);
		x = x + xoff;
		y = y + yoff;
		set_coord(coords[selects->index], x, y);
		selects = selects->next;
	}
	// move center of selection
	x = pshow->center->x;
	y = pshow->center->y;
	x = x + xoff;
	y = y + yoff;
	pshow->center->x = x;
	pshow->center->y = y;
	return 0;
}





int align_dots(void)
{
	// align selected dots to 8:5 grid
	struct coord_proto **coords = pshow->currset->coords;
	struct select_proto *select = pshow->select;
	float x, y;
	int done = 0;
	while (select != NULL)
	{
		retr_coord(coords[select->index], &x, &y);
		pushPerfmv(&pshow->undobr, select->index, x, y, done);
		x = roundf(x);
		y = roundf(y);
		set_coord(coords[select->index], x, y);
		select = select->next;
	}
	// move center of selection
	update_sel_center();
	return 0;
}




int movexy_grid(float xoff, float yoff)
{
	// move selected dots by xoff and yoff on 1-step intervals
	float x, y;
	struct coord_proto **coords = pshow->currset->coords;
	struct select_proto *selects = pshow->select;
	while(selects != NULL)
	{
		retr_coord(coords[selects->index], &x, &y);
		x = floorf(x + xoff);
		y = floorf(y + yoff);
		set_coord(coords[selects->index], x, y);
		selects = selects->next;
	}
	// move center of selection
	x = pshow->center->x;
	y = pshow->center->y;
	x = x + xoff;
	y = y + yoff;
	pshow->center->x = x;
	pshow->center->y = y;
	return 0;
}




void scale_form(float s_step)
{
	// basic expansion or contraction of form
	// center
	float cx, cy;
	// distance
	float distx, disty;
	// sign
	int signx, signy;
	// hypotenuse
	float hypo;
	// angle
	float angle;
	// selection
	struct select_proto *last;
	// coordinates
	struct coord_proto **coords;
	struct coord_proto *coord;
	// index
	int index;

	last = pshow->select;
	coords = pshow->currset->coords;
	cx = pshow->center->x;
	cy = pshow->center->y;
	undo_tclose();
	while (last != NULL)
	{
		// get coords for selected dot
		index = last->index;
		coord = coords[index];
		pushPerfmv(&pshow->undobr, index, coord->x, coord->y, 0);
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


void rot_form(float s_step)
{
	// basic rotation around center
	// center
	float cx, cy;
	// distance
	float distx, disty;
	// sign
	int signx, signy;
	// hypotenuse
	float hypo;
	// angle
	float angle;
	// selection
	struct select_proto *last;
	// coordinates
	struct coord_proto **coords;
	struct coord_proto *coord;
	// index
	int index;
	last = pshow->select;
	coords = pshow->currset->coords;
	cx = pshow->center->x;
	cy = pshow->center->y;
	undo_tclose();
	while (last != NULL)
	{
		// get coords for selected dot
		index = last->index;
		coord = coords[index];
		distx = cx - coord->x;
		disty = cy - coord->y;
		pushPerfmv(&pshow->undobr, index, coord->x, coord->y, 0);
		if (distx != 0 || disty != 0)
		{
			signx = distx < 0;
			signy = disty < 0;
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

