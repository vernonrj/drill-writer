#include "drill.h"

// TODO: put side-to-side into one function like front-to-back
int isInsideYard(float *x, float *y, int *field_side)
{
	// check to see if a dot is inside or outside
	// a yardline
	float ssrel;
	float coordx, coordy;
	int relation;
	coordx = *x;
	coordy = *y;
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

int getYardline(float *x, float *y)
{
	// get yardline number
	int yardline;
	float coordx, coordy;
	float inCheck;
	coordx = *x;
	coordy = *y;
	yardline = (coordx+4)/8;
	yardline = abs(yardline - 10);
	yardline = 5 * abs(10 - yardline);
	inCheck = (coordx+4)/8;
	if (coordx < 80 && inCheck == floorf(inCheck))
		yardline = yardline - 5;
	return yardline;
}


float getSidetoSide(float *x, float *y)
{
	// get the side-to-side relation
	float ssrel;
	float coordx, coordy;

	coordx = *x;
	coordy = *y;
	ssrel = coordx / 8;
	ssrel = 8*(ssrel - (int)ssrel);
	ssrel = (int)(ssrel*4);
	ssrel = ssrel / 4;
	if (ssrel > 4)
		ssrel = 8 - ssrel;
	return ssrel;
}

float getFronttoBack(float *x, float *y, char **inorout_r, char **frontback_r, char **hashorside_r)
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
	float fbrel;
	// back hash to back sideline
	int bhbs = (back_hash - back_side) / 2 + back_side;
	// front hash to back hash
	int bhfh = (front_hash - back_hash) / 2 + back_hash;	
	// front sideline to front hash
	int fhfs = (front_side - front_hash) / 2 + front_hash;
	float coordx = *x;
	float coordy = *y;
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


void xy_to_relation(float *x, float *y, char **buffer_r)
{
	// convert event to side-side and front-back relation
	int yardline;
	int relation;
	int sidetoside;
	int size;

	float ssrel;
	float fbrel;
	char *buffer;
	char *fbbuffer;
	char *xbuff, *ybuff;
	char *sideside_relation;
	char *side;
	char *hash;
	char *frontback_relation;
	float coordx, coordy;
	char *inorout;
	char *frontback;
	char *hashorside;
	// relation splits


	coordx = *x;
	coordy = *y;
	// Get side-to-side
	relation = (int)coordx % 8;
	relation = isInsideYard(x, y, &sidetoside);
	ssrel = getSidetoSide(x, y);
	sideside_relation = (char*)malloc(21*sizeof(char));
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

	//ssrel = labs(8 - ssrel);
	// Get yardline relation
	yardline = getYardline(x, y);

	fbrel = getFronttoBack(x, y, &inorout, &frontback, &hashorside);
	fbbuffer = (char*)malloc(21*sizeof(char));
	snprintf(fbbuffer, 20, "%.2f", fbrel);
	size = strlen(fbbuffer) + strlen(inorout) + strlen(frontback) + strlen(hashorside) + 6;
	frontback_relation = (char*)malloc(size*sizeof(char));
	snprintf(frontback_relation, size-1, "%s %s %s %s", fbbuffer, inorout, frontback, hashorside);
	free(fbbuffer);
	free(inorout);
	free(frontback);
	free(hashorside);

	xbuff = (char*)malloc(20*sizeof(char));
	ybuff = (char*)malloc(20*sizeof(char));
	snprintf(xbuff, 20, "%.2f", coordx);
	snprintf(ybuff, 20, "%.2f", coordy);
	size = strlen(xbuff) + strlen(ybuff) + strlen(sideside_relation) + strlen(frontback_relation) + 20;
	// put everything together
	buffer = (char*)malloc(size*sizeof(char));
	snprintf(buffer, size-1, "(%s, %s): %.2f %s %i %s", xbuff, ybuff, ssrel, sideside_relation, yardline, frontback_relation);
	free(sideside_relation);
	free(frontback_relation);
	free(xbuff);
	free(ybuff);

	*buffer_r = buffer;
	return;
}
	
