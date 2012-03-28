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
		if (perf_buffer<perfnum)
			perf_cur=perf_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void xperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int perf_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_x));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=160)
			perf[setnum][perf_cur][0] = perf_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void yperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int perf_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_y));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=85)
			perf[setnum][perf_cur][1] = perf_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	}
}

void add_perf (GtkWidget *widget)
{
	perfnum++;
	perf_cur=perfnum-1;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}
void delete_perf(GtkWidget *widget)
{
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
	if (!dshow->showname || !dshow->showinfo)
	{
		// internal allocation errors
		printf("char allocation error\n");
		return -1;
	}
	dshow->showname[0] = '\0';
	dshow->showinfo[0] = '\0';
	dshow->perfnum = perfs;

	// Make the list of performers
	excode = perf_construct(&dshow->perfs);
	if (excode == -1)
	{
		// performers not allocated
		printf("performer allocation error\n");
		return -1;
	}
	pcurr = dshow->perfs;
	pcurr->index = 0;
	for (i=1; i<perfs; i++)
	{
		// Build a linked list of performers
		excode = perf_construct(&plast);
		if (excode == -1)
		{
			// performers not allocated
			printf("performers allocation error\n");
			return -1;
		}
		pcurr->next = plast;
		pcurr = plast;
		pcurr->index = i;
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
	printf("coords = %g\n", coords[0]->x);
	if (excode == -1)
	{
		// coordinate allocation error
		printf("Coordinate allocation error\n");
		return -1;
	}

	*dshow_r = dshow;
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
		*sets_r = sets;

	return 0;
}


int newset_create(int index)
{
	// make a new set at a point right after index
	int i;
	int excode;

	// set structures
	struct set_proto *sets;
	struct set_proto *last;
	struct set_proto *curr;

	// coordinates
	struct coord_proto **coords;
	struct coord_proto **pcoords;

	last = pshow->firstset;
	curr = pshow->firstset;
	sets = 0;
	if (index == 0)
	{
		set_construct(&last, pshow->perfnum);
		pshow->firstset = last;
	}
	else
	{
		for (i=0; i<index && last != NULL; i++)
		{
			curr = last;
			last = last->next;
		}
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
	// excode = dot_construct(&dots);

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
	dots->next = 0;

	// pass by reference
	*dots_r = dots;

	return 0;
}



int set_coord(struct coord_proto *curr, float x, float y)
{
	// set coordinates from the coord struct
	curr->x = x;
	curr->y = y;
	return 0;
}


int retr_coord(struct coord_proto *curr, float *x, float *y)
{
	// retrieve coordinates from the coord struct
	*x = curr->x;
	*y = curr->y;

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
