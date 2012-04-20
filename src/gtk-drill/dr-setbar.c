#include <gtk/gtk.h>
#include "dr_setbar.h"
#include "d_gtk.h"

static void dr_setbar_class_init(DrSetbarClass *klass);
static void dr_setbar_init(DrSetbar *setbar);


struct _DrSetbarPrivate {
	GtkWidget *label;
	GtkWidget *entry_sets;
	GtkWidget *entry_counts;
	GtkWidget *entry_tempo;
};

//G_DEFINE_TYPE (DrSetbar, dr_setbar, GTK_TYPE_VBOX)
G_DEFINE_TYPE (DrSetbar, dr_setbar, GTK_TYPE_HBOX)

#define DR_SETBAR_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_SETBAR_TYPE, DrSetbarPrivate))

/*
GType dr_setbar_get_type(void)
{
	static GType dr_setbar_type = 0;

	if (!dr_setbar_type)
	{
		const GTypeInfo dr_setbar_info =
		{
			sizeof(DrSetbarClass),
			NULL,	// base_init
			NULL,	// base_finalize
			(GClassInitFunc) dr_setbar_class_init,
			NULL,	// class_finalize
			NULL,	// class_data
			sizeof(DrSetbar),
			0,	// n_preallocs
			(GInstanceInitFunc) dr_setbar_init,
		};

		dr_setbar_type = g_type_register_static(GTK_TYPE_WIDGET,
				"DrSetbar",
				&dr_setbar_info,
				0);
	}
	return dr_setbar_type;
}

*/
static void dr_setbar_class_init(DrSetbarClass *class)
{
	//GtkWidgetClass *widget_class;
	//GtkObjectClass *object_class;
	GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(g_object_class, sizeof(DrSetbarPrivate));
}

static void dr_setbar_init(DrSetbar *setbar)
{
	GtkWidget *label;
	GtkWidget *entry_sets;
	GtkWidget *entry_counts;
	GtkWidget *entry_tempo;
	int tempo;
	char set_buf[5];
	char count_buf[5];
	char tempo_buf[5];

	setbar->priv = DR_SETBAR_GET_PRIVATE(setbar);	

	label = gtk_label_new ("Set:");
	gtk_box_pack_start (GTK_BOX (setbar), label, FALSE, TRUE, 0);

	// Set entry
	//sprintf(set_buf, "%i", setnum);
	sprintf(set_buf, "%i", pstate.setnum);
	entry_sets = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_sets), 50);
	g_signal_connect(entry_sets, "activate", G_CALLBACK (goto_set_gtk), setbar);
	gtk_entry_set_text (GTK_ENTRY (entry_sets), set_buf);
	//tmp_pos = GTK_ENTRY (entry_sets)->text_length;
	gtk_editable_set_editable(GTK_EDITABLE(entry_sets), FALSE);
	gtk_editable_select_region (GTK_EDITABLE (entry_sets),
			0, GTK_ENTRY (entry_sets)->text_length);
	gtk_entry_set_alignment (GTK_ENTRY (entry_sets), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_sets), 4);
	gtk_box_pack_start (GTK_BOX (setbar), entry_sets, FALSE, TRUE, 0);
	gtk_widget_show(entry_sets);

	//separator = gtk_vseparator_new ();
	//gtk_box_pack_start (GTK_BOX (setbox), separator, FALSE, TRUE, 0);

	// Count entry
	label = gtk_label_new ("Counts:");
	gtk_box_pack_start (GTK_BOX (setbar), label, FALSE, TRUE, 0);

	sprintf(count_buf, "%i", pshow->sets->currset->counts);
	entry_counts = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_counts), 50);
	g_signal_connect(entry_counts, "activate", G_CALLBACK (change_counts), setbar);
	gtk_entry_set_text (GTK_ENTRY (entry_counts), count_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	//gtk_editable_insert_text (GTK_EDITABLE (entry_counts), " world", -1, &tmp_pos);
	gtk_editable_select_region (GTK_EDITABLE (entry_counts),
			0, GTK_ENTRY (entry_counts)->text_length);
	gtk_entry_set_alignment (GTK_ENTRY (entry_counts), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_counts), 4);
	gtk_box_pack_start (GTK_BOX (setbar), entry_counts, FALSE, TRUE, 0);
	gtk_widget_show(entry_counts);

	// Tempo box (embedded in set attribute box)
	label = gtk_label_new ("Tempo");
	gtk_box_pack_start(GTK_BOX(setbar), label, FALSE, TRUE, 0);

	// Tempo entry box
	tempo = pshow->currtempo->tempo;
	sprintf(tempo_buf, "%i", tempo);
	entry_tempo = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry_tempo), 50);
	g_signal_connect(entry_tempo, "activate", G_CALLBACK(change_tempo_gtk), setbar);
	gtk_entry_set_text(GTK_ENTRY(entry_tempo), tempo_buf);
	//tmp_pos = GTK_ENTRY(entry_tempo)->text_length;
	gtk_editable_select_region(GTK_EDITABLE(entry_tempo),
			0, GTK_ENTRY(entry_tempo)->text_length);
	gtk_entry_set_alignment(GTK_ENTRY(entry_tempo), 1);
	gtk_entry_set_width_chars(GTK_ENTRY(entry_tempo), 4);
	gtk_box_pack_start(GTK_BOX(setbar), entry_tempo, FALSE, TRUE, 0);
	gtk_widget_show(entry_tempo);

	setbar->priv->entry_sets = entry_sets;
	setbar->priv->entry_counts = entry_counts;
	setbar->priv->entry_tempo = entry_tempo;
}

