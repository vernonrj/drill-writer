// Functions that change sets go here
#include "drill.h"


int set_construct(struct set_proto **sets_r, int perfs)
{
	// Build storage for set
	//int i;
	int excode;
	
	struct set_proto *sets;
	struct set_proto *last;

	sets = (struct set_proto*) malloc(sizeof(struct set_proto));
	if (sets == NULL)
	{
		// allocation error
		return -1;
	}
	// allocate values inside set
	sets->name = (char*) malloc(1*sizeof(char));
	sets->info = (char*) malloc(1*sizeof(char));
	if (!sets->name || !sets->info)
		return -1;
	sets->name[0] = '\0';
	sets->info[0] = '\0';
	sets->counts = 0;
	// make sure not classified as midset
	sets->midset = 0;
	// make coordinate system
	excode = coords_construct(&sets->coords, perfs);
	if (excode == -1)
		return -1;


	// link
	last = *sets_r;
	if (last != NULL)
	{
		// Link previous set
		sets->next = last->next;
		last->next = sets;
		last = sets;
	}
	else
	{
		sets->next = 0;
		*sets_r = sets;
	}

	return 0;
}


int newset_create(struct set_proto *curr)
{
	// make a new set at a point right after index
	int i;
	int excode;
	int mid = 0;
	double x, y;
	//int newcounts;

	// set structures
	//struct set_proto *sets;
	struct set_proto *last;
	//struct set_proto *fset;
	struct set_proto *nextset;

	// coordinates
	struct coord_proto **coords;
	struct coord_proto **pcoords;
	struct coord_proto **ncoords;

	//sets = 0;
	last = 0;
	excode = set_construct(&last, pshow->perfnum);
	if (excode == -1)
		return -1;
	if (curr == 0 && pshow->firstset == NULL)
	{
		// Make the first set
		pshow->firstset = last;
	}
	else if (curr == 0 && pshow->firstset != NULL)
	{
		// Replace the first set with this set
		curr = pshow->firstset;
		pshow->firstset = last;
		pcoords = curr->coords;
		
		coords = last->coords;
		for (i=0; i<pshow->perfnum; i++)
		{
			// copy old first set dots into new first set
			coords[i]->x = pcoords[i]->x;
			coords[i]->y = pcoords[i]->y;
		}
		last->counts = curr->counts;
	}
	else
	{
		// allocate new set
		pcoords = curr->coords;
		coords = last->coords;
		nextset = curr->next;
		if (nextset && pshow->step)
		{
			ncoords = nextset->coords;
			mid = 1;
		}
		for (i=0; i<pshow->perfnum; i++)
		{
			// Copy previous dots into new set
			if (mid)
			{
				x = (ncoords[i]->x - pcoords[i]->x) / nextset->counts;
				coords[i]->x = x*pshow->step + pcoords[i]->x;
				y = (ncoords[i]->y - pcoords[i]->y) / nextset->counts;
				coords[i]->y = y*pshow->step + pcoords[i]->y;
			}
			else
			{
				coords[i]->x = pcoords[i]->x;
				coords[i]->y = pcoords[i]->y;
			}
		}
		if (mid)
		{
			// change counts to reflect a midset creation
			//newcounts = nextset->counts;
			nextset->counts = nextset->counts - pshow->step;
			last->counts = pshow->step;
			pshow->step = 0;
		}
		else
			last->counts = curr->counts;
		// link
		last->next = curr->next;
		curr->next = last;
		// set counts
		if (last->counts == 0)
		{
			last->counts = curr->counts;
			if (last->counts == 0)
				last->counts = 1;
		}
	}

	return 0;
}


int set_cldestroy(struct set_proto **setcurr_r, int perfnum)
{
	int i;
	struct set_proto *setcurr;
	struct coord_proto **coords;

	setcurr = *setcurr_r;
	// delete sets
	free(setcurr->name);
	free(setcurr->info);
	coords = setcurr->coords;
	for (i=0; i<perfnum; i++)
	{
		// delete coords
		free(coords[i]);
	}
	free(coords);
	free(setcurr);
	*setcurr_r = 0;
	return 0;
}


int set_destroy(void)
{
	// destroy current set
	//int i;
	//int perfnum;
	struct set_proto *last;
	struct set_proto *prevset;
	//struct set_proto *before;
	int excode;

	last = pshow->currset;
	prevset = pshow->prevset;
	//perfnum = pshow->perfnum;

	pushSetDel(&pshow->undobr, pshow->currset);
	// free memory in nodes
	/*
	free(last->name);
	free(last->info);
	for (i=0; i<perfnum; i++)
		free(last->coords[i]);
	free(last->coords);
	*/

	// unlink node
	if (prevset != NULL)
	{
		// Not at first set
		prevset->next = last->next;
		pshow->currset = prevset->next;
		// delete node
		//free(last);
		// put current set on next set if it exists
		if (prevset->next == NULL)
		{
			// next set does not exist. Put current set
			// at new last set
			set_first();
			set_last();
		}
		else
		{
			// next set does exist. Put current set there
			pshow->currset = prevset->next;
		}
	}
	else
	{
		// At first set
		pshow->firstset = last->next;
		//free(last);
		last = pshow->firstset;
		if (last == NULL)
		{
			// No sets now, make a new first set
			excode = set_construct(&pshow->firstset, pshow->perfnum);
			if (excode == -1)
				return -1;
		}
		pshow->currset = pshow->firstset;
	}
	return 0;
}



