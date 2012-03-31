// Functions that manipulate performers go here
#include "drill.h"
void goto_perf (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int perf_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer<pshow->perfnum)
			perf_cur=perf_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void xperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	float perf_buffer;
	struct coord_proto *coords = pshow->currset->coords[perf_cur];
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_x));
		perf_buffer = atof(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=160)
		{
			set_coord(coords, perf_buffer, coords->y);
			//perf[setnum][perf_cur][0] = perf_buffer;
		}
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void yperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	float perf_buffer;
	struct coord_proto *coords = pshow->currset->coords[perf_cur];
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_y));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=85)
		{
			set_coord(coords, coords->x, perf_buffer);
			//perf[setnum][perf_cur][1] = perf_buffer;
		}
		gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	}
}

void add_perf (GtkWidget *widget)
{
	/*
	perfnum++;
	perf_cur=perfnum-1;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	*/
}
void delete_perf(GtkWidget *widget)
{
	/*
	int i, j;
	for (i=0; i<set_tot; i++)
	{
		for (j=perf_cur; j<perfnum; j++)
		{
			perf[i][j][0] = perf[i][j+1][0];
			perf[i][j][1] = perf[i][j+1][1];
		}
	}
	perfnum--;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	*/
}

int show_construct(struct headset_proto **dshow_r, int perfs)
{
	// Create a show based on information given
	int i, j;
	int excode;
	struct headset_proto *dshow;
	// Performers
	struct perf_proto *pcurr;
	struct perf_proto *plast;
	// Sets
	struct set_proto *setcurr;
	struct set_proto *setlast;
	// coords

	dshow = (struct headset_proto*) malloc(sizeof(struct headset_proto));
	if (dshow == NULL)
	{
		// allocation error
		printf("headset allocation failure\n");
		return -1;
	}
	dshow->showname = (char*) malloc(sizeof(char));
	dshow->showinfo = (char*) malloc(sizeof(char));
	dshow->perfnum = perfs;
	if (!dshow->showname || !dshow->showinfo)
	{
		// internal allocation errors
		printf("char allocation error\n");
		return -1;
	}
	dshow->showname[0] = '\0';
	dshow->showinfo[0] = '\0';

	// Make the list of performers
	dshow->perfs = (struct perf_proto**) malloc(perfs * sizeof(struct perf_proto*));
	if (!dshow->perfs)
		return -1;
	for (i=0; i<perfs; i++)
	{
		// Build a linked list of performers
		pcurr = dshow->perfs[i];
		excode = perf_construct(&pcurr);
		if (excode == -1)
		{
			// performers not allocated
			printf("performers allocation error\n");
			return -1;
		}
		pcurr->index = i;
		dshow->perfs[i] = pcurr;
	}

	// Make the setlist
	// Make the first set
	setcurr = 0;
	excode = set_construct(&setcurr, perfs);
	if (excode == -1)
	{
		// sets not allocated
		printf("Set allocation error\n");
		return -1;
	}
	dshow->firstset = setcurr;

	// Make the index of dots for the first set
	excode = coord_construct(&dshow->firstset->coords, perfs);
	struct coord_proto **coords = dshow->firstset->coords;
	//printf("coords = %g\n", coords[0]->x);
	if (excode == -1)
	{
		// coordinate allocation error
		printf("Coordinate allocation error\n");
		return -1;
	}
	// Construct tempo
	excode = tempo_construct(&dshow->currtempo, 0);
	if (excode == -1)
	{
		// tempo allocation error
		return -1;
	}
	// Set the selection to "none"
	dshow->select = 0;
	// Set the current set to opening set
	dshow->currset = dshow->firstset;
	dshow->prevset = 0;

	// start at the beginning of the set
	dshow->step = 0;

	*dshow_r = dshow;
	return 0;
}

