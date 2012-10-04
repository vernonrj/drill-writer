// Functions that manipulate performers go here
#include "drillwriter.h"

// performer storage
perf_t *perf_construct(void)
{
	// Build storage for performer
	perf_t *perf;

	perf = (perf_t*) malloc(sizeof(perf_t));
	if (perf == NULL)
	{
		// allocation error
		return NULL;
	}
	// allocate values inside struct
	perf->symbol = (char*) malloc(2 * sizeof(char));
	if (perf->symbol == NULL)
	{
		// allocation error
		return NULL;
	}
	perf->symbol[0] = '\0';
	perf->valid = 0;

	return perf;
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




int perf_add(void)
{
	// Add a performer
	
	int i, setnum = 0;
	int index;
	int perfnum = pshow->perfnum;
	int found_dot = 0;
	// nodes used for reallocation
	perf_t **newperfs;
	coord_t **newcoords;
	// nodes used for checking
	perf_t *perf;
	set_t *last;

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
		setnum = 0;
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
			newperfs[i] = perf_construct();
			if (newperfs[i] == NULL)
				return -1;
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
			//last = last->next;
			last = set_get_next(pshow->sets, setnum);
			setnum++;
		}
		index = perfnum;
		pshow->perfnum = perfnum+5;
	}
	printf("performer created at %i\n", index);
	pshow->perfs[index]->valid = 1;
	//select_dots_discard();
	//select_dots_add_index(index);
	select_clear(pstate.select);
	select_add_dot(pstate.select, index);
	pushPerfMk(&pstate.undobr, index, 1);
	return index;
}



void perf_revert_selected(headset_t *dshow)
{
	// revert selected performers to dot at the previous set
	int index;
	// selections
	select_t *last;
	// coords
	coord_t *coord;
	// finished
	//int done;
	last = pstate.select;
	select_head(last);
	undo_tclose();
	while ((index = select_get_dot_advance(last)) != -1)
	{
		// set dots as invalid
		//index = last->index;
		//index = select_get_dot(last);
		coord = dshow->sets->currset->coords[index];
		//if (last->next)
		//if (select_has_next(last))
		/*
		if (!select_at_dot_end(last))
		{
			// more to delete
			done = 0;
		}
		else
		{
			// no more to delete
			done = 1;
		}
		*/
		pushPerfmv(&pstate.undobr, index, coord->x, coord->y, 1);
		perf_revert(dshow, index);
		// go to next performer
		//last = last->next;
		//last = select_get_next(last);
	}
	select_head(last);
	return;
}
	

void perf_revert(headset_t *dshow, int index)
{
	// revert a performer's dot to the dot at the previous set
	set_t *prevset;
	coord_t *prevcoord;

	//prevset = dshow->sets->currset->prev;

	if ((prevset = set_get_prev(pshow->sets, pstate.setnum))== NULL)
	{
		// first set
		coords_set_coord(dshow, index, 0, 0);
	}
	else
	{
		// not first set; use previous dots
		prevcoord = prevset->coords[index];
		coords_set_coord(dshow, index, prevcoord->x, prevcoord->y);
	}
	return;
}



void perf_delete_selected(void)
{
	// delete selected performers
	int index;
	select_t *last;
	perf_t *perf;
	perf_t **perfs;
	//int done;

	last = pstate.select;
	perfs = pshow->perfs;
	//while (last)
	while ((index = select_get_dot_advance(last)) != -1)
	{
		// set dots as invalid
		//index = last->index;
		//index = select_get_dot(last);
		perf = perfs[index];
		//if (last->next)
		/*
		if (select_has_next(last))
		{
			// more to delete
			done = 0;
		}
		else
		{
			// no more to delete
			done = 1;
		}
		*/
		pushPerfDel(&pstate.undobr, &perf, pshow->sets->firstset, 1);
		// TODO: eventually have to unlink perf struct for undo
		pshow->perfs[index] = perf;
		perf_delete(perf);
		perfs[index]->valid = 0;
		// go to next performer
		//last = last->next;
		//last = select_get_next(last);
	}
	//select_dots_discard();
	select_clear(pstate.select);
	return;
}



void perf_delete(perf_t *perf)
{
	// render performer invalid
	perf->valid = 0;
	return;
}



double perf_average_stepsize_selected(headset_t *dshow)
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
	set_t *prevset = set_get_prev(pshow->sets, pstate.setnum);
	// selection
	select_t *last = pstate.select;
	// coords
	coord_t **coords = currset->coords;
	coord_t **pcoords;

	if (!prevset)
		return 0;
	pcoords = prevset->coords;
	sCounts = currset->counts;
	select_head(last);
	//while (last != NULL)
	while ((index = select_get_dot_advance(last)) != -1)
	{
		//if (last->form)
		/*
		if (select_has_form(last))
		{
			//last = last->next;
			last = select_get_next(last);
			continue;
		}
		*/
		//index = last->index;
		//index = select_get_dot(last);
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
		//last = last->next;
		//last = select_get_next(last);
	}
	select_head(last);
	if (count == 0)
		return 0;
	stepsize = stepsize / count;
	return stepsize;
}



int perf_max_stepsize_selected(headset_t *dshow, double *stepsize_r)
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
	set_t *prevset = set_get_prev(pshow->sets, pstate.setnum);
	// selection
	select_t *last = pstate.select;
	// coords
	coord_t **coords = currset->coords;
	coord_t **pcoords;
	// stepsize
	double stepsize = 0;

	if (!prevset)
		return 0;
	pcoords = prevset->coords;
	sCounts = currset->counts;
	select_head(last);
	//while (last != NULL)
	while ((index = select_get_dot_advance(last)) != -1)
	{
		/*
		//if (last->form)
		if (select_has_form(last))
		{
			//last = last->next;
			last = select_get_next(last);
			continue;
		}
		*/
		//index = last->index;
		//index = select_get_dot(last);
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
		//last = last->next;
		//last = select_get_next(last);
	}
	*stepsize_r = stepsize;
	return largest;
}
