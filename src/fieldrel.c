#include "drillwriter.h"


bool fieldrel_check_dots_within_range(double x1, double y1, double x2, double y2)
{
	double dist_threshold = 9;
	double distance;
	double dx, dy;

	dx = x1 - x2;
	dy = y1 - y2;

	distance = pow(dx, 2) + pow(dy, 2);
	if (distance < dist_threshold)
	       return true;
	else
		return false;
}

// TODO: put side-to-side into one function like front-to-back
int fieldrel_check_is_inside_yard(double *x, double *y, int *field_side)
{
	// check to see if a dot is inside or outside
	// a yardline
	double ssrel;
	double coordx;
	coordx = *x;
	// get side-to-side
	ssrel = coordx / 8;
	ssrel = 8*(ssrel - (int)ssrel);
	if (ssrel > 4)
	{
		// to the right of yardline
		if (coordx > 80)
		{
			// inside side 2
			*field_side = 2;
			return 1;
		}
		else
		{
			// outside side 1
			*field_side = 1;
			return -1;
		}
	}
	else if (ssrel < 4)
	{
		// to the left of yardline
		if (coordx > 80)
		{
			// outside side 2
			*field_side = 2;
			return -1;
		}
		else
		{
			// inside side 1
			*field_side = 1;
			return 1;
		}
	}
	else if (ssrel == 4)
	{
		if (coordx > 80)
		{
			// split side 2 yardline
			*field_side = 2;
			return 1;
		}
		else
		{
			// on side 1 yardline
			*field_side = 1;
			return 1;
		}
	}
	// on the yardline
	return 0;
}

//int getYardline(double *x, double *y)
int fieldrel_get_yardline_number(double *x, double *y)
{
	// get yardline number
	int yardline;
	double coordx;
	double inCheck;
	coordx = *x;
	yardline = (coordx+4)/8;
	yardline = abs(yardline - 10);
	yardline = 5 * abs(10 - yardline);
	inCheck = (coordx+4)/8;
	if (coordx < 80 && inCheck == floorf(inCheck))
		yardline = yardline - 5;
	return yardline;
}


//double getSidetoSide(double *x, double *y)
double fieldrel_get_side_to_side(double *x, double *y)
{
	// get the side-to-side relation
	double ssrel;
	double coordx;

	coordx = *x;
	ssrel = coordx / 8;
	ssrel = 8*(ssrel - (int)ssrel);
	ssrel = (int)(ssrel*4);
	ssrel = ssrel / 4;
	if (ssrel > 4)
		ssrel = 8 - ssrel;
	return ssrel;
}