// Tempo storage
int tempo_construct(struct tempo_proto **tempo_r, int anchorpoint)
{
	// Add a new tempo at anchorpoint
	struct tempo_proto *tempo;
	tempo = (struct tempo_proto*) malloc(sizeof(struct tempo_proto));
	if (tempo == NULL)
	{
		// allocation error
		return -1;
	}
	tempo->prev = 0;
	tempo->next = 0;
	tempo->tempo = 120;
	tempo->anchorpoint = anchorpoint;

	// pass by reference back
	*tempo_r = tempo;

	return 0;
}



// dot storage
int set_construct(struct set_proto **sets_r, int perfs)
{
	// Build storage for set
	int i;
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
	// make coordinate system
	excode = coord_construct(&sets->coords, perfs);


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

	// set structures
	struct set_proto *sets;
	struct set_proto *last;
	//struct set_proto *curr;

	// coordinates
	struct coord_proto **coords;
	struct coord_proto **pcoords;

	sets = 0;
	if (curr == 0)
	{
		// place at beginning of list
		set_construct(&last, pshow->perfnum);
		pshow->firstset = last;
	}
	else
	{
		/*
		for (i=0; i<index && last != NULL; i++)
		{
			curr = last;
			last = last->next;
		}
		*/
		// allocate new set
		excode = set_construct(&sets, pshow->perfnum);
		if (excode == -1)
			return -1;
		pcoords = curr->coords;
		coords = sets->coords;
		for (i=0; i<pshow->perfnum; i++)
		{
			// Copy previous dots into new set
			coords[i]->x = pcoords[i]->x;
			coords[i]->y = pcoords[i]->y;
		}
		// link
		sets->next = curr->next;
		curr->next = sets;
	}

	return 0;
}



int coord_construct(struct coord_proto *** coords_r, int perfs)
{
	// Build the coordinates for the set
	
	// loop var
	int i;
	// 2d coordinate array
	struct coord_proto **coords;
	// Piece of coordinate array
	struct coord_proto *ccurr;

	// Make the root pointer
	coords = (struct coord_proto**) malloc(perfs*sizeof(struct coord_proto*));
	if (!coords)
	{
		// coordinate allocation error
		printf("Error: coords mapped to 0x%x\n", coords);
		return -1;
	}

	for (i=0; i<perfs; i++)
	{
		// Make the first set of dots
		ccurr = (struct coord_proto*) malloc(sizeof(struct coord_proto));
		coords[i] = ccurr;
		ccurr->x = 0;
		ccurr->y = 0;
	}

	// link to reference
	*coords_r = coords;

	return 0;
}


// performer storage
int perf_construct(struct perf_proto **dots_r)
{
	// Build storage for performer
	// give argument:
	// excode = perf_construct(&dots);
	// piece of performer array
	struct perf_proto *dots;

	dots = (struct perf_proto*) malloc(sizeof(struct perf_proto));
	if (dots == NULL)
	{
		// allocation error
		return -1;
	}
	// allocate values inside struct
	dots->name = (char*) malloc(1 * sizeof(char));
	dots->symbol = (char*) malloc(1 * sizeof(char));
	if (dots->name == NULL || dots->symbol == NULL)
	{
		// allocation error
		return -1;
	}
	dots->name[0] = '\0';
	dots->symbol = '\0';
	dots->valid = 0;

	// pass by reference
	*dots_r = dots;

	return 0;
}



int allow_perf(int index)
{
	// allow a performer to be drawn on the field
	return -1;
}

int set_coord(struct coord_proto *curr, float x, float y)
{
	// set coordinates from the coord struct
	curr->x = x;
	curr->y = y;
	return 0;
}


int set_coord_valid(struct coord_proto **curr, int index, float x, float y)
{
	// set coordinates and set performer valid
	curr[index]->x = x;
	curr[index]->y = y;
	pshow->perfs[index]->valid = 1;
	return 0;
}

