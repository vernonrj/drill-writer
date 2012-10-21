// Functions that change sets go here
#include "drillwriter.h"
#include "dr_forms.h"


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

	return setCont;
}



set_container_t *set_container_add_before(set_container_t *set_container, int setnum)
{
	// add a new set before setnum
	set_t *newset;
	set_t *nextset;
	int i;
	int perfnum = pshow->perfnum;
	set_t *currset = set_container->setlist[setnum];
	double xnew, ynew, x2, y2, x, y;
	int curr_step = pstate.curr_step;
	int counts;
	form_child_t *form = currset->forms, *newform;
	int newsetnum;

	if (curr_step)
		nextset = set_container->setlist[setnum+1];
	set_construct(&newset, pshow->perfnum);
	set_container = set_container_add_set_before(set_container, newset, setnum);
	if (currset)
	{
		for (i=0; i<perfnum; i++)
		{
			if (pshow->perfs[i]->valid)
			{
				x = currset->coords[i]->x;
				y = currset->coords[i]->y;
				if (curr_step && setnum+1 < pshow->sets->size)
				{
					x2 = pshow->sets->setlist[setnum+2]->coords[i]->x;
					y2 = pshow->sets->setlist[setnum+2]->coords[i]->y;
					counts = nextset->counts;
					xnew = x + ((x2 - x) / counts * curr_step);
					ynew = y + ((y2 - y) / counts * curr_step);
					currset->coords[i]->x = xnew;
					currset->coords[i]->y = ynew;
					//pshow->sets->setlist[setnum+2]->counts = counts - curr_step;
					//newset->counts = curr_step;
				}
				newset->coords[i]->x = x;
				newset->coords[i]->y = y;
			}
		}
	}
	if (curr_step)
	{
		currset->counts = curr_step;
		nextset->counts = nextset->counts - curr_step;
	}
	newsetnum = setnum;
	setnum = setnum+1;
	while (form)
	{
		form_parent_copy_to(form->parent, setnum, newsetnum);
		//form_parent_copy_to(form->parent, setnum, setnum-1);
		newform = form->parent->forms[newsetnum]; 
		//form_add_to_set(newform, newsetnum);
		form_add_to_set(newform, newset);
		form = form->next;
	}
	return set_container;
}



set_container_t *set_container_add_set_before(set_container_t *set_container, set_t *newset, int setnum)
{
	// insert newset before setnum
	int i, j;
	int size, size_alloc;
	set_t **setlist = set_container->setlist;
	set_t **newsetlist;
	form_container_t *fcont = pshow->topforms;
	form_parent_t *fparent = fcont->forms[0];
	form_child_t **newforms;
	form_child_t **forms;

	size_alloc = set_container->size_alloc; 
	size = set_container->size;
	if (size+1 >= size_alloc)
	{
		for (i=0; i<fcont->size; i++)
		{
			fparent = fcont->forms[i];
			forms = fparent->forms;
			newforms = (form_child_t**)malloc((size_alloc+5)*sizeof(form_child_t*));
			for (j=0; j<size; j++)
				newforms[j] = forms[j];
			fparent->forms = newforms;
			free(forms);
		}
		newsetlist = (set_t**)malloc((size_alloc+5)*sizeof(set_t*));
		for(i=0; i<size; i++)
			newsetlist[i] = setlist[i];
		set_container->setlist = newsetlist;
		free(setlist);
		set_container->size_alloc += 5;
		setlist = newsetlist;
	}
	if (fcont)
	{
		for (i=0; i<fcont->size; i++)
		{
			fparent = fcont->forms[i];
			for (j=set_container->size; j>setnum; j--)
				fparent->forms[j] = fparent->forms[j-1];
			fparent->forms[setnum] = NULL;
		}
	}

	if (newset != setlist[setnum])
	{
		//form_container_move_children_including(pshow->topforms, setnum);
		for(i=set_container->size; i>setnum; i--)
			setlist[i] = setlist[i-1];
		setlist[setnum] = newset;
		if (setnum == 0)
			pshow->sets->firstset = newset;
	}
	set_container->size++;
	return set_container;
}



