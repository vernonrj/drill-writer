// Functions that manipulate performers go here
//#include "drill.h"
#include "d_gtk.h"

// Side-to-Side relations
void change_ss_entry(GtkWidget *widget)
{
	return;
}
void toggle_ssYdRel(GtkWidget *widget)
{
	return;
}

void toggle_ssSide(GtkWidget *widget)
{
	return;
}

void toggle_ssYdLine(GtkWidget *widget)
{
	return;
}
// Front-to-back relations
void change_fb_entry(GtkWidget *widget)
{
	return;
}
void toggle_fbHashRel(GtkWidget *widget)
{
	return;
}

void toggle_fbFrontBack(GtkWidget *widget)
{
	return;
}

void toggle_HashSide(GtkWidget *widget)
{
	return;
}
void scale_form(float s_step)
{
	// basic expansion or contraction of form
	// center
	float cx, cy;
	// distance
	float distx, disty;
	// sign
	int signx, signy;
	// hypotenuse
	float hypo;
	// angle
	float angle;
	// selection
	struct select_proto *last;
	// coordinates
	struct coord_proto **coords;
	struct coord_proto *coord;
	// index
	int index;

	last = pshow->select;
	coords = pshow->currset->coords;
	cx = pshow->center->x;
	cy = pshow->center->y;
	undo_tclose();
	while (last != NULL)
	{
		// get coords for selected dot
		index = last->index;
		coord = coords[index];
		pushPerfmv(&pshow->undobr, index, coord->x, coord->y, 0);
		distx = cx - coord->x;
		disty = cy - coord->y;
		signx = distx < 0;
		signy = disty < 0;
		angle = atanf(disty / distx);
		if (angle < 0)
			angle = -1 * angle;
		hypo = powf(distx, 2) + powf(disty, 2);
		hypo = sqrtf(hypo);
		// don't contract if too close to center
		if (s_step > 0 || hypo > -1 * s_step)
		{
			// expand or contract
			distx = (hypo+s_step)*cosf(angle);
			disty = (hypo+s_step)*sinf(angle);
			if (signx)
				distx = -1 * distx;
			if (signy)
				disty = -1 * disty;
			// return to cartesian space
			coord->x = cx - distx;
			coord->y = cy - disty;
			// expand next dot
		}
		last = last->next;
	}
	return;
}


void rot_form(float s_step)
{
	// basic rotation around center
	// center
	float cx, cy;
	// distance
	float distx, disty;
	// sign
	int signx, signy;
	// hypotenuse
	float hypo;
	// angle
	float angle;
	// selection
	struct select_proto *last;
	// coordinates
	struct coord_proto **coords;
	struct coord_proto *coord;
	// index
	int index;
	last = pshow->select;
	coords = pshow->currset->coords;
	cx = pshow->center->x;
	cy = pshow->center->y;
	undo_tclose();
	while (last != NULL)
	{
		// get coords for selected dot
		index = last->index;
		coord = coords[index];
		distx = cx - coord->x;
		disty = cy - coord->y;
		pushPerfmv(&pshow->undobr, index, coord->x, coord->y, 0);
		if (distx != 0 || disty != 0)
		{
			signx = distx < 0;
			signy = disty < 0;
			angle = atanf(disty / distx);
			hypo = powf(distx, 2) + powf(disty, 2);
			hypo = sqrtf(hypo);
			if (distx < 0)
				angle = angle + M_PI;
			angle = angle + s_step;
			distx = hypo*cosf(angle);
			disty = hypo*sinf(angle);
			coord->x = cx - distx;
			coord->y = cy - disty;
		}
		last = last->next;
	}
	return;
}