GtkWidget *dr_setbar_new(void)
{
	return GTK_WIDGET(g_object_new(dr_setbar_get_type(), NULL));
}


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


void goto_set_gtk (GtkWidget *widget, DrSetbar *setbar)
{	// Go to set specified in entry_sets
	const gchar *entry_buffer;
	int set_buffer;
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (setbar->priv->entry_sets));
		set_buffer = atoi(entry_buffer);
		goto_set(set_buffer);
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
}



void goto_count (GtkWidget *widget, DrSetbar *setbar)
{
	const gchar *entry_buffer;
	int count_buffer;
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (setbar->priv->entry_counts));
		count_buffer = atoi(entry_buffer);
		if (!isLastSet() && pshow->step < pshow->sets->currset->counts)
			pshow->step = count_buffer;
		//if (setnum+1<set_tot && count_buffer < counts[setnum+1])
			//set_step=count_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);
	}
}
		
void change_counts (GtkWidget *widget, DrSetbar *setbar)
{
	const gchar *entry_buffer;
	int count_buffer;
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (setbar->priv->entry_counts));
		count_buffer = atoi(entry_buffer);
		if (count_buffer > 0)
		{
			pushCounts(&pshow->undobr, pstate.setnum, pshow->sets->currset->counts, 1);
			pshow->sets->currset->counts = count_buffer;
			//counts[setnum] = count_buffer;
		}
	}
}


void change_tempo_gtk (GtkWidget *widget, DrSetbar *setbar)
{
	// Change the tempo internally
	// TODO: Non-global tempo
	struct tempo_proto *currtempo;
	//struct tempo_proto *stempo;
	//struct tempo_proto *nexttempo;
	const gchar *entry_buffer;
	int tmpo;

	entry_buffer = gtk_entry_get_text(GTK_ENTRY(setbar->priv->entry_tempo));
	tmpo = atoi(entry_buffer);
	currtempo = pshow->currtempo;
	change_tempo(tmpo, &currtempo);
	pshow->currtempo = currtempo;
	gtk_widget_queue_draw_area(window, 0, 0, pstate.width, pstate.height);

	return;
}

void dr_setbar_update_entries(GtkWidget *drsetbar)
{
	DrSetbar *asetbar;
	char set_buf[5];
	char count_buf[5];
	char tempo_buf[5];
	int tempo;
	asetbar = (DrSetbar*)drsetbar;
	if (!pshow->sets->currset->name[0])
	{
		//sprintf(set_buf, "%i", setnum);
		sprintf(set_buf, "%i", pstate.setnum);
	}
	else
		strcpy(set_buf, pshow->sets->currset->name);
	update_tempo();
	tempo = pshow->currtempo->tempo;
	sprintf(tempo_buf, "%i", tempo);
	//printf("tempo = %i\n", tempo);
	if (pshow->step)
	{
		// take the counts from the next set
		sprintf(count_buf, "%i", pshow->sets->currset->next->counts);
	}
	else
	{
		// Take the counts from this set
		sprintf(count_buf, "%i", pshow->sets->currset->counts);
	}

	gtk_entry_set_text(GTK_ENTRY (asetbar->priv->entry_sets), set_buf);
	gtk_entry_set_text(GTK_ENTRY (asetbar->priv->entry_tempo), tempo_buf);
	gtk_entry_set_text(GTK_ENTRY (asetbar->priv->entry_counts), count_buf);

	return;
}