set_container_t *set_container_add_after(set_container_t *set_container, int setnum)
{
	// add a new set after setnum
	int i;
	int perfnum = pshow->perfnum;
	set_t *newset;
	set_t *currset = set_container->setlist[setnum];
	set_t *nextset;
	double x2, y2, x, y;
	int curr_step = pstate.curr_step;
	int counts;
	form_child_t *form = currset->forms;
	form_child_t *newform;

	if (curr_step)
		nextset = set_container->setlist[setnum+1];
	set_construct(&newset, perfnum);
	set_container = set_container_add_set_after(set_container, newset, setnum);
	if (currset)
	{
		for (i=0; i<perfnum; i++)
		{
			if (pshow->perfs[i]->valid)
			{
				x = currset->coords[i]->x;
				y = currset->coords[i]->y;
				if (curr_step && setnum+2 < pshow->sets->size)
				{
					x2 = pshow->sets->setlist[setnum+2]->coords[i]->x;
					y2 = pshow->sets->setlist[setnum+2]->coords[i]->y;
					counts = nextset->counts;
					x = x + ((x2 - x) / counts * curr_step);
					y = y + ((y2 - y) / counts * curr_step);
					//pshow->sets->setlist[setnum+2]->counts = counts - curr_step;
					//newset->counts = curr_step;
				}
				newset->coords[i]->x = x;
				newset->coords[i]->y = y;
			}
		}
	}
	if (curr_step)
	{
		newset->counts = curr_step;
		nextset->counts = nextset->counts - curr_step;
	}
	while (form)
	{
		form_parent_copy_to(form->parent, setnum, setnum+1);
		newform = form->parent->forms[setnum+1]; 
		//form_add_to_set(newform, setnum+1);
		form_add_to_set(newform, newset);
		form = form->next;
	}

	return set_container;
}



set_container_t *set_container_add_set_after(set_container_t *set_container, set_t *newset, int setnum)
{
	// insert newset after setnum
	int i, j = 0;
	int size, size_alloc;
	set_t **setlist = set_container->setlist;
	set_t **newsetlist;
	form_container_t *fcont = pshow->topforms;
	form_parent_t *fparent = fcont->forms[0];
	form_child_t **newforms;
	form_child_t **forms;

	size_alloc = set_container->size_alloc; 
	size = set_container->size;
	if (size+1 >= size_alloc)
	{
		//while (fparent)
		if (fcont)
		{
			for (i=0; i<fcont->size; i++)
			{
				fparent = fcont->forms[i];
				forms = fparent->forms;
				newforms = (form_child_t **)malloc((size_alloc+5)*sizeof(form_child_t*));
				for (j=0; j<size; j++)
					newforms[j] = forms[j];
				fparent->forms = newforms;
				free(forms);
				//fparent = LIST_NEXT(fparent, formlist_entries);
			}
		}
		newsetlist = (set_t**)malloc((size_alloc+5)*sizeof(set_t*));
		for(i=0; i<size; i++)
			newsetlist[i] = setlist[i];
		set_container->setlist = newsetlist;
		free(setlist);
		set_container->size_alloc += 5;
		setlist = newsetlist;
	}
	//fparent = LIST_FIRST(flist);
	//while (fparent)
	if (fcont)
	{
		for (i=0; i<fcont->size; i++)
		{
			fparent = fcont->forms[i];
			for (j=set_container->size; j>setnum+1; j--)
				fparent->forms[j] = fparent->forms[j-1];
			fparent->forms[setnum+1] = NULL;
			//fparent = LIST_NEXT(fparent, formlist_entries);
		}
	}

	for(i=set_container->size; i>setnum+1; i--)
		setlist[i] = setlist[i-1];
	setlist[setnum+1] = newset;
	set_container->size++;
	return set_container;
}



set_container_t *set_container_destruct(set_container_t *set_container)
{
	int i;
	for(i=0; i<pshow->sets->size; i++)
		set_cldestroy(&pshow->sets->setlist[i], pshow->perfnum);
	return set_container;
}



int set_construct(set_t **sets_r, int perfs)
{
	// allocate a new set
	
	set_t *newset;

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
	newset->counts = 8;
	newset->groups = NULL;

	// make sure not classified as midset
	newset->midset = 0;
	// make coordinate system
	newset->coords = coords_construct(perfs);
	if (!newset->coords)
		return -1;
	newset->forms = NULL;

	*sets_r = newset;

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
	int i, j;
	int size;
	set_t **setlist;
	form_container_t *fcont = pshow->topforms;
	form_parent_t *fparent = (fcont ? fcont->forms[0] : NULL);

	setlist = pshow->sets->setlist;

	pushSetDel(&pstate.undobr, pshow->sets->currset);

	size = pshow->sets->size;
	for(i=set_index; i<size; i++)
		setlist[i] = setlist[i+1];
	if (fcont)
	{
		for (i=0; i<fcont->size; i++)
		{
			fparent = fcont->forms[i];
			for (j=set_index; j < fcont->size-1; j++)
				fparent->forms[j] = fparent->forms[j+1];
			fparent->forms[fcont->size-1] = NULL;
		}
	}
	if (set_index == 0)
		pshow->sets->firstset = pshow->sets->setlist[0];

	size = --pshow->sets->size;
	if (set_index >= size)
		pstate.setnum = (size-1);
	if (!size)
	{
		set_construct(&pshow->sets->firstset, pshow->perfnum);
		setlist[0] = pshow->sets->firstset;
		pstate.setnum = 0;
	}
	pshow->sets->currset = pshow->sets->setlist[pstate.setnum];
	return;
}



