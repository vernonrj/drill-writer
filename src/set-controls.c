// Functions that change sets go here
#include "drill.h"

gboolean zoom_in(GtkWidget *widget, GdkEventScroll *event)
{
	// handle zoom events
	if (event->direction == GDK_SCROLL_DOWN)
	{
		printf("scrolled down\n");
		zoom_x = zoom_x + 100;
		zoom_y = zoom_y + 100;
		//gtk_widget_set_size_request(drill, zoom_x, zoom_y);
	}
	else if (event->direction == GDK_SCROLL_UP)
	{
		printf("scrolled up\n");
	}
	return FALSE;
}

void add_set_gtk(GtkWidget *widget)
{
	// add a set from a Gtk entry
	if (!playing)
	{
		add_set();
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


void goto_set (GtkWidget *widget)
{	// Go to set specified in entry_sets
	const gchar *entry_buffer;
	int set_buffer;
	int i = 0;
	struct set_proto *last = pshow->firstset;
	struct set_proto *curr = 0;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_sets));
		set_buffer = atoi(entry_buffer);
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
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}



void change_tempo (GtkWidget *widget)
{
	// Change the tempo internally
	// TODO: Non-global tempo
	const gchar *entry_buffer;
	struct tempo_proto *prevtempo;
	struct tempo_proto *currtempo;
	struct tempo_proto *nexttempo;
	struct tempo_proto *stempo;
	int tmpo;

	entry_buffer = gtk_entry_get_text(GTK_ENTRY(entry_tempo));
	tmpo = atoi(entry_buffer);
	currtempo = pshow->currtempo;
	if (tmpo >= 30 && tmpo <= 250)
	{
		// Tempo is valid. Make changes thusly

		tempo = tmpo;
		if (currtempo->anchorpoint == setnum)
		{
			// changing an existing tempo
			currtempo->tempo = tmpo;
		}
		else
		{
			// making a new node
			stempo = (struct tempo_proto*) malloc(sizeof(struct tempo_proto));
			// store data to new node
			stempo->tempo = tmpo;
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
			if (prevtempo->tempo == tmpo)
			{
				// current node is redundant, can be deleted
				prevtempo->next = currtempo->next;
				pshow->currtempo = prevtempo;
				free(currtempo);
				currtempo = prevtempo;
			}
		}
		nexttempo = currtempo->next;
		if (nexttempo)
		{
			if (nexttempo->tempo == tmpo)
			{
				// next node is redundant, can be deleted
				currtempo->next = nexttempo->next;
				free(nexttempo);
			}
		}
		pshow->currtempo = currtempo;
	}
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

void add_set(void)
{
	// Add a set after the current one
	newset_create(pshow->currset);
	return;
}


void delete_set(void)
{
	// delete a set
	set_destroy();
}


void set_first(void)
{	// Move to first set
	//if (!playing)
	{
		pshow->currset = pshow->firstset;
		pshow->prevset = 0;
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
				pshow->step = 0;
				pshow->prevset = pshow->currset;
				pshow->currset = nextset;
				setnum++;
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
		while (currtempo->anchorpoint > setnum)
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
	tempo = currtempo->tempo;
	pshow->currtempo = currtempo;
	return;
}


