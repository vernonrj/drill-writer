#include "drill.h"

int undo_destroy(undo_t **undlast_r, headset_t *dshow)
{
	undo_t *undlast;
	undo_t *undcurr;
	int i;
	int undop;
	int snum;
	perf_t *pcurr;
	int perfnum = dshow->perfnum;
	undlast = *undlast_r;
	while (undlast != NULL)
	{
		undcurr = undlast;
		undop = undcurr->operation;
		switch(undop)
		{
			case 0:		// set created
				break;
			case 1:		// set destroyed
				set_cldestroy(&undcurr->ud.set, perfnum);
				break;
			case 2:		// perf added
				break;
			case 3:		// perf deleted
				// TODO: need to track allocation in undo branch
				snum = dshow->perfnum;
				for (i=0; i<snum; i++)
					free(undcurr->coords[i]);
				free(undcurr->coords);
				pcurr = undcurr->ud.sperf;
				free(pcurr->symbol);
				free(pcurr);
				break;
			case 4:		// perf moved
				break;
			case 5:		// tempo changed
				break;
			case 6:		// counts changed
				break;
		}
		undlast = undlast->next;
		free(undcurr);
	}
	*undlast_r = 0;
	return 0;
}

int pushToStack(undo_t *unredo, undo_t **stack_r)
{
	// Push node to stack
	undo_t *stack;
	time_t new_undo_timer;
	double tdiff;
	if (!unredo)
		return -1;
	time(&new_undo_timer);
 	//tdiff = difftime(new_undo_timer, undo_timer);
 	tdiff = difftime(new_undo_timer, pstate.undo_timer);
	pstate.undo_timer = new_undo_timer;
	stack = *stack_r;

	unredo->next = stack;
	*stack_r = unredo;
	// link up undoes that happen in less than a second
	if (tdiff > 1)
		undo_tclose();
	return 0;
}

int pushSetMk(undo_t **stack_r)
{
	// New set created, push set num onto undo stack
	undo_t *unredo;
	int excode;

	unredo = (undo_t*)malloc(sizeof(undo_t));
	if (unredo)
	{
		//unredo->set_num = setnum;
		unredo->set_num = pstate.setnum;
		//printf("setnum = %i\n", setnum);
		unredo->operation = 0;	// set to be created
		unredo->done = 1;
		excode = pushToStack(unredo, stack_r);

		return excode;
	}
	else
		return -1;
}

int pushSetDel(undo_t **stack_r, set_t *oldset)
{
	// Set to be deleted, push current set onto stack
	undo_t *unredo;
	int excode;

	unredo = (undo_t*)malloc(sizeof(undo_t));
	if (unredo)
	{
		//unredo->set_num = setnum;
		unredo->set_num = pstate.setnum;
		unredo->operation = 1;		// set to be deleted
		unredo->ud.set = oldset;	// store set
		unredo->done = 1;		// finished
		excode = pushToStack(unredo, stack_r);	// push

		return excode;
	}
	else
		return -1;
}

int pushPerfMk(undo_t **stack_r, int index, int done)
{
	// Perf to be added, push index onto stack
	undo_t *unredo;
	int excode;

	unredo = (undo_t*)malloc(sizeof(undo_t));
	if (unredo)
	{
		//unredo->set_num = setnum;
		unredo->set_num = pstate.setnum;
		unredo->operation = 2;		// Performer adding
		unredo->ud.pindex = index;	// store perf index
		unredo->done = done;		// check if finished
		excode = pushToStack(unredo, stack_r);	// push
		printf("ping\n");

		return excode;
	}
	else
		return -1;
	return 0;
}

