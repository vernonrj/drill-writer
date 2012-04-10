#include "drill.h"

int undo_destroy(struct undo_proto **undlast_r, struct headset_proto *dshow)
{
	struct undo_proto *undlast;
	struct undo_proto *undcurr;
	int i;
	int undop;
	int snum;
	struct perf_proto *pcurr;
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
				for (i=0; i<snum; i++)
					free(undcurr->coords[i]);
				free(undcurr->coords);
				pcurr = undcurr->ud.sperf;
				free(pcurr->name);
				free(pcurr->symbol);
				free(undcurr->ud.sperf);
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

int pushToStack(struct undo_proto *unredo, struct undo_proto **stack_r)
{
	// Push node to stack
	struct undo_proto *stack;
	time_t new_undo_timer;
	double tdiff;
	if (!unredo)
		return -1;
	time(&new_undo_timer);
 	tdiff = difftime(new_undo_timer, undo_timer);
	undo_timer = new_undo_timer;
	stack = *stack_r;

	unredo->next = stack;
	*stack_r = unredo;
	// link up undoes that happen in less than a second
	if (tdiff > 1)
		undo_tclose();
	return 0;
}

int pushSetMk(struct undo_proto **stack_r)
{
	// New set created, push set num onto undo stack
	struct undo_proto *unredo;
	int excode;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		printf("setnum = %i\n", setnum);
		unredo->operation = 0;	// set to be created
		unredo->done = 1;
		excode = pushToStack(unredo, stack_r);

		return excode;
	}
	else
		return -1;
}

int pushSetDel(struct undo_proto **stack_r, struct set_proto *oldset)
{
	// Set to be deleted, push current set onto stack
	struct undo_proto *unredo;
	int excode;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		unredo->operation = 1;		// set to be deleted
		unredo->ud.set = oldset;	// store set
		unredo->done = 1;		// finished
		excode = pushToStack(unredo, stack_r);	// push

		return excode;
	}
	else
		return -1;
}

int pushPerfMk(struct undo_proto **stack_r, int index, int done)
{
	// Perf to be added, push index onto stack
	struct undo_proto *unredo;
	int excode;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		unredo->operation = 2;		// Performer adding
		unredo->ud.pindex = index;	// store perf index
		unredo->done = done;		// check if finished
		excode = pushToStack(unredo, stack_r);	// push

		return excode;
	}
	else
		return -1;
	return 0;
}

int pushPerfDel(struct undo_proto **stack_r, struct perf_proto **oldperf_r, 
		struct set_proto *firstset, int done)
{
	// Perf to be deleted, push node onto stack
	struct undo_proto *unredo;
	struct set_proto *last;
	struct perf_proto *perf = 0;
	struct perf_proto *oldperf;
	struct perf_proto *newperf;
	volatile int i;
	int index;
	int excode;
	int set_tot;


	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (!unredo)
	{
		return -1;
	}
	oldperf = *oldperf_r;
	index = oldperf->index;
	unredo->set_num = setnum;
	unredo->operation = 3;		// Performer removed
	unredo->ud.sperf = *oldperf_r;	// store performer data 
	*oldperf_r = 0;
	perf_construct(&newperf);
	printf("newperf = 0x%x\n", newperf);
	newperf->valid = 0;
	// store coordinates for performer
	// get total number of sets
	set_last();
	unredo->coords = (struct coord_proto**)malloc((setnum+1)*sizeof(struct coord_proto*));
	if (!unredo->coords)
		return -1;
	goto_set(unredo->set_num);
	last = firstset;
	i = 0;
	//while (last != NULL)
	for (i=0; last != NULL; i++, last = last->next)
	{
		unredo->coords[i] = last->coords[index];
		last->coords[index] = (struct coord_proto*)malloc(sizeof(struct coord_proto));
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

int pushPerfmv(struct undo_proto **stack_r, int index, float x, float y, int done)
{
	// push the relative movement of perf onto stack
	struct undo_proto *unredo;
	int excode;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
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


int pushTempo(struct undo_proto **stack_r, int tempo)
{
	// push tempo change onto stack
	struct undo_proto *unredo;
	int excode;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
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

int pushCounts(struct undo_proto **stack_r, int set_num, int counts, int done)
{
	// push counts change onto stack
	struct undo_proto *unredo;
	int excode;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
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

int sourcePop(struct undo_proto **sourcebr_r)
{
	struct undo_proto *sourcebr;
	struct undo_proto *dscard;
	struct set_proto *oldset;
	struct coord_proto **coords;
	int i;
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



int popFromStack(struct headset_proto *dshow, struct undo_proto **sourcebr_r,
		struct undo_proto **destbr_r)
{
	// pop values from source stack, reverse operation,
	// and push it onto destination stack. Can be used for undo and redo
	int i;				// loop var
	struct undo_proto *sourcebr;	// source branch
	struct undo_proto *destbr;	// destination branch
	struct set_proto *currset;	// current set
	struct perf_proto *perfcurr;	// current performer
	struct coord_proto **coords;	// coordinates
	struct tempo_proto *tempo;
	int operation;			// specified operation
	int excode;
	int done = 1;
	int index;
	float x, y;			// coords for moving dot
	float xold, yold;


	// get operation
	sourcebr = *sourcebr_r;
	if (!sourcebr)
		return 1;
	destbr = *destbr_r;
	operation = sourcebr->operation;
	currset = dshow->firstset;
	i = 0;
	set_first();
	while (i < sourcebr->set_num)
	{
		set_next();
		currset = dshow->currset;
		i++;
	}
	printf("on set %i\n", setnum);
	if (currset == NULL)
	{
		// might have been last set. For testing purposes (i.e. for now),
		// print warning and continue, grabbing last set instead
		printf("WARNING(pop): passed set number we were looking for\n");
		printf("\tcontinuing using last set instead\n");
		set_last();
		currset = dshow->currset;
	}
	switch(operation)
	{
		case 0:		// set was created
			currset = dshow->currset;
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
				sourcebr->ud.set->next = dshow->firstset;
				dshow->firstset = sourcebr->ud.set;
				dshow->prevset = sourcebr->ud.set;
				set_first();
			}
			else if (sourcebr->set_num > setnum)
			{
				// last set needs to be added
				set_last();
				currset = dshow->currset;
				sourcebr->ud.set->next = 0;
				currset->next = sourcebr->ud.set;
			}
			else
			{
				set_prev();
				currset = dshow->currset;
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
			excode = pushPerfDel(&destbr, &perfcurr, dshow->firstset, done);
			break;
		case 3:		// perf was deleted
			// re-add performer
			index = add_perf();
			if (index == -1)
				return -1;
			printf("ping\n");
			perfcurr = dshow->perfs[index];
			printf("new address @ 0x%x\n", perfcurr->symbol);
			free(perfcurr->name);
			free(perfcurr->symbol);
			free(dshow->perfs[index]);
			dshow->perfs[index] = sourcebr->ud.sperf;
			perfcurr = sourcebr->ud.sperf;
			sourcebr->ud.sperf = 0;
			perfcurr->index = index;
			currset = dshow->firstset;
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
			currset = dshow->currset;
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
			excode = pushCounts(&destbr, setnum, dshow->currset->counts, 1);
			if (excode != -1)
				dshow->currset->counts = sourcebr->ud.counts;
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
	struct undo_proto *undobr;
	undobr = pshow->undobr;
	if (undobr != NULL)
	{
		// close undo cascade
		undobr->done = 1;
	}
	return;
}

