// Functions that change sets go here
//#include "drill.h"
#include "d_gtk.h"
void zoom_amnt(float x, float y)
{
	if (x == 0 && y == 0)
	{
		// zoom to 100%
		zoom_x = (float)scrolled_window->allocation.width;
		zoom_y = (float)scrolled_window->allocation.height;
		//zoom_x = width;
		//zoom_y = height;
	}
	else
	{
		zoom_x = zoom_x + x;
		zoom_y = zoom_y + y;
	}
	return;
}

gboolean zoom_scroll(GtkWidget *widget, GdkEventScroll *event)
{
	// handle zoom events
	// propagate everything except control modifier
	if (event->state == 0)
		return FALSE;
	else if (event->state != 4)
		return FALSE;
	if (event->direction == GDK_SCROLL_UP)
	{
		// zoom in
		zoom_amnt(10, 10);
		//zoom_x = zoom_x + 10;
		//zoom_y = zoom_y + 10;
		gtk_widget_set_size_request(widget, zoom_x, zoom_y);
	}
	else if (event->direction == GDK_SCROLL_DOWN)
	{
		// zoom out
		zoom_amnt(-10, -10);
		//zoom_x = zoom_x - 10;
		//zoom_y = zoom_y - 10;
		gtk_widget_set_size_request(widget, zoom_x, zoom_y);
	}
	return TRUE;
}

void zoom_in(GtkWidget *widget)
{
	// zoom in
	zoom_amnt(10, 10);
	gtk_widget_set_size_request(drill, zoom_x, zoom_y);
}

void zoom_out(GtkWidget *widget)
{
	// zoom out
	zoom_amnt(-10, -10);
	gtk_widget_set_size_request(drill, zoom_x, zoom_y);
}

void zoom_standard(GtkWidget *widget)
{
	// zoom to 100%
	zoom_amnt(0, 0);
	gtk_widget_set_size_request(drill, zoom_x, zoom_y);
}



