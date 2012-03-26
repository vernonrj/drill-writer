// Drill Writer Prototype Version 0.0.0.6
// main.c
#include <stdlib.h>

#include "drill.h"		// header file, with most global variables
#include "media.c"		// playback functions
#include "set-controls.c"	// Various functions concerning the use of sets
#include "perf-con.c"		// Various functions for performers/dots
#include "file-ops.c"		// File operations
#include "count-con.c"		// count structure

extern int expose_flag;		// might not be required
//extern static GtkActionEntry entries;
//extern static guint n_entries;

GtkWidget *entry_perf;
GtkWidget *entry_perf_x;
GtkWidget *entry_perf_y;

void move_up(GtkWidget *widget)
{
	perf[setnum][perf_cur][1]--;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}

void move_down(GtkWidget *widget)
{
	perf[setnum][perf_cur][1]++;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}

void move_left(GtkWidget *widget)
{
	perf[setnum][perf_cur][0]--;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}

void move_right(GtkWidget *widget)
{
	perf[setnum][perf_cur][0]++;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}

void next_perf(GtkWidget *widget)
{
	if (perf_cur < perfnum-1)
	{
		perf_cur++;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void prev_perf(GtkWidget *widget)
{
	if (perf_cur > 0)
	{
		perf_cur--;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

static void not_implemented ()
{	// If I don't have something yet, callbacks go here
	g_print("This isn't done yet...\n");
}

void force_redraw(GtkWidget *widget)
{	// Refresh the field
	do_field=1;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}

static void quit_action ()
{	// Quit (will have more here later)
	gtk_main_quit();
}


gboolean clicked(GtkWidget *widget, GdkEventButton *event,
		gpointer user_data)
{
	int i, j;
	double coordx, coordy;
	double workx, worky;
	double closex, closey;
	if (event->button == 1)
	{
		coordx = event->x;
		coordy = event->y;
		printf("x = %g, y = %g\n", coordx, coordy);
		coordx = (coordx-xo2)/step;
		//coordy = (coordy-yo2-25)/step;
		coordy = (coordy-yo2-50)/step;

		printf("button 1 pressed at %g %g %g\n", coordx, coordy, yo2);
		closex = 10;
		closey = 10;
		for (i=0; i<perfnum; i++)
		{
			workx = perf[setnum][i][0]-coordx;
			printf("at %i workx = %g worky = %g\n", i, workx, worky);
			if (labs(workx) <= closex)
			{
				printf("ping\n");
				worky = perf[setnum][i][1] - coordy;
				if (labs(worky) <= closey)
				{
					perf_cur = i;
					printf("found dot: %i\n", i);
					closex = labs(workx);
					closey = labs(worky);
					printf("closex = %g closey = %g\n", closex, closey);
				}
			}
		}
		if (closex != 10)
			gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return TRUE;
}

void calc_stepsize (GtkWidget *widget)
{
	double dy, dx;
	double stepsize;
	dx = pow(perf[setnum][perf_cur][0] - perf[setnum-1][perf_cur][0], 2);
	dy = pow(perf[setnum][perf_cur][1] - perf[setnum-1][perf_cur][1], 2);
	stepsize = 8*counts[setnum];
	stepsize = stepsize/sqrt(dy + dx);
	printf("stepsize = %g:5\n", stepsize);
}

int main (int argc, char *argv[])
{
	GtkActionGroup *action_group;	// menus
	GtkUIManager *menu_manager;	// menus
	GError *error;	
	GtkWidget *menubar;		// for menus
	GtkWidget *toolbar;
	GtkWidget *drill;		// custom cairo widget
	// Container Widgets
	GtkWidget *box0;		// second-level packing box (for canvas)
	GtkWidget *box1;
	GtkWidget *setbox;		// Set attributes go in this box
	GtkWidget *perfbox;		// Dot attributes go in this box
	GtkWidget *box3;
	GtkWidget *menu_box;		// First-level packing box (for menus)
	GtkWidget *media_box;		// For media buttons
	// buttons, checks, etc
	GtkWidget *button;		// buttons
	GtkWidget *check;
	GtkWidget *label;
	GtkWidget *separator;
	GtkWidget *image;



	gint tmp_pos;

	int i;		// loop vars

	// Start up gtk
	gtk_init(&argc, &argv);

	// Build the menu
	static GtkActionEntry entries[] =
	{
	{ "FileMenuAction", NULL, "_File" },	// name, stock id, label
	{ "ToolsMenuAction", NULL, "_Tools"},
	{ "HelpMenuAction", NULL, "_Help"},
	{ "RunMenuAction", NULL, "_Run" },
	{ "AddMenuAction", NULL, "_Add"},
	{ "MoveMenuAction", NULL, "_Move"},
		
	{ "NewAction", GTK_STOCK_NEW,
		"_New", "<control>N",
		"Create a new file",
		G_CALLBACK (not_implemented) },
	{ "OpenAction", GTK_STOCK_OPEN,
		"_Open", "<control>O",
		"Open an existing file",
		G_CALLBACK (show_gen) },
	{ "SaveAction", GTK_STOCK_SAVE,
		"_Save", "<control>S",
		"Save file",
		G_CALLBACK (save_file) },
	{ "SaveAsAction", NULL,
		"_Save As", NULL,
		"Save to new file",
		G_CALLBACK (not_implemented) },
	{ "ExportPDFAction", NULL,
		"_Export to PDF", NULL,
		"Export show to PDF file",
		G_CALLBACK (not_implemented) },
	{ "QuitAction", GTK_STOCK_QUIT,		// name, stock id
		"_Quit", "<control>Q",		// label, accelerator
		"Exit Program",			// tooltip
		G_CALLBACK (gtk_main_quit) },
	{ "MoveUpAction", NULL,
		"_Move Up", "W",
		"Move performer up 1 step",
		G_CALLBACK (move_up) },
	{ "MoveDownAction", NULL,
		"_Move Down", "S",
		"Move performer down 1 step",
		G_CALLBACK (move_down) },
	{ "MoveLeftAction", NULL,
		"_Move Left", "A",
		"Move performer Left 1 step",
		G_CALLBACK (move_left) },
	{ "MoveRightAction", NULL,
		"_Move Right", "D",
		"Move performer right 1 step",
		G_CALLBACK (move_right) },
	{ "NextPerfAction", NULL,
		"_Next Performer", "bracketright",
		"Highlight next performer",
		G_CALLBACK (next_perf) },
	{ "PrevPerfAction", NULL,
		"_Previous Performer", "bracketleft",
		"Highlight previous performer",
		G_CALLBACK (prev_perf) },
	{ "NextSetAction", NULL,
		"_Next Set", "J",
		"Go to next set",
		G_CALLBACK(set_next) },
	{ "PrevSetAction", NULL,
		"_Previous Set", "K",
		"Go to previous set",
		G_CALLBACK (set_prev) },
	{ "PlayHereAction", NULL,
		"_Play", "P",
		"Play from current set",
		G_CALLBACK (queue_show) },
	{ "PlayFromStartAction", NULL,
		"_Play from Start", "<shift>P",
		"Play from first set",
		G_CALLBACK (play_show_from_start) },
	{ "StopPlayAction", NULL,
		"_Stop", NULL,
		"Stop playback",
		G_CALLBACK (stop_show) },
	{ "AddPerfAction", NULL,
		"_Add Performer", NULL,
		"Add a performer to the show",
		G_CALLBACK (add_perf) },
	{ "DelPerfAction", NULL,
		"_Delete Performer", NULL,
		"Delete a performer from the show",
		G_CALLBACK (delete_perf) },
	{ "AddSetAction", NULL,
		"_Add Set", NULL,
		"Add a set to the show",
		G_CALLBACK (add_set) },
	{ "DelSetAction", NULL,
		"_Delete Set", NULL,
		"Delete a set from the show",
		G_CALLBACK (delete_set) },
	{ "ForceRedrawAction", NULL,
		"_Force Redraw", "F5",
		"Refresh the canvas",
		G_CALLBACK (force_redraw) },
	{ "StepsizeAction", NULL,
		"_Calculate Step-size", NULL,
		"Find the step size of selected dot",
		G_CALLBACK (calc_stepsize) },
	{ "SnapshotAction", NULL,
		"_Snapshot", "<shift>S",
		"Save current page to *.png",
		G_CALLBACK (not_implemented) },
	{ "AboutAction", NULL, 
		"_About", NULL,
		"About this Program",
		G_CALLBACK (not_implemented) }
	};
	static guint n_entries = G_N_ELEMENTS (entries);

	// timer setup
	timer = g_timer_new();
	g_timer_stop(timer);

	// draw the field the first time
	do_field=1;

	// Window Actions
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_add_events (window, GDK_BUTTON_PRESS_MASK);
	gtk_window_set_title(GTK_WINDOW(window), "Drill-Writer-Proto 0.0.0.7");
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	
	// Might need these in the future
	//gtk_widget_set_app_paintable(window, TRUE);
	//gtk_widget_set_double_buffered(window, FALSE);

	// Hardcode dots in
	perf[0][0][0] = 32;
	perf[0][0][1] = 53;
	
	perf[0][1][0] = 36;
	perf[0][1][1] = 53;

	perf[0][2][0] = 40;
	perf[0][2][1] = 53;

	perf[0][3][0] = 34;
	perf[0][3][1] = 49;
	
	perf[0][4][0] = 38;
	perf[0][4][1] = 49;

	perf[0][5][0] = 36;
	perf[0][5][1] = 45;

	// Hard-coded number of performers
	perfnum = 6;
	
	for (i=0; i< perfnum; i++)
	{
		perf[1][i][0]=perf[0][i][0]+4;
		perf[1][i][1]=perf[0][i][1];
		perf[2][i][0]=perf[1][i][0];
		perf[2][i][1]=perf[1][i][1]-10;
	}
	// Hard-coded total number of sets/count structure
	set_tot = 3;
	counts[0] = 0;
	counts[1] = 8;
	counts[2] = 8;

	// Window Signals
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Menu Stuff
	menu_box = gtk_vbox_new (FALSE, 0);
	action_group = gtk_action_group_new ("TestActions");
	menu_manager = gtk_ui_manager_new ();

	// Hook up window to menu_box
	gtk_container_add (GTK_CONTAINER (window), menu_box);
	gtk_action_group_add_actions (action_group, entries, n_entries, NULL);
	gtk_ui_manager_insert_action_group (menu_manager, action_group, 0);

	// Read menu UI from XML file
	error = NULL;
	gtk_ui_manager_add_ui_from_file (menu_manager, "menu-ui.xml", &error);
	if (error)
	{
		g_message ("building menus failed: %s", error->message);
		g_error_free (error);
	}

	// Get the menubar and toolbar and put them in vertical packing box
	menubar = gtk_ui_manager_get_widget (menu_manager, "/MainMenu");
	gtk_box_pack_start (GTK_BOX (menu_box), menubar, FALSE, FALSE, 0);

	// Make sure the accelerators work
	gtk_window_add_accel_group (GTK_WINDOW (window),
			gtk_ui_manager_get_accel_group (menu_manager));

	// Start packing canvas
	box0 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX (menu_box), box0, TRUE, TRUE, 0);

	//box3 = gtk_hbox_new (FALSE, 0);
	//gtk_box_pack_start(GTK_BOX (box0), box3, FALSE, FALSE, 0);

	setbox = gtk_hbox_new (FALSE, 0);	// Set attributes (set, counts, tempo, etc)
	gtk_box_pack_start(GTK_BOX (box0), setbox, FALSE, FALSE, 0);

	// get and pack canvas
	printf("ping\n");
	drill = gtk_drill_new();
	printf("ping\n");
	gtk_box_pack_start(GTK_BOX (box0), drill, TRUE, TRUE, 0);
	g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), NULL);

	perfbox = gtk_hbox_new (FALSE, 0);	// Dot attributes
	gtk_box_pack_start(GTK_BOX (box0), perfbox, FALSE, FALSE, 0);


	media_box = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX (box0), media_box, FALSE, FALSE, 0);

	box1 = gtk_hbox_new (FALSE, 0);	// Media Controls (First, prev, next, last)
	gtk_box_pack_start(GTK_BOX (media_box), box1, FALSE, FALSE, 0);

	label = gtk_label_new ("Set:");
	gtk_box_pack_start (GTK_BOX (setbox), label, FALSE, TRUE, 0);

	// need to change these to spin buttons
	sprintf(set_buf, "%i", setnum);
	entry_sets = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_sets), 50);
	g_signal_connect(entry_sets, "activate", G_CALLBACK (goto_set), entry_sets);
	gtk_entry_set_text (GTK_ENTRY (entry_sets), set_buf);
	tmp_pos = GTK_ENTRY (entry_sets)->text_length;
	//gtk_editable_insert_text (GTK_EDITABLE (entry_sets), " world", -1, &tmp_pos);
	gtk_editable_select_region (GTK_EDITABLE (entry_sets),
			0, GTK_ENTRY (entry_sets)->text_length);
	gtk_entry_set_alignment (GTK_ENTRY (entry_sets), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_sets), 4);
	gtk_box_pack_start (GTK_BOX (setbox), entry_sets, FALSE, TRUE, 0);

	//separator = gtk_vseparator_new ();
	//gtk_box_pack_start (GTK_BOX (setbox), separator, FALSE, TRUE, 0);

	label = gtk_label_new ("Counts:");
	gtk_box_pack_start (GTK_BOX (setbox), label, FALSE, TRUE, 0);

	// need to change these to spin buttons
	//sprintf(count_buf, "%i", set_step);
	sprintf(count_buf, "%i", counts[setnum]);
	entry_counts = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_counts), 50);
	g_signal_connect(entry_counts, "activate", G_CALLBACK (change_counts), entry_counts);
	gtk_entry_set_text (GTK_ENTRY (entry_counts), count_buf);
	tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	//gtk_editable_insert_text (GTK_EDITABLE (entry_counts), " world", -1, &tmp_pos);
	gtk_editable_select_region (GTK_EDITABLE (entry_counts),
			0, GTK_ENTRY (entry_counts)->text_length);
	gtk_entry_set_alignment (GTK_ENTRY (entry_counts), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_counts), 4);
	gtk_box_pack_start (GTK_BOX (setbox), entry_counts, FALSE, TRUE, 0);

	label = gtk_label_new ("Performer:");
	gtk_box_pack_start (GTK_BOX (perfbox), label, FALSE, TRUE, 0);

	sprintf(perf_buf, "%i", perf_cur);
	entry_perf = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf), 5);
	g_signal_connect (entry_perf, "activate", G_CALLBACK (goto_perf), entry_perf);
	gtk_entry_set_text (GTK_ENTRY (entry_perf), perf_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf), 4);
	gtk_box_pack_start (GTK_BOX (perfbox), entry_perf, FALSE, TRUE, 0);

	label = gtk_label_new ("X:");
	gtk_box_pack_start (GTK_BOX (perfbox), label, FALSE, TRUE, 0);

	sprintf(perf_buf_x, "%g", perf[setnum][perf_cur][0]);
	entry_perf_x = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf_x), 5);
	g_signal_connect (entry_perf_x, "activate", G_CALLBACK (xperf_change), entry_perf_x);
	gtk_entry_set_text (GTK_ENTRY (entry_perf_x), perf_buf_x);
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf_x), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf_x), 4);
	gtk_box_pack_start (GTK_BOX (perfbox), entry_perf_x, FALSE, TRUE, 0);

	label = gtk_label_new ("Y:");
	gtk_box_pack_start (GTK_BOX (perfbox), label, FALSE, TRUE, 0);

	sprintf(perf_buf_y, "%g", perf[setnum][perf_cur][1]);
	entry_perf_y = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf_y), 5);
	g_signal_connect (entry_perf_y, "activate", G_CALLBACK (yperf_change), entry_perf_y);
	gtk_entry_set_text (GTK_ENTRY (entry_perf_y), perf_buf_y);
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf_y), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf_y), 4);
	gtk_box_pack_start (GTK_BOX (perfbox), entry_perf_y, FALSE, TRUE, 0);

	// Play
	button = gtk_button_new_with_label ("Play");
	g_signal_connect(button, "clicked", G_CALLBACK(queue_show), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_MENU);
	gtk_button_set_image (GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Stop
	button = gtk_button_new_with_label ("Stop");
	g_signal_connect(button, "clicked", G_CALLBACK(stop_show), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Play from Start
	button = gtk_button_new_with_label ("Play Start");
	g_signal_connect(button, "clicked", G_CALLBACK(play_show_from_start), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PREVIOUS, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);


	// First Set
	button = gtk_button_new_with_label ("First");
	g_signal_connect(button, "clicked", G_CALLBACK(set_first), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_GOTO_FIRST, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Previous Set
	button = gtk_button_new_with_label ("Prev"); 
	g_signal_connect(button, "clicked", G_CALLBACK(set_prev), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_REWIND, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Step
	button = gtk_button_new_with_label ("Step");
	g_signal_connect(button, "clicked", G_CALLBACK(set_next_count), NULL);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	// Next Set
	button = gtk_button_new_with_label ("Next");
	g_signal_connect(button, "clicked", G_CALLBACK(set_next), NULL);//, window);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_FORWARD, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Last Set
	button = gtk_button_new_with_label ("Last");
	image = gtk_image_new_from_stock (GTK_STOCK_GOTO_LAST, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	g_signal_connect(button, "clicked", G_CALLBACK(set_last), NULL);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);


	// Show everything
	gtk_widget_show(drill);
	gtk_widget_show(box0);
	gtk_widget_show(box1);
	gtk_widget_show(setbox);
	gtk_widget_show(perfbox);
	//gtk_widget_show(box3);
	gtk_widget_show(media_box);
	gtk_widget_show(menu_box);
	gtk_widget_show_all (window);

	tempo = 120;
	// Actual main loop for now
	(void)g_timeout_add(50, (GSourceFunc)play_show, window);

	// Run Main Loop
	gtk_main();

	return 0;
}
	