void goto_set(int set_buffer)
{

	if (set_buffer < pshow->sets->size)
	{
		//select_update_scope_set1_set2(pstate.select, pshow->sets->currset, pshow->sets->setlist[set_buffer]);
		pstate.setnum = set_buffer;
		pshow->sets->currset = pshow->sets->setlist[set_buffer];
		pstate.curr_step = 0;
	}
	return;
}



int isLastSet(void)
{
	// check to see if we're at the last set
	set_container_t *setC = pshow->sets;
	return (pstate.setnum == setC->size-1);
}



int isFirstSet(void)
{
	// check to see if we're at the first set
	return (pstate.setnum == 0);
}



int set_add_before_current(void)
{
	// add a set before the current set
	// Add a set after the current one
	int newcounts = 0;

	pshow->sets = set_container_add_before(pshow->sets, pstate.setnum);
	//set_next();
	newcounts = pshow->sets->currset->counts;
	if (newcounts)
	{
		pushCounts(&pstate.undobr, pstate.setnum, newcounts, 0);
		pshow->sets->currset->counts = newcounts;
	}
	else
		pshow->sets->currset->counts = 8;
	pushSetMk(&pstate.undobr);

	return 0;
}
	

int set_add_after_current(void)
{
	// Add a set after the current one
	headset_t *dshow = pshow;
	int newcounts = 0;

	dshow->sets = set_container_add_after(dshow->sets, pstate.setnum);
	set_next();
	newcounts = pshow->sets->currset->counts;
	if (newcounts)
	{
		pushCounts(&pstate.undobr, pstate.setnum, newcounts, 0);
		dshow->sets->currset->counts = newcounts;
	}
	else
		dshow->sets->currset->counts = 8;
	pushSetMk(&pstate.undobr);

	return 0;
}


/*
int add_set(void)
{
	return set_add_after_current();
}
*/




void delete_set(void)
{
	// delete a set
	// undo chain in set_destroy
	set_destroy(pstate.setnum);
}



set_t *set_get_first(set_container_t *set_container)
{	// Move to first set
	return set_container->setlist[0];
}



set_t *set_get_last(set_container_t *set_container)
{
	// Goto the last set
	int setnum = pshow->sets->size-1;
	return set_container->setlist[setnum];
}



set_t *set_get_next(set_container_t *set_container, int index)
{	// Move to the next set
	if (index < pshow->sets->size-1)
	{
		return set_container->setlist[index+1];
	}
	return NULL;
}



set_t *set_get_prev(set_container_t *set_container, int index)
{
	// if in the middle of set,
	// go to beginning of set
	if (index > 0)
		return set_container->setlist[index-1];

	return NULL;
}



void set_first(void)
{
	//pstate.select = select_update_scope_set1_set2(pstate.select, pshow->sets->currset, pshow->sets->firstset);
	pstate.curr_step = 0;
	pstate.setnum = 0;
	pshow->sets->currset = set_get_first(pshow->sets);
}



void set_last(void)
{
	//pstate.select = select_update_scope_set1_set2(pstate.select, pshow->sets->currset, set_get_last(pshow->sets));
	pstate.curr_step = 0;
	pstate.setnum = pshow->sets->size-1;
	pshow->sets->currset = set_get_last(pshow->sets);
}



void set_prev(void)
{
	set_t *set;
	//pstate.select = select_update_scope_set1_set2(pstate.select, pshow->sets->currset, pshow->sets->firstset);
	if (pstate.curr_step)
		pstate.curr_step = 0;
	else
	{
		if ((set = set_get_prev(pshow->sets, pstate.setnum)) != NULL)
		{
			pshow->sets->currset = set;
			pstate.setnum--;
		}
	}
	return;

}



void set_next(void)
{
	set_t *set;
	//pstate.select = select_update_scope_set1_set2(pstate.select, pshow->sets->currset, pshow->sets->firstset);
	if (pstate.curr_step)
		pstate.curr_step = 0;
	if ((set = set_get_next(pshow->sets, pstate.setnum)) != NULL)
	{
		pshow->sets->currset = set;
		pstate.setnum++;
	}
	return;
}



void set_next_count(void)
{
	// Go to the next count in the set
	set_t *nextset;
	if (!isLastSet())
	{
		pstate.curr_step++;
		nextset = pshow->sets->setlist[pstate.setnum+1];
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
	int counts;
	if (!isFirstSet() || pstate.curr_step)
	{
		counts = pshow->sets->currset->counts;
		pstate.curr_step--;
		if (pstate.curr_step < 0)
		{
			// to the previous set
			pstate.curr_step = 0;
			set_prev();
			pstate.curr_step = counts-1;
		}
	}
	return;
}