void expand_form(GtkWidget *widget)
{
	// expand the form by 1 step
	scale_form(1);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void contract_form(GtkWidget *widget)
{
	// contract the form by 1 step
	scale_form(-1);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void rot_cw(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(M_PI/16);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void rot_countercw(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(-M_PI/16);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void rot_cw_small(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(M_PI/64);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void rot_countercw_small(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(-M_PI/64);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

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

int select_all_gtk (GtkWidget *widget)
{
	select_all();
	gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
}

int add_perf_gtk(GtkWidget *widget)
{
	int excode;
	excode = add_perf();
	gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	if (excode != -1)
		return 0;
	return excode;
}

int add_perf(void)
{
	// Add a performer
	// TODO: have an "add mode"
	// 	where dots are placed by clicking on the field
	// TODO: Keep track of valid dots and allocated dots
	
	int i;
	int index;
	int perfnum = pshow->perfnum;
	int found_dot = 0;
	// nodes used for reallocation
	struct perf_proto **newperfs;
	struct coord_proto **newcoords;
	// nodes used for checking
	struct perf_proto *perf;
	struct coord_proto *coord;
	struct set_proto *last;

	// find an open node
	for (i=0; i<perfnum && found_dot == 0; i++)
	{
		// check this performer
		perf = pshow->perfs[i];
		if (perf->valid == 0)
		{
			found_dot = 1;
			index = i;
		}
	}
	if (!found_dot)
	{
		// no open nodes found, 
		// have to make more nodes
		// in every set
		last = pshow->firstset;
		newperfs = (struct perf_proto**)malloc((perfnum+5)*sizeof(struct perf_proto*));
		for (i=0; i<perfnum; i++)
		{
			// copy old performers
			newperfs[i] = pshow->perfs[i];
		}
		// allocate the rest
		for (i=perfnum; i<perfnum+5; i++)
		{
			// make new performers
			newperfs[i] = (struct perf_proto*)malloc(sizeof(struct perf_proto));
			if (newperfs[i] == NULL)
				return -1;
			newperfs[i]->name = (char*)malloc(1*sizeof(char));
			newperfs[i]->symbol = (char*)malloc(1*sizeof(char));
			if (newperfs[i]->name == NULL || newperfs[i]->symbol == NULL)
				return -1;
			newperfs[i]->name[0] = '\0';
			newperfs[i]->symbol[0] = '\0';
			newperfs[i]->valid = 0;
		}
		// set new performers
		free(pshow->perfs);
		pshow->perfs = newperfs;
		// allocate coords for every set
		while (last != NULL)
		{
			newcoords = (struct coord_proto**)
				malloc((perfnum+5)*sizeof(struct coord_proto*));
			if (newcoords == NULL)
				return -1;
			for (i=0; i<perfnum; i++)
			{
				// copy old coords
				newcoords[i] = last->coords[i];
			}
			for (i=perfnum; i<perfnum+5; i++)
			{
				// copy the rest
				newcoords[i] = (struct coord_proto*)
					malloc(sizeof(struct coord_proto));
				if (newcoords[i] == NULL)
					return -1;
				newcoords[i]->x = 0;
				newcoords[i]->y = 0;
			}
			// hook up new coords
			free(last->coords);
			last->coords = newcoords;
			// go to next set
			last = last->next;
		}
		index = perfnum;
		pshow->perfnum = perfnum+5;
	}
	pshow->perfs[index]->valid = 1;
	pushPerfMk(&pshow->undobr, index, 1);
	return index;
}



void delete_perf_gtk(GtkWidget *widget)
{
	// Delete selected performers
	if (!playing)
	{
		delete_perf_selected();
		gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	}
}
void delete_perf_selected(void)
{
	int index;
	struct select_proto *last;
	struct perf_proto *perf;
	struct perf_proto **perfs;
	int done;
	last = pshow->select;
	perfs = pshow->perfs;
	while (last)
	{
		// set dots as invalid
		index = last->index;
		perf = perfs[index];
		if (last->next)
		{
			// more to delete
			done = 0;
		}
		else
		{
			// no more to delete
			done = 1;
		}
		pushPerfDel(&pshow->undobr, &perf, pshow->firstset, done);
		// TODO: eventually have to unlink perf struct for undo
		pshow->perfs[index] = perf;
		delete_perf(perf);
		perfs[index]->valid = 0;
		// go to next performer
		last = last->next;
	}
	select_discard();
	return;
}

void delete_perf(struct perf_proto *perf)
{
	// render performer invalid
	//struct perf_proto *perf;
	perf->valid = 2;
	return;
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
	//excode = coord_construct(&dshow->firstset->coords, perfs);
	//struct coord_proto **coords = dshow->firstset->coords;
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
	// Construct selection moments
	dshow->center = (struct coord_proto*)malloc(sizeof(struct coord_proto));
	dshow->center->x = 0;
	dshow->center->y = 0;
	// Set the selection to "none"
	dshow->select = 0;
	// Set the current set to opening set
	dshow->currset = dshow->firstset;
	dshow->prevset = 0;
	// init undo/redo to NULL
	dshow->undobr = 0;
	dshow->redobr = 0;
	time(&undo_timer);

	// start at the beginning of the set
	dshow->step = 0;

	*dshow_r = dshow;
	return 0;
}

int show_destroy(struct headset_proto **dshow_r)
{
	int i, j;
	int perfnum;
	int undop;
	int snum;
	// show
	struct headset_proto *dshow;
	// performers
	struct perf_proto **perfs;
	struct perf_proto *pcurr;
	// sets
	struct set_proto *setcurr;
	struct set_proto *setlast;
	// coords
	struct coord_proto **coords;
	// selects
	struct select_proto *select;
	struct select_proto *sellast;
	// undo/redo
	struct undo_proto *undcurr;
	struct undo_proto *undlast;
	// tempo
	struct tempo_proto *tcurr;
	struct tempo_proto *tlast;
	
	// get show
	dshow = *dshow_r;
	perfnum = dshow->perfnum;
	// free toplevel allocs
	free(dshow->center);
	free(dshow->showname);
	free(dshow->showinfo);

	// delete sets
	setcurr = dshow->firstset;
	snum = 0;
	while (setcurr != NULL)
	{
		setlast = setcurr;
		setcurr = setcurr->next;
		set_cldestroy(&setlast, perfnum);
		snum = snum + 1;
	}
	// delete performers
	perfs = pshow->perfs;
	for (i=0; i<perfnum; i++)
	{
		// delete performers
		pcurr = perfs[i];
		free(pcurr->name);
		free(pcurr->symbol);
		free(perfs[i]);
	}
	free(perfs);
	// delete selects
	select = dshow->select;
	while (select != NULL)
	{
		sellast = select->next;
		free(select);
		select = sellast;
	}
	// delete undo
	undo_destroy(&dshow->undobr, dshow);
	undo_destroy(&dshow->redobr, dshow);
	// delete tempo
	tlast = dshow->currtempo;
	if (tlast)
	{
		while (tlast->prev)
			tlast = tlast->prev;
		while (tlast)
		{
			tcurr = tlast->next;
			free(tlast);
			tlast = tcurr;
		}
	}
	// delete last piece
	free(dshow);
	*dshow_r = 0;
	return 0;
}


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


// Tempo storage
int tempo_construct(struct tempo_proto **tempo_r, int anchorpoint)
{
	// Add a new tempo at anchorpoint
	struct tempo_proto *stempo;
	stempo = (struct tempo_proto*) malloc(sizeof(struct tempo_proto));
	if (stempo == NULL)
	{
		// allocation error
		return -1;
	}
	stempo->prev = 0;
	stempo->next = 0;
	stempo->tempo = 120;
	stempo->anchorpoint = anchorpoint;

	// pass by reference back
	*tempo_r = stempo;

	return 0;
}



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
	// make sure not classified as midset
	sets->midset = 0;
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
	int mid = 0;
	float x, y;

	// set structures
	struct set_proto *sets;
	struct set_proto *last;
	struct set_proto *fset;
	struct set_proto *nextset;

	// coordinates
	struct coord_proto **coords;
	struct coord_proto **pcoords;
	struct coord_proto **ncoords;

	sets = 0;
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
			nextset->counts = nextset->counts - pshow->step;
			printf("nextset = %i\n", nextset->counts);
			last->counts = pshow->step;
			printf("current set = %i\n", last->counts);
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



int set_destroy(void)
{
	// destroy current set
	int i;
	int perfnum;
	struct set_proto *last;
	struct set_proto *prevset;
	struct set_proto *before;
	int excode;

	last = pshow->currset;
	prevset = pshow->prevset;
	perfnum = pshow->perfnum;

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
	return;
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
	dots->symbol[0] = '\0';
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
void update_sel_center(void)
{
	// update the center of the form based on dot selection
	// TODO: Use heaps to keep track of max, min values?
	int index;
	int selnum = 0;
	float cx, cy;
	float minx, miny, maxx, maxy;
	struct select_proto *last;
	struct coord_proto **coords;
	struct coord_proto *coord;

	cx = 0;
	cy = 0;
	minx = -1;
	miny = -1;
	maxx = 0;
	maxy = 0;
	last = pshow->select;
	coords = pshow->currset->coords;
	while (last)
	{
		// get coordinates for selected dot
		index = last->index;
		coord = coords[index];
		cx = cx + coord->x;
		cy = cy + coord->y;
		selnum++;
		last = last->next;
	}
	if (selnum == 0)
	{
		// no selections
		cx = 0;
		cy = 0;
	}
	else
	{
		// have selections
		cx = cx / selnum;
		cy = cy / selnum;
	}
	// store
	pshow->center->x = cx;
	pshow->center->y = cy;
	pshow->selnum = selnum;

	return;
}


void add_sel_center(struct coord_proto *coord)
{
	// add a selection to center weight
	int index;
	int selnum;
	float x, y;
	
	// open up weight
	selnum = pshow->selnum;
	x = pshow->center->x * selnum;
	y = pshow->center->y * selnum;

	// add new dot to center
	x = x + coord->x;
	y = y + coord->y;
	selnum++;

	// take mean of new set
	pshow->center->x = x / selnum;
	pshow->center->y = y / selnum;
	pshow->selnum = selnum;

	return;
}



void rem_sel_center(struct coord_proto *coord)
{
	// remove a selection from center weight
	int index;
	int selnum;
	float x, y;

	// open up weight
	selnum = pshow->selnum;
	if (selnum == 0)
		return;
	else if (selnum == 1)
	{
		pshow->selnum = 0;
		pshow->center->x = 0;
		pshow->center->y = 0;
		return;
	}
	x = pshow->center->x * selnum;
	y = pshow->center->y * selnum;

	// remove dot from center
	x = x - coord->x;
	y = y - coord->y;
	selnum--;

	// take mean of new set if not 0
	pshow->center->x = x / selnum;
	pshow->center->y = y / selnum;
	pshow->selnum = selnum;
	
	return;
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
	pshow->selnum = 0;
	update_sel_center();
	return;
}

int select_add(int index)
{
	// add a selection if it's not selected;
	// remove a selection if it is selected
	
	struct select_proto *selects;
	struct select_proto *last;
	struct select_proto *curr;
	struct coord_proto *coord;
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
		// update selection center
		add_sel_center(pshow->currset->coords[index]);
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
				rem_sel_center(pshow->currset->coords[index]);
			}
			else if (last->index > index)
			{
				// no match found
				// exit to create new node
				loop_done = 1;
				matched = 0;
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
		add_sel_center(pshow->currset->coords[index]);
	}
	return;
}

int select_all(void)
{
	// select all dots
	int i;
	int perfnum;
	struct perf_proto **perfs;
	select_discard();
	perfnum = pshow->perfnum;
	perfs = pshow->perfs;
	for (i=0; i<perfnum; i++)
	{
		if (perfs[i]->valid == 1)
		{
			// performer is valid. Add
			select_add(i);
		}
	}
	return 0;
}



