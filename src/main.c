// Drill Writer Prototype Version 0.0.0.6
// main.c
#include <stdlib.h>

#include "drill.h"		// header file, with most global variables

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



int main (int argc, char *argv[])
{
	// specific set
	struct set_proto *currset;
	struct set_proto *prevset;
	// specific performer
	struct perf_proto *currperf;
	// coords
	struct coord_proto *coords; 
	struct coord_proto *prevcr;
	int excode;
	int i, j;		// loop vars
	float x, y;

	// Hardcode tempo in
	int tempo = 120;
	// set show as uninitialized
	pshow = 0;
	//dot_construct(&pshow);

	// Make a show with 5 performers
	pshow = 0;
	excode = show_construct(&pshow, 21);
	if (excode == -1)
	{
		printf("Allocation error\n");
		return -1;
	}
	currset = pshow->firstset;
	//pshow->perfnum = 15;
	currset->counts = 0;
	set_coord(pshow, 0, 32, 53);
	set_coord(pshow, 1, 36, 53);
	set_coord(pshow, 2, 40, 53);
	set_coord(pshow, 3, 34, 49);
	set_coord(pshow, 4, 38, 49);
	set_coord(pshow, 5, 36, 45);

	set_coord(pshow, 6, 32, 70);
	set_coord(pshow, 7, 36, 70);
	set_coord(pshow, 8, 40, 70);
	set_coord(pshow, 9, 34, 66);
	set_coord(pshow, 10, 38, 66);
	set_coord(pshow, 11, 36, 62);
	// add new set
	add_set();
	/*
	newset_create(currset);
	prevset = currset;
	currset = currset->next;
	*/
	currset = pshow->currset;
	prevset = pshow->prevset;
	currset->counts = 8;
	for (i=0; i<12; i++)
	{
		prevcr = prevset->coords[i];
		set_coord(pshow, i, prevcr->x+4, prevcr->y);
	}
	// add new set
	add_set();
	/*
	newset_create(currset);
	prevset = currset;
	currset = currset->next;
	*/
	currset = pshow->currset;
	prevset = pshow->prevset;
	currset->counts = 8;
	for (i=0; i<12; i++)
	{
		prevcr = prevset->coords[i];
		set_coord(pshow, i, prevcr->x, prevcr->y-10);
	}

	// Start at first set
	setnum = 0;
	pshow->currset = pshow->firstset;
	pshow->prevset = 0;

	perf_cur = 0;
	undo_destroy(&pshow->undobr, pshow);
	//foo();

	///*
	// Start up gtk
	startTk(argc, argv);
	// Create gtk interface
	buildIfacegtk();
	//printf("%s\n", perf_buf_x);


	// Run Main Loop
	gtk_main();
	// */
	
	show_destroy(&pshow);

	return 0;
}
	