int pushPerfDel(undo_t **stack_r, perf_t **oldperf_r, 
		set_t *firstset, int done)
{
	// Perf to be deleted, push node onto stack
	undo_t *unredo;
	set_t *last;
	//perf_t *perf = 0;
	perf_t *oldperf;
	perf_t *newperf;
	volatile int i;
	int index;
	int excode;
	//int set_tot;


	unredo = (undo_t*)malloc(sizeof(undo_t));
	if (!unredo)
	{
		return -1;
	}
	oldperf = *oldperf_r;
	index = oldperf->index;
	//unredo->set_num = setnum;
	unredo->set_num = pstate.setnum;
	unredo->operation = 3;		// Performer removed
	unredo->ud.sperf = *oldperf_r;	// store performer data 
	*oldperf_r = 0;
	excode = perf_construct(&newperf);
	if (excode == -1)
	{
		printf("performer allocation error\n");
		exit(255);
	}
	//printf("newperf = 0x%x\n", newperf);
	newperf->valid = 0;
	// store coordinates for performer
	// get total number of sets
	set_last();
	//unredo->coords = (coord_t**)malloc(
	//		(setnum+1)*sizeof(coord_t*));
	unredo->coords = (coord_t**)malloc(
			(pstate.setnum+1)*sizeof(coord_t*));
	if (!unredo->coords)
		return -1;
	goto_set(unredo->set_num);
	last = firstset;
	i = 0;
	//while (last != NULL)
	for (i=0; last != NULL; i++, last = last->next)
	{
		unredo->coords[i] = last->coords[index];
		last->coords[index] = (coord_t*)malloc(sizeof(coord_t));
		if (!last->coords[index])
			return -1;
		last->coords[index]->x = 0;
		last->coords[index]->y = 0;
		//i++;
		//last = last->next;
	}

	*oldperf_r = newperf;
	unredo->done = done;		// check if finished
	excode = pushToStack(unredo, stack_r);	// push

	return excode;
}

int pushPerfmv(undo_t **stack_r, int index, double x, double y, int done)
{
	// push the relative movement of perf onto stack
	undo_t *unredo;
	int excode;

	unredo = (undo_t*)malloc(sizeof(undo_t));
	if (unredo)
	{
		//unredo->set_num = setnum;
		unredo->set_num = pstate.setnum;
		unredo->operation = 4;		// Performer moved
		unredo->ud.pindex = index;	// store perf index
		unredo->x = x;			// store coords
		unredo->y = y;			// store coords
		unredo->done = done;		// check if finished
		excode = pushToStack(unredo, stack_r);	// push

		return excode;
	}
	else
		return -1;
	return 0;
}


int pushTempo(undo_t **stack_r, int tempo)
{
	// push tempo change onto stack
	undo_t *unredo;
	int excode;

	unredo = (undo_t*)malloc(sizeof(undo_t));
	if (unredo)
	{
		//unredo->set_num = setnum;
		unredo->set_num = pstate.setnum;
		unredo->operation = 5;		// tempo changed
		unredo->ud.tempo = tempo;	// store tempo
		unredo->done = 1;		// finished
		excode = pushToStack(unredo, stack_r);	// push

		return excode;
	}
	else
		return -1;
	return 0;
}

int pushCounts(undo_t **stack_r, int set_num, int counts, int done)
{
	// push counts change onto stack
	undo_t *unredo;
	int excode;

	unredo = (undo_t*)malloc(sizeof(undo_t));
	if (unredo)
	{
		unredo->set_num = set_num;
		unredo->operation = 6;		// counts changed
		unredo->ud.counts = counts;	// store counts
		unredo->done = done;		// finished
		printf("push done = %i\n", done);
		excode = pushToStack(unredo, stack_r);	// push

		return excode;
	}
	else
		return -1;
	return 0;
}

int sourcePop(undo_t **sourcebr_r)
{
	undo_t *sourcebr;
	undo_t *dscard;
	//set_t *oldset;
	//coord_t **coords;
	//int i;
	int done;
	sourcebr = *sourcebr_r;
	if (!sourcebr)
		return 1;
	dscard = *sourcebr_r;
	sourcebr = sourcebr->next;
	*sourcebr_r = sourcebr;
	done = dscard->done;
	free(dscard);
	return done;
}