void goto_set(int set_buffer)
{
	int i = 0;
	struct set_proto *last = pshow->firstset;
	struct set_proto *curr = 0;
	{
		for (i=0; i<set_buffer && last != NULL; i++)
		{
			// go to current set
			curr = last;
			last = last->next;
			i++;
		}
		/*
		if (set_buffer<set_tot)
			setnum=set_buffer;
		*/
		if (last != NULL)
		{
			pshow->currset = last;
			pshow->prevset = curr;
			setnum = i;
		}
	}
	return;
}



int isLastSet(void)
{
	// check to see if we're at the last set
	if (pshow->currset->next == NULL)
	{
		// at the last set
		return 1;
	}
	else
	{
		// not at the last set
		return 0;
	}
	return 0;
}

int isFirstSet(void)
{
	// check to see if we're at the first set
	if (pshow->currset == pshow->firstset)
	{
		// at the first set
		return 1;
	}
	else
	{
		// not at the first set
		return 0;
	}
	return 0;
}

void add_set(void)
{
	// Add a set after the current one
	struct set_proto *nextset;
	int newcounts = 0;
	nextset = pshow->currset->next;
	if (nextset && pshow->step)
		newcounts = nextset->counts;
	newset_create(pshow->currset);
	set_next();
	if (newcounts)
		pushCounts(&pshow->undobr, setnum, newcounts, 0);
	pushSetMk(&pshow->undobr);

	return;
}


void delete_set(void)
{
	// delete a set
	// undo chain in set_destroy
	set_destroy();
}


void set_first(void)
{	// Move to first set
	//if (!playing)
	{
		pshow->currset = pshow->firstset;
		pshow->prevset = 0;
		pshow->step = 0;
		setnum=0;
		//set_step=0;
	}
}


void set_last(void)
{
	// Goto the last set
	//if (!playing)
	{
		while (pshow->currset->next != NULL)
		{
			if (pshow->prevset == NULL)
				pshow->prevset = pshow->currset;
			else
				pshow->prevset = pshow->prevset->next;
			pshow->currset = pshow->currset->next;
			setnum++;
		}
		pshow->step = 0;

		//setnum=set_tot-1;
		//set_step=0;
	}
}

void set_next(void)
{	// Move to the next set
	//if (!playing)
	{
		if (pshow->currset->next != NULL)
		{
			pshow->prevset = pshow->currset;
			pshow->currset = pshow->currset->next;
			setnum++;
			pshow->step = 0;
		}
	}
}


void set_next_count(void)
{
	struct set_proto *nextset;
	//if (!playing)
	{	// shouldn't use this when playing
		//printf("set_next setnum=%i\nset_tot=%i\nset_step=%i\n", setnum, set_tot, set_step);
		if (!isLastSet())
		{
			pshow->step++;
			nextset = pshow->currset;
			nextset = nextset->next;
			if (pshow->step >= nextset->counts)
			{
				// to the next set
				set_next();
				/*
				pshow->step = 0;
				pshow->prevset = pshow->currset;
				pshow->currset = nextset;
				setnum++;
				*/
			}
		}
		/*
		if (setnum+1<set_tot)
			set_step++;
		else
			setnum=set_tot-1;
		if (set_step >= counts[setnum+1] && setnum+1<set_tot)
		{
			set_step=0;
			setnum++;
		}
		*/
	}
}

void set_prev_count(void)
{
	// go to the previous count
	//struct set_proto *nextset;
	if (!isFirstSet() || pshow->step)
	{
		pshow->step--;
		if (pshow->step < 0)
		{
			// to the next set
			pshow->step = 0;
			set_prev();
			pshow->step = pshow->currset->next->counts-1;
		}
	}
	return;
}


void set_prev(void)
{
	struct set_proto *last;

	//if (!playing)	// shouldn't use this when playing
	{
		// if in the middle of set,
		// go to beginning of set
		if (pshow->step)
			pshow->step = 0;
		else if (pshow->prevset != NULL)
		{
			// not already at first set, move backwards
			pshow->currset = pshow->prevset;
			last = pshow->firstset;
			if (pshow->currset == last)
			{
				// first set
				pshow->prevset = 0;
				setnum = 0;
			}
			else
			{
				// find previous set
				while (last->next != pshow->currset && last != NULL)
				{
					last = last->next;
				}
				if (last != NULL)
				{
					pshow->prevset = last;
					setnum--;
				}
				else
				{
					pshow->prevset = 0;
					setnum = 0;
				}
			}
		}
		/*
		if (!set_step)
			setnum--;
		else	// go back to start of set, instead of set-1
			set_step=0;
		if (setnum < 0)
			setnum = 0;
		*/
	}
}



