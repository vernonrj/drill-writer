// Functions that change sets go here
#include "drill.h"


int set_container_construct(struct set_container **setCont_r, int perfs)
{
	// construct the set container
	struct set_proto *sets = 0;
	struct set_container *setCont;
	int excode;

	setCont = (struct set_container*)malloc(sizeof(struct set_container));
	if (!setCont)
		return -1;
	excode = set_construct(&sets, perfs);
	if (excode == -1)
		return -1;
	setCont->currset = sets;

	//sets->prev = setC->prevset;
	sets->next = 0;
	*setCont_r = setCont;
	return 0;
}


int set_construct(struct set_proto **sets_r, int perfs)
{
	// Build storage for set
	//int i;
	int excode;
	
	struct set_proto *newset;
	struct set_proto *last;

	newset = (struct set_proto*) malloc(sizeof(struct set_proto));
	if (newset == NULL)
	{
		// allocation error
		return -1;
	}
	// allocate values inside set
	newset->name = (char*) malloc(1*sizeof(char));
	newset->info = (char*) malloc(1*sizeof(char));
	if (!newset->name || !newset->info)
		return -1;
	newset->name[0] = '\0';
	newset->info[0] = '\0';
	newset->counts = 0;
	newset->groups = (struct group_container*)malloc(sizeof(struct group_container));
	newset->groups->head = 0;
	newset->groups->include = 0;
	// make sure not classified as midset
	newset->midset = 0;
	// make coordinate system
	excode = coords_construct(&newset->coords, perfs);
	if (excode == -1)
		return -1;

	// link
	last = *sets_r;
	if (last != NULL)
	{
		// Link previous set
		newset->next = last->next;
		newset->prev = last;
		last->next = newset;
		last = newset;
	}
	else
	{
		newset->next = NULL;
		newset->prev = NULL;
		*sets_r = newset;
	}

	return 0;
}


int newset_create(struct set_container *sets)
{
	// make a new set at a point right after index
	int i;
	int excode;
	int mid = 0;
	double x, y;
	//int newcounts;

	// set structures
	struct set_proto *curr = sets->currset;
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
	if (curr == 0 && pshow->sets->firstset == NULL)
	{
		// Make the first set
		pshow->sets->firstset = last;
	}
	else if (curr == 0 && pshow->sets->firstset != NULL)
	{
		// Replace the first set with this set
		curr = pshow->sets->firstset;
		pshow->sets->firstset = last;
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
		if (nextset && pstate.curr_step)
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
				coords[i]->x = x*pstate.curr_step + pcoords[i]->x;
				y = (ncoords[i]->y - pcoords[i]->y) / nextset->counts;
				coords[i]->y = y*pstate.curr_step + pcoords[i]->y;
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
			nextset->counts = nextset->counts - pstate.curr_step;
			last->counts = pstate.curr_step;
			pstate.curr_step = 0;
		}
		else
			last->counts = curr->counts;
		// link
		last->next = curr->next;
		last->prev = curr;
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

	last = pshow->sets->currset;
	prevset = last->prev;
	//perfnum = pshow->perfnum;

	pushSetDel(&pstate.undobr, pshow->sets->currset);
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
		pshow->sets->currset = prevset->next;
		// delete node
		//free(last);
		// put current set on next set if it exists
		if (prevset->next == NULL)
		{
			// next set does not exist. Put current set
			// at new last set
			// TODO: Make more efficient
			set_first();
			set_last();
		}
		else
		{
			// next set does exist. Put current set there
			pshow->sets->currset = prevset->next;
		}
	}
	else
	{
		// At first set
		pshow->sets->firstset = last->next;
		//free(last);
		last = pshow->sets->firstset;
		if (last == NULL)
		{
			// No sets now, make a new first set
			excode = set_construct(&pshow->sets->firstset, pshow->perfnum);
			if (excode == -1)
				return -1;
		}
		pshow->sets->currset = pshow->sets->firstset;
	}
	return 0;
}



