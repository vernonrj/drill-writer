/*
#include "d_gtk.h"
#include "dr_setbar.h"
void add_set_gtk(GtkWidget *widget)
{
	// add a set from a Gtk entry
	if (!pstate.playing)
	{
		add_set();
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
	return;
}

void delete_set_gtk(GtkWidget *widget)
{
	// delete a set from a Gtk entry
	if (!pstate.playing)
	{
		delete_set();
		do_field = 0;
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
	return;
}



void set_first_gtk(GtkWidget *widget)
{
	// Move to first set, do gtk stuff
	if (!pstate.playing)
	{
		set_first();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
	return;
}

void set_last_gtk(GtkWidget *widget)
{
	// Go to last set from Gtk button
	if (!pstate.playing)
	{
		set_last();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);

	}
	return;
}

void set_next_gtk(GtkWidget *widget)
{
	// Go to next set from Gtk button
	if (!pstate.playing)
	{
		set_next();
		do_field=0;
		//gdk_drawable_get_size(widget->window, &width, &height);
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
	return;
}


void set_next_count_gtk(GtkWidget *widget)
{
	// Go to next count from gtk button
	if (!pstate.playing)
	{
		set_next_count();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
	return;
}

void set_prev_count_gtk(GtkWidget *widget)
{
	// Go to next count from gtk button
	if (!pstate.playing)
	{
		set_prev_count();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
	return;
}
				
void set_prev_gtk(GtkWidget *widget)
{
	// Go to previous set from Gtk button
	if (!pstate.playing)
	{
		set_prev();
		do_field=0;	// don't need to draw field
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
	return;
}



void set_set_name_gtk(GtkWidget *widget, DrSetbar *setbar)
{
	// Set the name of the set
	// Name hard-limited to 4 chars currently
	// name is currently taken from entry_sets
	const gchar *entry_buffer;
	//int set_buffer;
	struct set_proto *setcurr = pshow->sets->currset;
	int size;

	if (!pstate.playing)
	{	
		entry_buffer = gtk_entry_get_text(GTK_ENTRY(setbar->priv->entry_sets));
		size = strlen(entry_buffer);
		if (size < 5)
		{
			// set the name of the current set
			free(setcurr->name);
			setcurr->name = (char*)malloc(size*sizeof(char)+1);
			strcpy(setcurr->name, entry_buffer);
		}
		do_field = 0;	// don't need to redraw field yet
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
	return;
}


void goto_set_gtk (GtkWidget *widget)
{	// Go to set specified in entry_sets
	const gchar *entry_buffer;
	int set_buffer;
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_sets));
		set_buffer = atoi(entry_buffer);
		goto_set(set_buffer);
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
}



void goto_count (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int count_buffer;
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_counts));
		count_buffer = atoi(entry_buffer);
		if (!isLastSet() && pshow->step < pshow->sets->currset->counts)
			pshow->step = count_buffer;
		//if (setnum+1<set_tot && count_buffer < counts[setnum+1])
			//set_step=count_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
}
		
void change_counts (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int count_buffer;
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_counts));
		count_buffer = atoi(entry_buffer);
		if (count_buffer > 0)
		{
			pushCounts(&pshow->undobr, pstate.setnum, pshow->sets->currset->counts, 1);
			pshow->sets->currset->counts = count_buffer;
			//counts[setnum] = count_buffer;
		}
	}
}


void change_tempo_gtk (GtkWidget *widget)
{
	// Change the tempo internally
	// TODO: Non-global tempo
	struct tempo_proto *currtempo;
	//struct tempo_proto *stempo;
	//struct tempo_proto *nexttempo;
	const gchar *entry_buffer;
	int tmpo;

	entry_buffer = gtk_entry_get_text(GTK_ENTRY(entry_tempo));
	tmpo = atoi(entry_buffer);
	currtempo = pshow->currtempo;
	change_tempo(tmpo, &currtempo);
	pshow->currtempo = currtempo;
	gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);

	return;
}
*/
