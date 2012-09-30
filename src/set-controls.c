// Functions that change sets go here
#include "drillwriter.h"


set_container_t *set_container_construct(int perfs)
{
	// construct the set container
	set_t *sets = 0;
	set_t **setlist;
	set_container_t *setCont;
	int excode;
	int size = 1, size_alloc = 5;

	setCont = (set_container_t*)malloc(sizeof(set_container_t));
	if (!setCont)
		return NULL;

	excode = set_construct(&sets, perfs);
	if (excode == -1)
		return NULL;
	setCont->currset = sets;
	pstate.setnum = 0;

	setCont->size = size;
	setCont->size_alloc = size_alloc;
	setlist = (set_t**)malloc(size_alloc*sizeof(set_t**));
	setCont->setlist = setlist;
	setlist[0] = sets;

	//sets->prev = setC->prevset;
	sets->next = NULL;
	return setCont;
}


set_container_t *set_container_add_before(set_container_t *set_container, int setnum)
{
	int i;
	int size, size_alloc;
	set_t *newset;
	set_t **setlist = set_container->setlist;
	set_t **newsetlist;
	size_alloc = set_container->size_alloc; 
	size = set_container->size;
	if (size+1 >= size_alloc)
	{
		newsetlist = (set_t**)malloc((size_alloc+5)*sizeof(set_t*));
		for(i=0; i<size; i++)
			newsetlist[i] = setlist[i];
		set_container->setlist = newsetlist;
		free(setlist);
		set_container->size_alloc += 5;
		setlist = newsetlist;
	}

	newset = set_construct_before(setlist[setnum], pshow->perfnum);
	if (newset != setlist[setnum])
	{
		for(i=set_container->size; i>setnum; i--)
			setlist[i] = setlist[i-1];
		setlist[setnum] = newset;
	}
	set_container->size++;
	return set_container;
}

set_container_t *set_container_add_after(set_container_t *set_container, int setnum)
{
	int i;
	int size, size_alloc;
	set_t *newset;
	set_t **setlist = set_container->setlist;
	set_t **newsetlist;
	size_alloc = set_container->size_alloc; 
	size = set_container->size;
	if (size+1 >= size_alloc)
	{
		newsetlist = (set_t**)malloc((size_alloc+5)*sizeof(set_t*));
		for(i=0; i<size; i++)
			newsetlist[i] = setlist[i];
		set_container->setlist = newsetlist;
		free(setlist);
		set_container->size_alloc += 5;
		setlist = newsetlist;
	}

	newset = set_construct_after(setlist[setnum], pshow->perfnum);
	if (newset != setlist[setnum])
	{
		for(i=set_container->size; i>setnum+1; i--)
			setlist[i] = setlist[i-1];
		setlist[setnum] = newset;
	}
	set_container->size++;
	return set_container;
}

set_t *set_construct_before(set_t *sets, int perfs)
{
	set_t **newset;
	set_t *last;
	set_t *prev;
	int excode;
	// interface with set_construct
	newset = (set_t**)malloc(sizeof(set_t*));
	excode = set_construct(newset, perfs);
	if (excode != -1 && sets)
	{
		last = *newset;
		free(newset);
		prev = sets->prev;
		if (prev)
			prev->next = last;
		last->prev = prev;
		last->next = sets;
		sets->prev = last;
		sets = last;
	}
	return sets;
}

set_t *set_construct_after(set_t *sets, int perfs)
{
	set_t **newset;
	set_t *last;
	set_t *next;
	int excode;
	// interface with set_construct
	newset = (set_t**)malloc(sizeof(set_t*));
	excode = set_construct(newset, perfs);
	if (excode != -1 && sets)
	{
		last = *newset;
		free(newset);
		next = sets->next;
		if (next)
			next->prev = last;
		last->next = next;
		last->prev = sets;
		sets->next = last;
		sets = last;
	}
	return sets;
}