void goto_set(int set_buffer)
{
	int i = 0;
	struct set_proto *last = pshow->sets->firstset;
	//struct set_proto *curr = 0;
	for (i=0; i<set_buffer && last != NULL; i++)
	{
		// go to current set
		//curr = last;
		last = last->next;
		i++;
	}
	/*
	if (set_buffer<set_tot)
		setnum=set_buffer;
	*/
	if (last != NULL)
	{
		pshow->sets->currset = last;
		//pshow->sets->prevset = curr;
		//setnum = i;
		pstate.setnum = i;
	}
	return;
}



int isLastSet(void)
{
	// check to see if we're at the last set
	if (pshow->sets->currset->next == NULL)
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
	//if (pshow->sets->currset == pshow->sets->firstset)
	if (pshow->sets->currset->prev == NULL)
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

int add_set(void)
{
	// Add a set after the current one
	struct headset_proto *dshow = pshow;
	struct set_proto *nextset;
	int newcounts = 0;
	int excode;

	nextset = dshow->sets->currset->next;
	if (nextset && pstate.curr_step)
		newcounts = nextset->counts;
	excode = newset_create(dshow->sets);
	if (excode == -1)
		return -1;
	set_next();
	if (newcounts)
	{
		//pushCounts(&pstate.undobr, setnum, newcounts, 0);
		pushCounts(&pstate.undobr, pstate.setnum, newcounts, 0);
	}
	pushSetMk(&pstate.undobr);

	return 0;
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
		pshow->sets->currset = pshow->sets->firstset;
		//pshow->sets->prevset = 0;
		pstate.curr_step = 0;
		//setnum=0;
		pstate.setnum=0;
		//set_step=0;
	}
}


void set_last(void)
{
	// Goto the last set
	struct set_proto *last = pshow->sets->currset;
	if (!last)
		return;
	while (last->next != NULL)
	{
		/*
		if (pshow->sets->prevset == NULL)
			pshow->sets->prevset = pshow->sets->currset;
		else
			pshow->sets->prevset = pshow->sets->prevset->next;
		*/
		//pshow->sets->currset = pshow->sets->currset->next;
		last = last->next;
		//setnum++;
		pstate.setnum++;
	}
	pshow->sets->currset = last;
	pstate.curr_step = 0;

	//setnum=set_tot-1;
	//set_step=0;
	return;
}

void set_next(void)
{	// Move to the next set
	//if (!playing)
	{
		if (pshow->sets->currset->next != NULL)
		{
			//pshow->sets->prevset = pshow->sets->currset;
			pshow->sets->currset = pshow->sets->currset->next;
			//setnum++;
			pstate.setnum++;
			pstate.curr_step = 0;
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
			pstate.curr_step++;
			nextset = pshow->sets->currset;
			nextset = nextset->next;
			if (pstate.curr_step >= nextset->counts)
			{
				// to the next set
				set_next();
				/*
				pstate.curr_step = 0;
				pshow->sets->prevset = pshow->sets->currset;
				pshow->sets->currset = nextset;
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
	if (!isFirstSet() || pstate.curr_step)
	{
		pstate.curr_step--;
		if (pstate.curr_step < 0)
		{
			// to the next set
			pstate.curr_step = 0;
			set_prev();
			pstate.curr_step = pshow->sets->currset->next->counts-1;
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
		last = pshow->sets->currset;
		if (pstate.curr_step)
			pstate.curr_step = 0;
		//else if (pshow->sets->prevset != NULL)
		else if (last->prev != NULL)
		{
			// not already at first set, move backwards
			//pshow->sets->currset = pshow->sets->currset->prev;
			last = last->prev;
			pstate.setnum--;
			if (!last->prev)
			{
				// first set
				pstate.setnum = 0;
			}
			pshow->sets->currset = last;
			/*
			last = pshow->sets->firstset;
			if (pshow->sets->currset == last)
			{
				// first set
				//pshow->sets->prevset = 0;
				//setnum = 0;
				pstate.setnum = 0;
			}
			else
			{
				// find previous set
				while (last->next != pshow->sets->currset && last != NULL)
				{
					last = last->next;
				}
				if (last != NULL)
				{
					pshow->sets->prevset = last;
					//setnum--;
					pstate.setnum--;
				}
				else
				{
					pshow->sets->prevset = 0;
					//setnum = 0;
					pstate.setnum = 0;
				}
			}
			*/
		}
	}
}



