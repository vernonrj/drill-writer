// Drill Writer Pre-Alpha Version 0.0.6
// main.c
#include <stdlib.h>

#include "drillwriter.h"		// header file, with most global variables
#include "dr_forms.h"
#include "dr_select.h"
#include "coords.h"

int show_construct(headset_t **dshow_r, int perfs)
{
	// Create a show based on information given
	int i;
	int excode;
	headset_t *dshow;
	// Performers
	perf_t *pcurr;
	// Sets
	set_t *setcurr;

	dshow = (headset_t*) malloc(sizeof(headset_t));
	// initialize filename to empty
	pstate.filename = 0;
	if (dshow == NULL)
	{
		// allocation error
		printf("headset allocation failure\n");
		return -1;
	}
	// make some info
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
	dshow->perfs = (perf_t**) malloc(perfs * sizeof(perf_t*));
	if (!dshow->perfs)
		return -1;
	for (i=0; i<perfs; i++)
	{
		// Build a linked list of performers
		pcurr = perf_construct();
		if (pcurr == NULL)
			return -1;
		pcurr->index = i;
		dshow->perfs[i] = pcurr;
	}

	// Make the first set
	setcurr = 0;
	dshow->sets = set_container_construct(perfs);
	if (!dshow->sets)
	{
		// sets not allocated
		printf("Set allocation error\n");
		return -1;
	}
	setcurr = dshow->sets->currset;
	dshow->sets->firstset = setcurr;

	// Construct tempo
	excode = tempo_construct(&dshow->currtempo, 0);
	if (excode == -1)
	{
		// tempo allocation error
		return -1;
	}
	// Construct selection moments
	pstate.center = coord_construct();
	// Set the selection to "none"
	pstate.select = select_create_with_size(dshow->perfnum, 0);
	// Initialize toplevel groups to NULL
	dshow->topgroups = NULL;
	dshow->topforms = NULL;
	// Set the current set to opening set
	dshow->sets->currset = dshow->sets->firstset;
	// init undo/redo to NULL
	pstate.undobr = 0;
	pstate.redobr = 0;
	time(&pstate.undo_timer);

	// start at the beginning of the set
	pstate.curr_step = 0;

	pstate.setnum = 0;

	*dshow_r = dshow;
	return 0;
}