int set_construct(set_t **sets_r, int perfs)
{
	// Build storage for set
	
	set_t *newset;
	set_t *last;

	newset = (set_t*) malloc(sizeof(set_t));
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
	//newset->groups = (group_t*)malloc(sizeof(group_t));
	newset->groups = NULL;

	// make sure not classified as midset
	newset->midset = 0;
	// make coordinate system
	newset->coords = coords_construct(perfs);
	if (!newset->coords)
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




int set_cldestroy(set_t **setcurr_r, int perfnum)
{
	// Just destruct set, don't do any cleanup
	set_t *setcurr;
	coord_t **coords;

	setcurr = *setcurr_r;
	// delete sets
	free(setcurr->name);
	free(setcurr->info);
	coords = setcurr->coords;
	coords = coords_destruct(coords, perfnum);
	free(setcurr);
	*setcurr_r = 0;
	return 0;
}


void set_destroy(int set_index)
{
	// destroy current set
	int i;
	set_t *last;
	set_t *prevset;
	int excode;
	int size;
	set_t **setlist;

	last = pshow->sets->currset;
	prevset = last->prev;

	pushSetDel(&pstate.undobr, pshow->sets->currset);

	pshow->sets->size--;
	size = pshow->sets->size;
	setlist = pshow->sets->setlist;
	for(i=set_index; i<size; i++)
		setlist[i] = setlist[i+1];
	if (set_index == 0)
		pshow->sets->firstset = pshow->sets->setlist[0];

	// unlink node
	if (prevset != NULL)
	{
		// Not at first set
		prevset->next = last->next;
		pshow->sets->currset = prevset->next;
		// put current set on next set if it exists
		if (prevset->next == NULL)
		{
			// next set does not exist. Put current set
			// at new last set
			// TODO: Make more efficient
			set_first();
			pstate.setnum = 0;
			set_last();
		}
		else
		{
			// next set does exist. Put current set there
			last = last->next;
			last->prev = prevset;
			pshow->sets->currset = prevset->next;
		}
	}
	else
	{
		// At first set
		pshow->sets->firstset = last->next;
		last = pshow->sets->firstset;
		if (last == NULL)
		{
			// No sets now, make a new first set
		}
		else 
			last->prev = NULL;
		pshow->sets->currset = pshow->sets->firstset;
		pstate.setnum = 0;
	}
	if (!size)
		excode = set_construct(&pshow->sets->firstset, pshow->perfnum);
	return;
}



int newset_create(set_container_t *sets)
{
	// make a new set at a point right after index
	int i;
	int excode;
	int mid = 0;
	double x, y;

	// set structures
	set_t *curr = sets->currset;
	set_t *last;
	set_t *nextset;

	// coordinates
	coord_t **coords;
	coord_t **pcoords;
	coord_t **ncoords;

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




void goto_set(int set_buffer)
{
	int i = 0;
	set_t *curr = pshow->sets->currset;
	set_t *last = pshow->sets->firstset;

	pstate.setnum = set_buffer;
	for (i=0; i<set_buffer && last != NULL; i++)
	{
		// go to current set
		last = last->next;
		i++;
	}
	if (last != NULL)
	{
		pshow->sets->currset = last;
		pstate.setnum = i;
		pstate.select = select_update_scope_set1_set2(pstate.select, curr, last);
	}
	return;
}



int isLastSet(void)
{
	// check to see if we're at the last set
	/*
	set_container_t *setC = pshow->sets;
	return (setC->set_index == setC->size-1);
	*/
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
	//return (pshow->sets->set_index == 0);
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
	headset_t *dshow = pshow;
	set_t *nextset;
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
		pushCounts(&pstate.undobr, pstate.setnum, newcounts, 0);
	}
	pushSetMk(&pstate.undobr);

	return 0;
}



void delete_set(void)
{
	// delete a set
	// undo chain in set_destroy
	set_destroy(pstate.setnum);
}


void set_first(void)
{	// Move to first set
	pstate.select = select_update_scope_set1_set2(pstate.select, pshow->sets->currset, pshow->sets->firstset);
	pshow->sets->currset = pshow->sets->firstset;
	pstate.curr_step = 0;
	pstate.setnum=0;
}



void set_last(void)
{
	// Goto the last set
	set_t *last = pshow->sets->currset;
	if (!last)
		return;
	while (last->next != NULL)
	{
		last = last->next;
		pstate.setnum++;
	}
	pstate.select = select_update_scope_set1_set2(pstate.select, pshow->sets->currset, last);
	pshow->sets->currset = last;
	pstate.curr_step = 0;

	return;
}



void set_next(void)
{	// Move to the next set
	if (pshow->sets->currset->next != NULL)
	{
		pstate.select = select_update_scope_set1_set2(pstate.select, pshow->sets->currset, pshow->sets->currset->next);
		pshow->sets->currset = pshow->sets->currset->next;
		pstate.setnum++;
		pstate.curr_step = 0;
	}
}



void set_next_count(void)
{
	// Go to the next count in the set
	set_t *nextset;
	if (!isLastSet())
	{
		pstate.curr_step++;
		nextset = pshow->sets->currset;
		nextset = nextset->next;
		if (pstate.curr_step >= nextset->counts)
		{
			// to the next set
			set_next();
		}
	}
}



void set_prev_count(void)
{
	// go to the previous count
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
	set_t *last;

	// if in the middle of set,
	// go to beginning of set
	last = pshow->sets->currset;
	if (pstate.curr_step)
		pstate.curr_step = 0;
	else if (last->prev != NULL)
	{
		// not already at first set, move backwards
		last = last->prev;
		pstate.setnum--;
		if (!last->prev)
		{
			// first set
			pstate.setnum = 0;
		}
		pstate.select = select_update_scope_set1_set2(pstate.select, pshow->sets->currset, last);
		pshow->sets->currset = last;
	}
}



