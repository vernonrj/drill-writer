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
int pushToStack(struct undo_proto *unredo, struct undo_proto **stack_r)
{
	// Push node to stack
	return 0;
}

int pushSetMk(struct undo_proto **stack_r)
{
	// New set created, push set num onto undo stack
	struct undo_proto *unredo;
	struct undo_proto *stack;
	stack = *stack_r;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		unredo->operation = 0;	// set to be created
		unredo->done = 1;
		pushToStack(unredo, stack_r);

		return 0;
	}
	else
		return -1;
}

int pushSetDel(struct undo_proto **stack_r, struct set_proto *oldset)
{
	// Set to be deleted, push current set onto stack
	struct undo_proto *unredo;
	struct undo_proto *stack;
	stack = *stack_r;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		unredo->operation = 1;		// set to be deleted
		unredo->ud.set = oldset;	// store set
		unredo->done = 1;		// finished
		pushToStack(unredo, stack_r);	// push

		return 0;
	}
	else
		return -1;
}

int pushPerfMk(struct undo_proto **stack_r, int index, int done)
{
	// Perf to be added, push index onto stack
	struct undo_proto *unredo;
	struct undo_proto *stack;
	stack = *stack_r;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		unredo->operation = 2;		// Performer adding
		unredo->ud.pindex = index;	// store perf index
		unredo->done = done;		// check if finished
		pushToStack(unredo, stack_r);	// push

		return 0;
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
	struct undo_proto *stack;
	struct set_proto *last;
	int i;
	int index;

	stack = *stack_r;

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
	pushToStack(unredo, stack_r);	// push

	return 0;
}

int pushPerfmv(struct undo_proto **stack_r, int index, float x, float y, int done)
{
	// push the relative movement of perf onto stack
	struct undo_proto *unredo;
	struct undo_proto *stack;
	stack = *stack_r;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		unredo->operation = 4;		// Performer moved
		unredo->ud.pindex = index;	// store perf index
		unredo->x = x;			// store coords
		unredo->y = y;			// store coords
		unredo->done = done;		// check if finished
		pushToStack(unredo, stack_r);	// push

		return 0;
	}
	else
		return -1;
	return 0;
}


int pushTempo(struct undo_proto **stack_r, int tempo)
{
	// push tempo change onto stack
	struct undo_proto *unredo;
	struct undo_proto *stack;
	stack = *stack_r;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		unredo->operation = 5;		// tempo changed
		unredo->ud.tempo = tempo;	// store tempo
		unredo->done = 1;		// finished
		pushToStack(unredo, stack_r);	// push

		return 0;
	}
	else
		return -1;
	return 0;
}

int pushCounts(struct undo_proto **stack_r, int counts)
{
	// push counts change onto stack
	struct undo_proto *unredo;
	struct undo_proto *stack;
	stack = *stack_r;

	unredo = (struct undo_proto*)malloc(sizeof(struct undo_proto));
	if (unredo)
	{
		unredo->set_num = setnum;
		unredo->operation = 6;		// counts changed
		unredo->ud.counts = counts;	// store counts
		unredo->done = 1;		// finished
		pushToStack(unredo, stack_r);	// push

		return 0;
	}
	else
		return -1;
	return 0;
}
