// Functions that manipulate performers go here
#include "drill.h"

// performer storage
int perf_construct(perf_t **perf_r)
{
	// Build storage for performer
	// give argument:
	// excode = perf_construct(&dots);
	// piece of performer array
	perf_t *perf;

	perf = (perf_t*) malloc(sizeof(perf_t));
	if (perf == NULL)
	{
		// allocation error
		return -1;
	}
	// allocate values inside struct
	perf->symbol = (char*) malloc(2 * sizeof(char));
	if (perf->symbol == NULL)
	{
		// allocation error
		return -1;
	}
	perf->symbol[0] = '\0';
	perf->valid = 0;

	// pass by reference
	*perf_r = perf;

	return 0;
}



int perf_destroy(perf_t **perf_r)
{
	// destroy storage for performer
	perf_t *perf;

	perf = *perf_r;

	free(perf->symbol);
	free(perf);
	*perf_r = 0;
	return 0;
}




int add_perf(void)
{
	// Add a performer
	// TODO: have an "add mode"
	// 	where dots are placed by clicking on the field
	// TODO: Keep track of valid dots and allocated dots
	
	int i;
	int index;
	int perfnum = pshow->perfnum;
	int found_dot = 0;
	// nodes used for reallocation
	perf_t **newperfs;
	coord_t **newcoords;
	// nodes used for checking
	perf_t *perf;
	//coord_t *coord;
	set_t *last;
	int excode;

	// find an open node
	for (i=0; i<perfnum && found_dot == 0; i++)
	{
		// check this performer
		perf = pshow->perfs[i];
		if (perf->valid == 0)
		{
			found_dot = 1;
			index = i;
		}
	}
	if (!found_dot)
	{
		// no open nodes found, 
		// have to make more nodes
		// in every set
		last = pshow->sets->firstset;
		newperfs = (perf_t**)malloc((perfnum+5)*sizeof(perf_t*));
		if (!newperfs)
			return -1;
		for (i=0; i<perfnum; i++)
		{
			// copy old performers
			newperfs[i] = pshow->perfs[i];
		}
		// allocate the rest
		for (i=perfnum; i<perfnum+5; i++)
		{
			// make new performers
			excode = perf_construct(&newperfs[i]);
			if (excode == -1)
				return -1;
			/*
			newperfs[i] = (perf_t*)malloc(sizeof(perf_t));
			if (newperfs[i] == NULL)
				return -1;
			newperfs[i]->symbol = (char*)malloc(1*sizeof(char));
			if (newperfs[i]->symbol == NULL)
				return -1;
			newperfs[i]->symbol[0] = '\0';
			newperfs[i]->valid = -1;
			*/
		}
		// set new performers
		free(pshow->perfs);
		pshow->perfs = newperfs;
		// allocate coords for every set
		while (last != NULL)
		{
			newcoords = (coord_t**)
				malloc((perfnum+5)*sizeof(coord_t*));
			if (newcoords == NULL)
				return -1;
			for (i=0; i<perfnum; i++)
			{
				// copy old coords
				newcoords[i] = last->coords[i];
			}
			for (i=perfnum; i<perfnum+5; i++)
			{
				// copy the rest
				newcoords[i] = (coord_t*)
					malloc(sizeof(coord_t));
				if (newcoords[i] == NULL)
					return -1;
				newcoords[i]->x = 0;
				newcoords[i]->y = 0;
			}
			// hook up new coords
			free(last->coords);
			last->coords = newcoords;
			// go to next set
			last = last->next;
		}
		index = perfnum;
		pshow->perfnum = perfnum+5;
	}
	printf("performer created at %i\n", index);
	pshow->perfs[index]->valid = 1;
	select_dots_discard();
	select_dots_add(index);
	pushPerfMk(&pstate.undobr, index, 1);
	return index;
}



void revert_perf_selected(headset_t *dshow)
{
	// revert selected performers to dot at the previous set
	int index;
	// selections
	select_t *last;
	// coords
	coord_t *coord;
	// finished
	int done;
	last = pstate.select;
	undo_tclose();
	while (last)
	{
		// set dots as invalid
		index = last->index;
		coord = dshow->sets->currset->coords[index];
		if (last->next)
		{
			// more to delete
			done = 0;
		}
		else
		{
			// no more to delete
			done = 1;
		}
		pushPerfmv(&pstate.undobr, index, coord->x, coord->y, done);
		revert_perf(dshow, index);
		// go to next performer
		last = last->next;
	}
	return;
}
	