int show_destroy(headset_t **dshow_r)
{
	int i;
	int perfnum;
	int snum;
	// show
	headset_t *dshow;
	// performers
	perf_t **perfs;
	// sets
	set_t *setcurr;
	set_t *setlast;
	// selects
	//select_t *select;
	//select_t *sellast;
	// tempo
	tempo_t *tcurr;
	tempo_t *tlast;
	
	// get show
	dshow = *dshow_r;
	perfnum = dshow->perfnum;
	// free toplevel allocs
	free(pstate.center);
	free(dshow->showname);
	free(dshow->showinfo);

	// delete sets
	setcurr = dshow->sets->firstset;
	snum = 0;
	while (setcurr != NULL)
	{
		//printf("%i\n", snum);
		setlast = setcurr;
		//setcurr = setcurr->next;
		setcurr = set_get_next(pshow->sets, snum);
		set_cldestroy(&setlast, perfnum);
		snum = snum + 1;
	}
	free(dshow->sets);
	// delete performers
	perfs = pshow->perfs;
	for (i=0; i<perfnum; i++)
	{
		// delete performers
		perf_destroy(&perfs[i]);
	}
	free(perfs);
	// delete selects
	pstate.select = select_destroy(pstate.select);
	/*
	select = pstate.select;
	while (select != NULL)
	{
		//sellast = select->next;
		sellast = select_get_next(select);
		free(select);
		select = sellast;
	}
	*/
	// delete undo
	undo_destroy(&pstate.undobr, dshow);
	undo_destroy(&pstate.redobr, dshow);
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


int drillwriter_main (int argc, char *argv[])
{
	// specific set
	set_t *currset;
	// coords
	int excode;
	int i;		// loop vars
	form_child_t *form = NULL;
	select_t *select = NULL;

	mouse_currentMode = SELECTONE;
	// set show as uninitialized
	pshow = 0;

	// Make a show with 21 performers
	pshow = 0;
	excode = show_construct(&pshow, 21);

	if (excode == -1)
	{
		printf("Allocation error\n");
		return -1;
	}
	if (argc > 1)
		open_file(argv[1]);
	else
	{
		currset = pshow->sets->firstset;
		currset->counts = 0;

		//coords_set_coord(pshow, 0, 32, 53);
		//coords_set_coord(pshow, 1, 36, 53);
		//coords_set_coord(pshow, 2, 40, 53);
		//coords_set_coord(pshow, 3, 34, 49);
		//coords_set_coord(pshow, 4, 38, 49);
		//coords_set_coord(pshow, 5, 36, 45);

		for (i=0; i<12; i++)
			perf_set_valid(pshow->perfs[i]);

		coords_set(currset->coords[0], 32, 53);
		coords_set(currset->coords[1], 36, 53);
		coords_set(currset->coords[2], 40, 53);
		coords_set(currset->coords[3], 34, 49);
		coords_set(currset->coords[4], 38, 49);
		coords_set(currset->coords[5], 36, 45);
		//*
		select = select_create();
		for(i=0; i<6; i++)
		{
			select_add_dot(select, i);
		}
		form = form_build_line(NULL, select);
		pshow->topforms = form_parent_add_form(pshow->topforms, form, pstate.setnum);
		//free(form->name);
		//form->name = (char*)malloc(6*sizeof(char));
		//snprintf(form->name, 6, "Form1");
		form_set_endpoint(form, 0, 0, 32, 53, 0.0);
		form_set_endpoint(form, 0, 0, 50, 53, 0.0);
		form_child_set_name(form, "Form1");
		form_add_to_current_set(form);
		//*/

		//coords_set_coord(pshow, 6, 32, 70);
		//coords_set_coord(pshow, 7, 36, 70);
		//coords_set_coord(pshow, 8, 40, 70);
		//coords_set_coord(pshow, 9, 34, 66);
		//coords_set_coord(pshow, 10, 38, 66);
		//coords_set_coord(pshow, 11, 36, 62);

		coords_set(currset->coords[6], 32, 70);
		coords_set(currset->coords[7], 36, 70);
		coords_set(currset->coords[8], 40, 70);
		coords_set(currset->coords[9], 34, 66);
		coords_set(currset->coords[10], 38, 66);
		coords_set(currset->coords[11], 36, 62);
		//*
		select_clear(select);
		for(i=6; i<12; i++)
		{
			//select = select_add_index(select, i, false);
			select_add_dot(select, i);
		}
		form = form_build_line(NULL, select);
		free(select);
		pshow->topforms = form_parent_add_form(pshow->topforms, form, pstate.setnum);
		//free(form->name);
		//form->name = (char*)malloc(6*sizeof(char));
		//snprintf(form->name, 6, "Form2");
		form_set_endpoint(form, 0, 0, 20, 70, 0.0);
		form_set_endpoint(form, 0, 0, 36, 62, 0.0);
		form_child_set_name(form, "Form2");
		form_add_to_current_set(form);
		//*/

		// add new set
		//*
		//add_set();
		set_add_after_current();
		currset = pshow->sets->currset;
		form = currset->forms;
		while (form)
		{
			form_movexy(form, 4, 0);
			//form = form->next;
			form = form_child_get_next(form);
		}
		/*
		prevset = set_get_prev(pshow->sets, pstate.setnum);
		currset->counts = 8;
		for (i=0; i<12; i++)
		{
			prevcr = prevset->coords[i];
			coords_set_coord(pshow, i, prevcr->x+4, prevcr->y);
		}
		*/
		///*
		// add new set
		//add_set();
		set_add_after_current();
		currset = pshow->sets->currset;
		form = currset->forms;
		while (form)
		{
			form_movexy(form, 0, -10);
			//form = form->next;
			form = form_child_get_next(form);
		}
		/*
		prevset = set_get_prev(pshow->sets, pstate.setnum);
		currset->counts = 8;
		for (i=0; i<12; i++)
		{
			prevcr = prevset->coords[i];
			coords_set_coord(pshow, i, prevcr->x, prevcr->y-10);
		}
		*/
	}

	// Start at first set
	pstate.setnum = 0;
	pshow->sets->currset = pshow->sets->firstset;

	perf_cur = 0;
	//undo_destroy(&pstate.undobr, pshow);

	
	//menuIface();
	//show_destroy(&pshow);

	return 0;
}