//double getFronttoBack(double *x, double *y, char **inorout_r, char **frontback_r, char **hashorside_r)
double fieldrel_get_front_to_back(double *x, double *y, char **inorout_r, char **frontback_r, char **hashorside_r)
{
	// check to see if a dot is inside or outside
	// a hash/sideline, and whether it's on the front or back
	int back_side = 0;
	int back_hash = 32;
	int front_hash = 53;
	int front_side = 85;
	char *inorout;
	char *frontback;
	char *hashorside;
	double fbrel;
	// back hash to back sideline
	int bhbs = (back_hash - back_side) / 2 + back_side;
	// front hash to back hash
	int bhfh = (front_hash - back_hash) / 2 + back_hash;	
	// front sideline to front hash
	int fhfs = (front_side - front_hash) / 2 + front_hash;
	double coordy = *y;
	// Get front-to-back
	if (coordy == back_side)
	{
		// coord is on back sideline
		fbrel = 0;
		inorout = strdup("on");
		frontback = strdup("back");
		hashorside = strdup("sideline");
	}
	else if (coordy < bhbs)
	{
		// coord is close to back sideline
		fbrel = coordy;
		inorout = strdup("inside");
		frontback = strdup("back");
		hashorside = strdup("sideline");
	}
	else if (coordy < back_hash)
	{
		// coord is behind back hash
		fbrel = back_hash - coordy;
		inorout = strdup("outside");
		frontback = strdup("back");
		hashorside = strdup("hash");
	}
	else if (coordy == back_hash)
	{
		// coord is on back hash
		fbrel = 0;
		inorout = strdup("on");
		frontback = strdup("back");
		hashorside = strdup("hash");
	}
	else if (coordy < bhfh)
	{
		// coord is close, but in front of, back hash
		fbrel = coordy - back_hash;
		inorout = strdup("inside");
		frontback = strdup("back");
		hashorside = strdup("hash");
	}
	else if (coordy < front_hash)
	{
		// coord is close to, but behind, front hash
		fbrel = front_hash - coordy;
		inorout = strdup("inside");
		frontback = strdup("front");
		hashorside = strdup("hash");
	}
	else if (coordy == front_hash)
	{
		// coord is on front hash
		fbrel = 0;
		inorout = strdup("on");
		frontback = strdup("front");
		hashorside = strdup("hash");
	}
	else if (coordy < fhfs)
	{
		// coord is close to, but in front of, front hash
		fbrel = coordy - front_hash;
		inorout = strdup("outside");
		frontback = strdup("front");
		hashorside = strdup("hash");
	}
	else if (coordy < front_side)
	{
		// coord is close to, but behind, front sideline
		fbrel = front_side - coordy;
		inorout = strdup("inside");
		frontback = strdup("front");
		hashorside = strdup("sideline");
	}
	else
	{
		// coord is on front sideline
		fbrel = 0;
		inorout = strdup("on");
		frontback = strdup("front");
		hashorside = strdup("sideline");
	}
	if (inorout == NULL || frontback == NULL || hashorside == NULL)
	{
		printf("statusbar allocation error\n");
		exit(255);
	}

	*inorout_r = inorout;
	*frontback_r = frontback;
	*hashorside_r = hashorside;
	return fbrel;
}


int fieldrel_convert_xy_to_relation(double *x, double *y, char **buffer_r)
{
	// convert event to side-side and front-back relation
	int yardline;
	int relation;
	int sidetoside;
	int size;

	double ssrel;
	double fbrel;
	char *buffer;
	char *fbbuffer;
	char *xbuff, *ybuff;
	char *sideside_relation;
	char *frontback_relation;
	double coordx, coordy;
	char *inorout;
	char *frontback;
	char *hashorside;
	// relation splits


	coordx = *x;
	coordy = *y;
	// Get side-to-side
	relation = (int)coordx % 8;
	relation = fieldrel_check_is_inside_yard(x, y, &sidetoside);
	ssrel = fieldrel_get_side_to_side(x, y);
	sideside_relation = (char*)malloc(21*sizeof(char));
	if (!sideside_relation)
		return -1;
	if (relation == 1)
	{
		// inside
		if (sidetoside == 2)
			snprintf(sideside_relation, 20, "inside side 2");
		else
			snprintf(sideside_relation, 20, "inside side 1");
	}
	else if (relation == -1)
	{
		// outside
		if (sidetoside == 2)
			snprintf(sideside_relation, 20, "outside side 2");
		else
			snprintf(sideside_relation, 20, "outside side 1");
	}
	else
	{
		// on yardline
		if (sidetoside == 2)
			snprintf(sideside_relation, 20, "on side 2");
		else
			snprintf(sideside_relation, 20, "on side 1");
	}
	if (sideside_relation == NULL)
	{
		printf("ERROR\n");
		exit(255);
	}

	// Get yardline relation
	yardline = fieldrel_get_yardline_number(x, y);

	fbrel = fieldrel_get_front_to_back(x, y, &inorout, &frontback, &hashorside);
	fbbuffer = (char*)malloc(21*sizeof(char));
	if (!fbbuffer)
		return -1;
	snprintf(fbbuffer, 20, "%.2f", fbrel);
	size = strlen(fbbuffer) + strlen(inorout) + strlen(frontback) + strlen(hashorside) + 10;
	frontback_relation = (char*)malloc(size*sizeof(char));
	if (!frontback_relation)
		return -1;
	snprintf(frontback_relation, size-1, "%s %s %s %s", fbbuffer, inorout, frontback, hashorside);
	free(fbbuffer);
	free(inorout);
	free(frontback);
	free(hashorside);

	xbuff = (char*)malloc(21*sizeof(char));
	ybuff = (char*)malloc(21*sizeof(char));
	if (!xbuff || !ybuff)
		return -1;
	snprintf(xbuff, 20, "%.2f", coordx);
	snprintf(ybuff, 20, "%.2f", coordy);
	size = strlen(xbuff) + strlen(ybuff) + strlen(sideside_relation) + strlen(frontback_relation) + 25;
	// put everything together
	buffer = (char*)malloc(size*sizeof(char));
	if (!buffer)
		return -1;
	snprintf(buffer, size-1, "(%s, %s): %.2f %s %i %s", xbuff, ybuff, ssrel, sideside_relation, yardline, frontback_relation);
	free(sideside_relation);
	free(frontback_relation);
	free(xbuff);
	free(ybuff);

	*buffer_r = buffer;
	return 0;
}