int retr_coord(struct coord_proto *curr, float *x, float *y)
{
	// retrieve coordinates from the coord struct
	*x = curr->x;
	*y = curr->y;

	return 0;
}


int retr_midset(struct set_proto *currset, int index, float *x_r, float *y_r)
{
	// retrieve midset coordinates from set struct
	struct set_proto *last;
	float xcurr, ycurr;
	float xnext, ynext;
	float xbias, ybias;
	int cstep;
	int countnum;

	struct set_proto *nextset;

	retr_coord(currset->coords[index], &xcurr, &ycurr);
	if (currset->next != NULL)
	{
		// not last set,
		// need to check to see if midset should be found
		nextset = currset->next;
		retr_coord(nextset->coords[index], &xnext, &ynext);
		cstep = pshow->step;
		if (cstep != 0)
		{
			// there is a bias that should be calculated
			countnum = nextset->counts;
			xbias = (xnext - xcurr) / countnum;
			ybias = (ynext - ycurr) / countnum;
			xbias = xbias * cstep;
			ybias = ybias * cstep;
			// find new locations based on bias
			xcurr = xcurr + xbias;
			ycurr = ycurr + ybias;
		}
	}

	// store reference
	*x_r = xcurr;
	*y_r = ycurr;

	return 0;
}



int dot_realloc(struct perf_proto ***dots_r, int oldsize, int newsize)
{
	// Change number of performers
	return 0;
}



int dot_new_set(struct perf_proto ***dots_r, int setnum)
{
	// Make a new set after setnum
	return 0;
}



int dot_destroy(struct perf_proto ***dots_r, int size)
{
	// Destroy list
	int i;
	struct perf_proto **dots;

	dots = *dots_r;

	for (i=0; i<size; i++)
	{
		free(dots[i]->name);
		free(dots[i]->symbol);
		free(dots[i]);
	}
	free(dots);
	*dots_r = 0;

	return 0;
}


void select_discard(void)
{
	// remove all selections from selection list
	struct select_proto *selects;
	struct select_proto *last;
	if (pshow->select != 0)
	{
		selects = pshow->select;
		last = pshow->select;
		while (selects != NULL)
		{
			// remove
			last = selects->next;
			free(selects);
			selects = last;
		}
		pshow->select = 0;
	}
	return;
}

int select_add(int index)
{
	// add a selection if it's not selected;
	// remove a selection if it is selected
	
	struct select_proto *selects;
	struct select_proto *last;
	struct select_proto *curr;
	int matched = 0;
	int loop_done = 0;

	if (!pshow->select)
	{
		// no selection yet
		curr = (struct select_proto*)malloc(sizeof(struct select_proto));
		if (!curr)
			return -1;
		curr->next = 0;
		curr->index = index;
		pshow->select = curr;
	}
	else
	{
		// have some selections; 
		// add inorder or remove if already selected
		last = pshow->select;
		selects = 0;
		while (loop_done == 0)
		{
			if (last->index == index)
			{
				// found a match, toggle off
				if (selects == NULL)
				{
					// match is first node
					pshow->select = last->next;
					free(last);
				}
				else
				{
					// match is not first node, remove
					selects->next = last->next;
					free(last);
				}
				loop_done = 1;
				matched = 1;
			}
			else if (last->index > index)
			{
				// no match found
				// exit to create new node
				loop_done = 1;
			}
			else
			{
				// continue searching
				selects = last;
				last = last->next;
				if (last == NULL)
					loop_done = 1;
			}
		}
		if (matched == 0)
		{
			// create a new node
			curr = (struct select_proto*)malloc(sizeof(struct select_proto));
			if (!curr)
				return -1;
			curr->index = index;
			if (selects == NULL)
			{
				// insert at beginning
				curr->next = pshow->select;
				pshow->select = curr;
			}
			else
			{
				// insert in the middle
				curr->next = selects->next;
				selects->next = curr;
			}
		}
	}
	return;
}