int popFromStack(headset_t *dshow, undo_t **sourcebr_r,
		undo_t **destbr_r)
{
	// pop values from source stack, reverse operation,
	// and push it onto destination stack. Can be used for undo and redo
	int i;				// loop var
	undo_t *sourcebr;	// source branch
	undo_t *destbr;	// destination branch
	set_t *currset;	// current set
	perf_t *perfcurr;	// current performer
	coord_t **coords;	// coordinates
	//tempo_t *tempo;
	int operation;			// specified operation
	int excode;
	int done = 1;
	int index;
	double x, y;			// coords for moving dot
	double xold, yold;


	// get operation
	sourcebr = *sourcebr_r;
	if (!sourcebr)
		return 1;
	destbr = *destbr_r;
	operation = sourcebr->operation;
	currset = dshow->sets->firstset;
	i = 0;
	set_first();
	while (i < sourcebr->set_num)
	{
		set_next();
		currset = dshow->sets->currset;
		i++;
	}
	//printf("on set %i\n", setnum);
	if (currset == NULL)
	{
		// might have been last set. For testing purposes (i.e. for now),
		// print warning and continue, grabbing last set instead
		printf("WARNING(pop): passed set number we were looking for\n");
		printf("\tcontinuing using last set instead\n");
		set_last();
		currset = dshow->sets->currset;
	}
	switch(operation)
	{
		case 0:		// set was created
			currset = dshow->sets->currset;
			excode = pushSetDel(&destbr, currset);
			if (!excode)
			{
				delete_set();
				done = sourcePop(&sourcebr);
			}
			else
				done = 1;
			break;
		case 1:		// set was destroyed
			// link up old set
			if (sourcebr->set_num == 0)
			{
				// first set
				set_first();
				sourcebr->ud.set->next = dshow->sets->firstset;
				dshow->sets->firstset = sourcebr->ud.set;
				dshow->sets->currset->prev = sourcebr->ud.set;
				set_first();
			}
			//else if (sourcebr->set_num > setnum)
			else if (sourcebr->set_num > pstate.setnum)
			{
				// last set needs to be added
				set_last();
				currset = dshow->sets->currset;
				sourcebr->ud.set->next = 0;
				currset->next = sourcebr->ud.set;
			}
			else
			{
				set_prev();
				currset = dshow->sets->currset;
				sourcebr->ud.set->next = currset->next;
				currset->next = sourcebr->ud.set;
			}
			excode = pushSetMk(&destbr);
			if (!excode)
			{
				done = sourcePop(&sourcebr);
			}
			else
				done = 1;
			set_next();
			break;
		case 2: 	// perf was added
			// render new performer invalid
			perfcurr = dshow->perfs[sourcebr->ud.pindex];
			perfcurr->valid = 0;
			done = sourcePop(&sourcebr);
			excode = pushPerfDel(&destbr, &perfcurr, dshow->sets->firstset, done);
			break;
		case 3:		// perf was deleted
			// re-add performer
			index = add_perf();
			if (index == -1)
				return -1;
			perfcurr = dshow->perfs[index];
			printf("new address @ 0x%x\n", (unsigned int)perfcurr->symbol);
			free(perfcurr->symbol);
			free(dshow->perfs[index]);
			dshow->perfs[index] = sourcebr->ud.sperf;
			perfcurr = sourcebr->ud.sperf;
			sourcebr->ud.sperf = 0;
			perfcurr->index = index;
			currset = dshow->sets->firstset;
			i = 0;
			// re-add coordinates for performer
			coords = currset->coords;
			printf("performer added @ %i\n", index);
			while (currset != NULL)
			{
				free(coords[index]);
				coords[index] = sourcebr->coords[i];
				//coords[index]->x = sourcebr->coords[i]->x;
				//coords[index]->y = sourcebr->coords[i]->y;
				printf("(x,y) @ %i = %.2f, %.2f\n", i, coords[index]->x, coords[index]->y);
				//free(sourcebr->coords[i]);
				i++;
				currset = currset->next;
				if (currset)
					coords = currset->coords;
			}
			printf("ping\n");
			free(sourcebr->coords);
			done = sourcePop(&sourcebr);
			excode = pushPerfMk(&destbr, index, done);
			break;
		case 4:		// performer moved
			// move performer back
			currset = dshow->sets->currset;
			index = sourcebr->ud.pindex;
			perfcurr = dshow->perfs[index];
			xold = currset->coords[index]->x;
			yold = currset->coords[index]->y;
			x = sourcebr->x;
			y = sourcebr->y;
			done = sourcePop(&sourcebr);
			excode = pushPerfmv(&destbr, index, xold, yold, done);
			currset->coords[index]->x = x;
			currset->coords[index]->y = y;
			break;
		case 5:		// tempo changed
			// changed tempo back
			excode = pushTempo(&destbr, dshow->currtempo->tempo);
			if (excode != -1)
				change_tempo(sourcebr->ud.tempo, &dshow->currtempo);
			done = sourcePop(&sourcebr);
			break;
		case 6:		// count structure was changed
			// change counts back
			//excode = pushCounts(&destbr, setnum, 
					//dshow->sets->currset->counts, 1);
			excode = pushCounts(&destbr, pstate.setnum, 
					dshow->sets->currset->counts, 1);
			if (excode != -1)
				dshow->sets->currset->counts = sourcebr->ud.counts;
			done = sourcePop(&sourcebr);
			printf("done = %i\n", done);
			break;
	}
	*sourcebr_r = sourcebr;
	*destbr_r = destbr;
	return done;
}


void undo_tclose(void)
{
	undo_t *undobr;
	undobr = pstate.undobr;
	if (undobr != NULL)
	{
		// close undo cascade
		undobr->done = 1;
	}
	return;
}

