// Functions manipulating count structure goes here
#include "drill.h"
void goto_count (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int count_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_counts));
		count_buffer = atoi(entry_buffer);
		if (!isLastSet() && pshow->step < pshow->currset->counts)
			pshow->step = count_buffer;
		//if (setnum+1<set_tot && count_buffer < counts[setnum+1])
			//set_step=count_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}
		
void change_counts (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int count_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_counts));
		count_buffer = atoi(entry_buffer);
		if (count_buffer > 0)
		{
			pshow->currset->counts = count_buffer;
			//counts[setnum] = count_buffer;
		}
	}
}

// undo code
void do_undo_gtk(GtkWidget *widget)
{
	popFromStack(pshow, &pshow->undobr, &pshow->redobr);
	gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	return;
}
void do_redo_gtk(GtkWidget *widget)
{
	gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	return;
}
int pushToStack(struct undo_proto *unredo, struct undo_proto **stack_r)
{
	// Push node to stack
	struct undo_proto *stack;
	if (!unredo)
		return -1;
	stack = *stack_r;

	unredo->next = stack;
	*stack_r = unredo;
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

int pushPerfDel(struct undo_proto **stack_r, struct perf_proto *oldperf, 
		struct set_proto *firstset, int done)
{
	// Perf to be deleted, push node onto stack
	struct undo_proto *unredo;
	struct set_proto *last;
	int i;
	int index;
	int excode;


	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (!unredo)
	{
		return -1;
	}
	unredo->set_num = setnum;
	unredo->operation = 3;		// Performer removed
	unredo->ud.sperf = oldperf;	// store performer data 
	// store coordinates for performer
	unredo->coords = (struct coord_proto**)malloc(
			setnum*sizeof(struct coord_proto*));
	if (!unredo->coords)
		return -1;
	last = firstset;
	i = 0;
	while (last != NULL)
	{
		unredo->coords[i] = (struct coord_proto*)malloc(
				sizeof(struct coord_proto));
		if (!unredo->coords[i])
			return -1;
		unredo->coords[i]->x = last->coords[index]->x;
		unredo->coords[i]->y = last->coords[index]->y;
		i++;
		last = last->next;
	}

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

int pushCounts(struct undo_proto **stack_r, int counts)
{
	// push counts change onto stack
	struct undo_proto *unredo;
	int excode;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		unredo->operation = 6;		// counts changed
		unredo->ud.counts = counts;	// store counts
		unredo->done = 1;		// finished
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
	if (!*sourcebr_r)
		return 1;
	sourcebr = *sourcebr_r;
	dscard = *sourcebr_r;
	sourcebr = sourcebr->next;
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
	int done;
	int index;
	float x, y;			// coords for moving dot
	float xold, yold;

	// get operation
	sourcebr = *sourcebr_r;
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
			excode = pushPerfDel(&destbr, perfcurr, dshow->firstset, done);
			break;
		case 3:		// perf was deleted
			// re-add performer
			index = add_perf();
			if (index == -1)
				return -1;
			perfcurr = dshow->perfs[index];
			free(perfcurr->name);
			free(perfcurr->symbol);
			free(dshow->perfs[index]);
			dshow->perfs[index] = sourcebr->ud.sperf;
			sourcebr->ud.sperf->valid = 1;
			currset = dshow->firstset;
			i = 0;
			// re-add coordinates for performer
			coords = currset->coords;
			while (currset != NULL && coords[index] && sourcebr->coords[i])
			{
				coords[index]->x = sourcebr->coords[i]->x;
				coords[index]->y = sourcebr->coords[i]->y;
				free(sourcebr->coords[i]);
				i++;
				currset = currset->next;
				if (currset)
					coords = currset->coords;
			}
			// finish de-allocating if not done
			while (sourcebr->coords[i])
			{
				free(sourcebr->coords[i]);
				i++;
			}
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
			excode = pushCounts(&destbr, dshow->currset->counts);
			if (excode != -1)
				dshow->currset->counts = sourcebr->ud.counts;
			done = sourcePop(&sourcebr);
			break;
	}
	return 0;
}