select_t *field_get_in_area(double x, double y)
{
	int i;
	int min_index = -1;
	int index;
	form_child_t *form = pshow->sets->currset->forms;
	form_child_t *min_form = NULL;
	int perfnum = pshow->perfnum;
	double coordx, coordy;
	double distance, distance_min = -1;
	coord_t *coord;
	select_t *dot_select = NULL;
	select_t *form_select = NULL;
	select_t *select = NULL;
	for(i=0; i<perfnum; i++)
	{
		if (!coords_check_managed_by_index(i))
		{
			coords_retrieve_midset(pstate.setnum, i, &coordx, &coordy);
			if (fieldrel_check_dots_within_range(x, y, coordx, coordy))
				dot_select = select_add_index(dot_select, i, false);
		}
	}
	while (form)
	{
		form = form_find_with_coords(form, x, y);
		form_select = select_add_form(form_select, form, false);
		if (form)
			form = form->next;
	}
	if (dot_select)
	{
		select = dot_select;
		distance_min = -1;
		while (select)
		{
			//coords_retrieve_midset(pstate.setnum, select->index, &coordx, &coordy);
			coords_retrieve_midset(pstate.setnum, select_get_index(select), &coordx, &coordy);
			distance = pow((coordx-x),2) + pow((coordy-y),2);
			if (distance < distance_min || distance_min == -1)
			{
				distance_min = distance;
				//min_index = select->index;
				min_index = select_get_index(select);
			}
			//select = select->next;
			select = select_get_next(select);
		}
		select = NULL;
		dot_select = select_discard(dot_select);
		dot_select = select_add_index(dot_select, min_index, false);
	}
	if (form_select)
	{
		select = form_select;
		distance_min = -1;
		while (select)
		{
			//index = form_find_index_with_coords(select->form, x, y);
			index = form_find_index_with_coords(select_get_form(select), x, y);
			if (index == -1)
			{
				//coord = form_get_coord_near(select->form, x, y);
				coord = form_get_coord_near(select_get_form(select), x, y);
				coordx = coord->x;
				coordy = coord->y;
			}
			else
				coords_retrieve_midset(pstate.setnum, index, &coordx, &coordy);
			distance = pow((coordx-x),2) + pow((coordy-y),2);
			if (distance < distance_min || distance_min == -1)
			{
				distance_min = distance;
				//min_form = select->form;
				min_form = select_get_form(select);
			}
			//select = select->next;
			select = select_get_next(select);
		}
		form_select = select_discard(form_select);
		form_select = select_add_form(form_select, min_form, false);
	}

	if (!form_select)
		return dot_select;
	else if (!dot_select)
		return form_select;
	//if (form_hole_contains_coords(form_select->form, x, y))
	if (form_hole_contains_coords(select_get_form(form_select), x, y))
		return dot_select;
	else
		return form_select;
	return NULL;
}


