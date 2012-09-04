#include "drill.h"


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
	