void add_set_gtk(GtkWidget *widget)
{
	// add a set from a Gtk entry
	if (!playing)
	{
		add_set();
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}

void delete_set_gtk(GtkWidget *widget)
{
	// delete a set from a Gtk entry
	if (!playing)
	{
		delete_set();
		do_field = 0;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}



void set_first_gtk(GtkWidget *widget)
{
	// Move to first set, do gtk stuff
	if (!playing)
	{
		set_first();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}

void set_last_gtk(GtkWidget *widget)
{
	// Go to last set from Gtk button
	if (!playing)
	{
		set_last();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);

	}
	return;
}

void set_next_gtk(GtkWidget *widget)
{
	// Go to next set from Gtk button
	if (!playing)
	{
		set_next();
		do_field=0;
		//gdk_drawable_get_size(widget->window, &width, &height);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}


void set_next_count_gtk(GtkWidget *widget)
{
	// Go to next count from gtk button
	if (!playing)
	{
		set_next_count();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}

void set_prev_count_gtk(GtkWidget *widget)
{
	// Go to next count from gtk button
	if (!playing)
	{
		set_prev_count();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}
				
void set_prev_gtk(GtkWidget *widget)
{
	// Go to previous set from Gtk button
	if (!playing)
	{
		set_prev();
		do_field=0;	// don't need to draw field
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}



void set_set_name_gtk(GtkWidget *widget)
{
	// Set the name of the set
	// Name hard-limited to 4 chars currently
	// name is currently taken from entry_sets
	const gchar *entry_buffer;
	int set_buffer;
	struct set_proto *setcurr = pshow->currset;
	int size;

	if (!playing)
	{	
		entry_buffer = gtk_entry_get_text(GTK_ENTRY(entry_sets));
		size = strlen(entry_buffer);
		if (size < 5)
		{
			// set the name of the current set
			free(setcurr->name);
			setcurr->name = (char*)malloc(size*sizeof(char)+1);
			strcpy(setcurr->name, entry_buffer);
		}
		do_field = 0;	// don't need to redraw field yet
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}


void goto_set_gtk (GtkWidget *widget)
{	// Go to set specified in entry_sets
	const gchar *entry_buffer;
	int set_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_sets));
		set_buffer = atoi(entry_buffer);
		goto_set(set_buffer);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void goto_set(int set_buffer)
{
	int i = 0;
	struct set_proto *last = pshow->firstset;
	struct set_proto *curr = 0;
	{
		for (i=0; i<set_buffer && last != NULL; i++)
		{
			// go to current set
			curr = last;
			last = last->next;
			i++;
		}
		/*
		if (set_buffer<set_tot)
			setnum=set_buffer;
		*/
		if (last != NULL)
		{
			pshow->currset = last;
			pshow->prevset = curr;
			setnum = i;
		}
	}
	return;
}


void change_tempo(int tempo, struct tempo_proto **currtempo_r)
{
	struct tempo_proto *prevtempo;
	struct tempo_proto *currtempo;
	struct tempo_proto *nexttempo;
	struct tempo_proto *stempo;
	struct tempo_proto *othertempo;
	currtempo = *currtempo_r;
	if (tempo >= 30 && tempo <= 250)
	{
		// Tempo is valid. Make changes thusly
		pushTempo(&pshow->undobr, currtempo->tempo);
		if (currtempo->anchorpoint == setnum)
		{
			// changing an existing tempo
			currtempo->tempo = tempo;
		}
		else
		{
			// making a new node
			stempo = (struct tempo_proto*) malloc(sizeof(struct tempo_proto));
			// store data to new node
			stempo->tempo = tempo;
			stempo->anchorpoint = setnum;
			// link new node
			stempo->next = currtempo->next;
			currtempo->next = stempo;
			stempo->prev = currtempo;
			currtempo = currtempo->next;
		}
		// Now check to see if node is unneccesary
		// Check to see if node can be deleted
		prevtempo = currtempo->prev;
		if (prevtempo)
		{
			if (prevtempo->tempo == tempo)
			{
				// current node is redundant, can be deleted
				prevtempo->next = currtempo->next;
				nexttempo = currtempo->next;
				if (nexttempo)
				{
					// link previously
					nexttempo->prev = prevtempo;
				}
				//pshow->currtempo = prevtempo;
				free(currtempo);
				currtempo = prevtempo;
			}
		}
		nexttempo = currtempo->next;
		if (nexttempo)
		{
			if (nexttempo->tempo == tempo)
			{
				// next node is redundant, can be deleted
				currtempo->next = nexttempo->next;
				othertempo = nexttempo->next;
				if (othertempo)
				{
					// link next to prv
					othertempo->prev = currtempo;
				}
				free(nexttempo);
			}
		}
	}
	*currtempo_r = currtempo;
	return;
}


void change_tempo_gtk (GtkWidget *widget)
{
	// Change the tempo internally
	// TODO: Non-global tempo
	struct tempo_proto *currtempo;
	struct tempo_proto *stempo;
	struct tempo_proto *nexttempo;
	const gchar *entry_buffer;
	int tmpo;

	entry_buffer = gtk_entry_get_text(GTK_ENTRY(entry_tempo));
	tmpo = atoi(entry_buffer);
	currtempo = pshow->currtempo;
	change_tempo(tmpo, &currtempo);
	pshow->currtempo = currtempo;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);

	return;
}


int isLastSet(void)
{
	// check to see if we're at the last set
	if (pshow->currset->next == NULL)
	{
		// at the last set
		return 1;
	}
	else
	{
		// not at the last set
		return 0;
	}
	return 0;
}

int isFirstSet(void)
{
	// check to see if we're at the first set
	if (pshow->currset == pshow->firstset)
	{
		// at the first set
		return 1;
	}
	else
	{
		// not at the first set
		return 0;
	}
	return 0;
}

void add_set(void)
{
	// Add a set after the current one
	struct set_proto *nextset;
	int newcounts = 0;
	nextset = pshow->currset->next;
	if (nextset && pshow->step)
		newcounts = nextset->counts;
	newset_create(pshow->currset);
	set_next();
	if (newcounts)
		pushCounts(&pshow->undobr, setnum, newcounts, 0);
	pushSetMk(&pshow->undobr);

	return;
}


void delete_set(void)
{
	// delete a set
	// undo chain in set_destroy
	set_destroy();
}


void set_first(void)
{	// Move to first set
	//if (!playing)
	{
		pshow->currset = pshow->firstset;
		pshow->prevset = 0;
		pshow->step = 0;
		setnum=0;
		//set_step=0;
	}
}


void set_last(void)
{
	// Goto the last set
	//if (!playing)
	{
		while (pshow->currset->next != NULL)
		{
			if (pshow->prevset == NULL)
				pshow->prevset = pshow->currset;
			else
				pshow->prevset = pshow->prevset->next;
			pshow->currset = pshow->currset->next;
			setnum++;
		}
		pshow->step = 0;

		//setnum=set_tot-1;
		//set_step=0;
	}
}

void set_next(void)
{	// Move to the next set
	//if (!playing)
	{
		if (pshow->currset->next != NULL)
		{
			pshow->prevset = pshow->currset;
			pshow->currset = pshow->currset->next;
			setnum++;
			pshow->step = 0;
		}
	}
}


void set_next_count(void)
{
	struct set_proto *nextset;
	//if (!playing)
	{	// shouldn't use this when playing
		//printf("set_next setnum=%i\nset_tot=%i\nset_step=%i\n", setnum, set_tot, set_step);
		if (!isLastSet())
		{
			pshow->step++;
			nextset = pshow->currset;
			nextset = nextset->next;
			if (pshow->step >= nextset->counts)
			{
				// to the next set
				set_next();
				/*
				pshow->step = 0;
				pshow->prevset = pshow->currset;
				pshow->currset = nextset;
				setnum++;
				*/
			}
		}
		/*
		if (setnum+1<set_tot)
			set_step++;
		else
			setnum=set_tot-1;
		if (set_step >= counts[setnum+1] && setnum+1<set_tot)
		{
			set_step=0;
			setnum++;
		}
		*/
	}
}

void set_prev_count(void)
{
	// go to the previous count
	struct set_proto *nextset;
	if (!isFirstSet() || pshow->step)
	{
		pshow->step--;
		if (pshow->step < 0)
		{
			// to the next set
			pshow->step = 0;
			set_prev();
			pshow->step = pshow->currset->next->counts-1;
		}
	}
	return;
}


void set_prev(void)
{
	struct set_proto *last;

	//if (!playing)	// shouldn't use this when playing
	{
		// if in the middle of set,
		// go to beginning of set
		if (pshow->step)
			pshow->step = 0;
		else if (pshow->prevset != NULL)
		{
			// not already at first set, move backwards
			pshow->currset = pshow->prevset;
			last = pshow->firstset;
			if (pshow->currset == last)
			{
				// first set
				pshow->prevset = 0;
				setnum = 0;
			}
			else
			{
				// find previous set
				while (last->next != pshow->currset && last != NULL)
				{
					last = last->next;
				}
				if (last != NULL)
				{
					pshow->prevset = last;
					setnum--;
				}
				else
				{
					pshow->prevset = 0;
					setnum = 0;
				}
			}
		}
		/*
		if (!set_step)
			setnum--;
		else	// go back to start of set, instead of set-1
			set_step=0;
		if (setnum < 0)
			setnum = 0;
		*/
	}
}


void update_tempo(void)
{
	// if the tempo is out of date, change it
	// to the correct tempo
	struct tempo_proto *currtempo;
	struct tempo_proto *othertempo;
	int cset;
	int nset;

	currtempo = pshow->currtempo;
	// Go back to general location
	if (currtempo->anchorpoint > setnum)
	{
		// Go backward to tempo
		while (currtempo->anchorpoint > setnum && currtempo->prev != NULL)
			currtempo = currtempo->prev;
	}
	else
	{
		// Go forward to tempo
		othertempo = currtempo->next;
		while (othertempo)
		{
			cset = currtempo->anchorpoint;
			nset = othertempo->anchorpoint;
			// find correct tempo forward
			if (nset <= setnum)
			{
				// go to next tempo
				currtempo = othertempo;
				othertempo = othertempo->next;
			}
			else
				othertempo = 0;
		}
	}
	// interface with deprecated tempo system
	//tempo = currtempo->tempo;
	pshow->currtempo = currtempo;
	return;
}


