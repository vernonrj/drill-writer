// Functions that manipulate performers go here
#include "drill.h"

// performer storage
int perf_construct(struct perf_proto **perf_r)
{
	// Build storage for performer
	// give argument:
	// excode = perf_construct(&dots);
	// piece of performer array
	struct perf_proto *perf;

	perf = (struct perf_proto*) malloc(sizeof(struct perf_proto));
	if (perf == NULL)
	{
		// allocation error
		return -1;
	}
	// allocate values inside struct
	perf->name = (char*) malloc(1 * sizeof(char));
	perf->symbol = (char*) malloc(1 * sizeof(char));
	if (perf->name == NULL || perf->symbol == NULL)
	{
		// allocation error
		return -1;
	}
	perf->name[0] = '\0';
	perf->symbol[0] = '\0';
	coord_construct(&perf->vdot, 0, 0);
	perf->valid = -1;

	// pass by reference
	*perf_r = perf;

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
	struct perf_proto **newperfs;
	struct coord_proto **newcoords;
	// nodes used for checking
	struct perf_proto *perf;
	struct coord_proto *coord;
	struct set_proto *last;

	// find an open node
	for (i=0; i<perfnum && found_dot == 0; i++)
	{
		// check this performer
		perf = pshow->perfs[i];
		if (perf->valid == -1)
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
		last = pshow->firstset;
		newperfs = (struct perf_proto**)malloc((perfnum+5)*sizeof(struct perf_proto*));
		for (i=0; i<perfnum; i++)
		{
			// copy old performers
			newperfs[i] = pshow->perfs[i];
		}
		// allocate the rest
		for (i=perfnum; i<perfnum+5; i++)
		{
			// make new performers
			newperfs[i] = (struct perf_proto*)malloc(sizeof(struct perf_proto));
			if (newperfs[i] == NULL)
				return -1;
			newperfs[i]->name = (char*)malloc(1*sizeof(char));
			newperfs[i]->symbol = (char*)malloc(1*sizeof(char));
			if (newperfs[i]->name == NULL || newperfs[i]->symbol == NULL)
				return -1;
			newperfs[i]->name[0] = '\0';
			newperfs[i]->symbol[0] = '\0';
			newperfs[i]->valid = -1;
		}
		// set new performers
		free(pshow->perfs);
		pshow->perfs = newperfs;
		// allocate coords for every set
		while (last != NULL)
		{
			newcoords = (struct coord_proto**)
				malloc((perfnum+5)*sizeof(struct coord_proto*));
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
				newcoords[i] = (struct coord_proto*)
					malloc(sizeof(struct coord_proto));
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
	pshow->perfs[index]->valid = 0;
	pushPerfMk(&pshow->undobr, index, 1);
	return index;
}



void revert_perf_selected(struct headset_proto *dshow)
{
	// revert selected performers to dot at the previous set
	int index;
	// selections
	struct select_proto *last;
	// coords
	struct coord_proto *coord;
	// finished
	int done;
	last = dshow->select;
	undo_tclose();
	while (last)
	{
		// set dots as invalid
		index = last->index;
		coord = dshow->currset->coords[index];
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
		pushPerfmv(&dshow->undobr, index, coord->x, coord->y, done);
		revert_perf(dshow, index);
		// go to next performer
		last = last->next;
	}
	return;
}
	

void revert_perf(struct headset_proto *dshow, int index)
{
	// revert a performer's dot to the dot at the previous set
	struct set_proto *currset;
	struct set_proto *prevset;
	struct coord_proto *coord;
	struct coord_proto *prevcoord;

	currset = dshow->currset;
	prevset = dshow->prevset;

	coord = currset->coords[index];

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
	struct select_proto *last;
	struct perf_proto *perf;
	struct perf_proto **perfs;
	int done;
	last = pshow->select;
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
		pushPerfDel(&pshow->undobr, &perf, pshow->firstset, done);
		// TODO: eventually have to unlink perf struct for undo
		pshow->perfs[index] = perf;
		delete_perf(perf);
		perfs[index]->valid = -1;
		// go to next performer
		last = last->next;
	}
	select_discard();
	return;
}

void delete_perf(struct perf_proto *perf)
{
	// render performer invalid
	//struct perf_proto *perf;
	perf->valid = -1;
	return;
}