void revert_perf(headset_t *dshow, int index)
{
	// revert a performer's dot to the dot at the previous set
	//set_t *currset;
	set_t *prevset;
	//coord_t *coord;
	coord_t *prevcoord;

	//currset = dshow->sets->currset;
	prevset = dshow->sets->currset->prev;

	//coord = currset->coords[index];

	if (prevset == NULL)
	{
		// first set
		set_coord(dshow, index, 0, 0);
	}
	else
	{
		// not first set; use previous dots
		prevcoord = prevset->coords[index];
		set_coord(dshow, index, prevcoord->x, prevcoord->y);
	}
	return;
}

void delete_perf_selected(void)
{
	int index;
	select_t *last;
	perf_t *perf;
	perf_t **perfs;
	int done;
	last = pstate.select;
	perfs = pshow->perfs;
	while (last)
	{
		// set dots as invalid
		index = last->index;
		perf = perfs[index];
		if (last->next)
		{
			// more to delete
			done = 0;
		}
		else
		{
			// no more to delete
			done = 1;
		}
		pushPerfDel(&pstate.undobr, &perf, pshow->sets->firstset, done);
		// TODO: eventually have to unlink perf struct for undo
		pshow->perfs[index] = perf;
		delete_perf(perf);
		perfs[index]->valid = 0;
		// go to next performer
		last = last->next;
	}
	select_dots_discard();
	return;
}

void delete_perf(perf_t *perf)
{
	// render performer invalid
	//perf_t *perf;
	perf->valid = 0;
	return;
}



double check_stepsize_selected(headset_t *dshow)
{
	// get average stepsize of selected dots
	int count = 0;
	int index;
	int sCounts;
	double stepsize = 0;
	double x, y;
	double xpr, ypr;
	double dx, dy;
	double dxy;
	// sets
	set_t *currset = dshow->sets->currset;
	set_t *prevset = dshow->sets->currset->prev;
	// selection
	select_t *last = pstate.select;
	// coords
	coord_t **coords = currset->coords;
	coord_t **pcoords;

	//if (setnum == 0)
	if (pstate.setnum == 0)
		return 0;
	pcoords = prevset->coords;
	sCounts = currset->counts;
	while (last != NULL)
	{
		index = last->index;
		x = coords[index]->x;
		y = coords[index]->y;
		xpr = pcoords[index]->x;
		ypr = pcoords[index]->y;
		dx = x - xpr;
		dy = y - ypr;
		dxy = sqrt(powf(dx,2)+powf(dy,2));
		if (dxy)
			dxy = 8 * sCounts / dxy;
		stepsize = stepsize + dxy;
		count++;
		last = last->next;
	}
	if (count == 0)
		return 0;
	stepsize = stepsize / count;
	return stepsize;
}


int max_stepsize_selected(headset_t *dshow, double *stepsize_r)
{
	// get the max stepsize of a group
	// largest dot
	int largest = -1;
	// select index
	int index;
	int sCounts;
	double x, y;
	double xpr, ypr;
	double dx, dy;
	double dxy;
	// sets
	set_t *currset = dshow->sets->currset;
	set_t *prevset = dshow->sets->currset->prev;
	// selection
	select_t *last = pstate.select;
	// coords
	coord_t **coords = currset->coords;
	coord_t **pcoords;
	// stepsize
	double stepsize = 0;
	//if (setnum == 0)
	if (pstate.setnum == 0)
		return 0;
	pcoords = prevset->coords;
	sCounts = currset->counts;
	while (last != NULL)
	{
		index = last->index;
		x = coords[index]->x;
		y = coords[index]->y;
		xpr = pcoords[index]->x;
		ypr = pcoords[index]->y;
		dx = x - xpr;
		dy = y - ypr;
		dxy = sqrt(powf(dx,2)+powf(dy,2));
		if (dxy)
			dxy = 8 * sCounts / dxy;
		if ((stepsize > dxy && dxy >= 0.1) || stepsize <= 0.1)
		{
			stepsize = dxy;
			largest = index;
		}
		last = last->next;
	}
	*stepsize_r = stepsize;
	return largest;
}
	
