// Functions like playing, stopping, etc go here (automated set movement)
//#include "drill.h"
#include "d_gtk.h"
#include "dr-media.h"

static void dr_mediabar_class_init(DrMediabarClass *klass);
static void dr_mediabar_init(DrMediabar *mediabar);

/*
struct _DrMediabarPrivate {
	// nothing here yet
};
*/

G_DEFINE_TYPE (DrMediabar, dr_mediabar, GTK_TYPE_VBOX)

#define DR_MEDIABAR_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_MEDIABAR_TYPE, DrMediabarPrivate))

static void dr_mediabar_class_init(DrMediabarClass *class)
{
	//GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	//g_type_class_add_private(g_object_class, sizeof(DrMediabarPrivate));
}


static void dr_mediabar_init(DrMediabar *mediabar)
{
	GtkWidget *playbox;
	GtkWidget *firstlastbox;
	GtkWidget *nextprevbox;

	GtkWidget *button;
	GtkWidget *image;

	playbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(mediabar), playbox, FALSE, FALSE, 0);

	// Play
	button = gtk_button_new_with_label ("Play");
	g_signal_connect(button, "clicked", G_CALLBACK(queue_show), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_MENU);
	gtk_button_set_image (GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (playbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	/*
	// Stop
	button = gtk_button_new_with_label ("Stop");
	g_signal_connect(button, "clicked", G_CALLBACK(stop_show), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (mediabar), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);
	*/

	// Play from Start
	button = gtk_button_new_with_label ("Start");
	g_signal_connect(button, "clicked", G_CALLBACK(play_show_from_start), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PREVIOUS, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_end(GTK_BOX (playbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);


	firstlastbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(mediabar), firstlastbox, FALSE, FALSE, 0);

	// First Set
	button = gtk_button_new_with_label ("First");
	g_signal_connect(button, "clicked", G_CALLBACK(set_first_gtk), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_GOTO_FIRST, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (firstlastbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Last Set
	button = gtk_button_new_with_label ("Last");
	image = gtk_image_new_from_stock (GTK_STOCK_GOTO_LAST, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	g_signal_connect(button, "clicked", G_CALLBACK(set_last_gtk), NULL);
	gtk_box_pack_start(GTK_BOX (firstlastbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	nextprevbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(mediabar), nextprevbox, FALSE, FALSE, 0);

	// Previous Set
	button = gtk_button_new_with_label ("Prev"); 
	g_signal_connect(button, "clicked", G_CALLBACK(set_prev_gtk), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_REWIND, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (nextprevbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Next Set
	button = gtk_button_new_with_label ("Next");
	g_signal_connect(button, "clicked", G_CALLBACK(set_next_gtk), NULL);//, window);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_FORWARD, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (nextprevbox), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Step
	button = gtk_button_new_with_label ("Step");
	g_signal_connect(button, "clicked", G_CALLBACK(set_next_count_gtk), NULL);
	gtk_box_pack_start(GTK_BOX (mediabar), button, FALSE, FALSE, 0);
	gtk_widget_show(button);


}

GtkWidget *dr_mediabar_new(void)
{
	return GTK_WIDGET(g_object_new(dr_mediabar_get_type(), NULL));
}


gboolean play_show (GtkWidget *widget)
{	// Play the show
	gulong dumb_API;
	struct set_proto *nextset;
	int local_tempo = pshow->currtempo->tempo;
	time_elapsed = g_timer_elapsed(timer, &dumb_API);
	if (pstate.playing == 1 && time_elapsed >= (double)60/(double)local_tempo && !expose_flag)
	{	
		//set_step++;
		pstate.curr_step++;
		do_field=0;	// don't need to redraw field
		nextset = pshow->sets->currset->next;
		if (pstate.curr_step >= nextset->counts)
		{
			// next set
			set_next();
			if (pshow->sets->currset->next == NULL)
			{
				// last set
				pstate.playing = 0;
			}
		}
		//g_print("width=%i\theight=%i\n", width, height);
		expose_flag =1;
		gtk_widget_queue_draw_area(window, 0, 0, 
				fldstate.width, fldstate.height);
		//g_print("Play_show %i %i %i\n", setnum, set_step, playing);
		if (pstate.playing)
		{
			g_timer_start(timer);
			return TRUE;
		}
		else
			return FALSE;
	}
	else if (pstate.playing == 0)
		return FALSE;
	return TRUE;
}


void stop_show (GtkWidget *widget)
{
	pstate.playing=0;
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
}


void queue_show (GtkWidget *widget)//, GtkWidget *window)
{
	if (!pstate.playing)
	{
		(void)g_timeout_add(50, (GSourceFunc)play_show, window);
		do_field=0;	// don't need to redraw field
		gtk_widget_queue_draw_area(window, 0, 0, 
				fldstate.width, fldstate.height);
		if (pshow->sets->currset->next != NULL)
			pstate.playing = 1;
		else
		{
			// play the last set
			set_prev();
			pstate.playing = 1;
		}
		g_timer_start(timer);	// start up the timer
	}
	else
		stop_show(widget);

}

void play_show_from_start (GtkWidget *widget)
{
	if (!pstate.playing)
	{
		pshow->sets->currset = pshow->sets->firstset;
		pshow->sets->currset->prev = 0;
		pstate.curr_step = 0;
		pstate.setnum=0;
		(void)g_timeout_add(50, (GSourceFunc)play_show, window);
		pstate.playing=1;
		gtk_widget_queue_draw_area(window, 0, 0, 
				fldstate.width, fldstate.height);
		g_timer_start(timer);
	}
}
