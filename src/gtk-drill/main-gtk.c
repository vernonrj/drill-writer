#include "d_gtk.h"

int startTk(int argc, char *argv[])
{
	// Start Toolkit of choice
	// gtk
	gtk_init(&argc, &argv);
	return 0;
}


static void not_implemented ()
{	// If I don't have something yet, callbacks go here
	g_print("This isn't done yet...\n");
}


static void quit_action ()
{	// Quit (will have more here later)
	gtk_main_quit();
}


int update_entries(void)
{
	int tempo;
	char set_buf[5];
	char count_buf[5];
	char tempo_buf[5];
	char perf_buf[5];
	char perf_buf_x[5];
	char perf_buf_y[5];
	// side-to-side entries/buttons
	char ss_buf[20];
	char ss_ydrel[10];
	char ss_siderel[8];
	char ss_ydline[3];
	// front-to-back entries/buttons
	float ssStep;
	float fbStep;
	int yardRel;
	int fieldSide;
	float cx, cy;
	gchar *fb_buf;
	gchar *fb_hashrel;
	gchar *fb_frontback;
	gchar *fb_hashside;
	// Update all the entries
	cx = pshow->center->x;
	cy = pshow->center->y;
	ssStep = getSidetoSide(&cx, &cy);
	sprintf(ss_buf, "%.2f", ssStep);
	yardRel = isInsideYard(&cx, &cy, &fieldSide);
	if (yardRel == -1)
	{
		// outside
		sprintf(ss_ydrel, "outside");
	}
	else if (yardRel == 1)
	{
		// inside
		sprintf(ss_ydrel, "inside");
	}
	else
	{
		// on
		sprintf(ss_ydrel, "on");
	}
	sprintf(ss_siderel, "side %i", fieldSide);
	sprintf(ss_ydline, "%i", getYardline(&cx, &cy));
	fbStep = getFronttoBack(&cx, &cy, &fb_hashrel, &fb_frontback, &fb_hashside);
	fb_buf = g_strdup_printf("%.2f", fbStep);
	gtk_entry_set_text(GTK_ENTRY (sidesideBtns.ssStepEntry), ss_buf);
	gtk_button_set_label(GTK_BUTTON (sidesideBtns.ssYdRelButton), ss_ydrel);
	gtk_button_set_label(GTK_BUTTON (sidesideBtns.ssSide), ss_siderel);
	gtk_button_set_label(GTK_BUTTON (sidesideBtns.ssYdLine), ss_ydline);
	gtk_entry_set_text(GTK_ENTRY (frbkBtns.fbStepEntry), fb_buf);
	gtk_button_set_label(GTK_BUTTON (frbkBtns.fbHashRelButton), fb_hashrel);
	gtk_button_set_label(GTK_BUTTON (frbkBtns.fbToggleButton), fb_frontback);
	gtk_button_set_label(GTK_BUTTON (frbkBtns.HashSideButton), fb_hashside);
	g_free(fb_buf);
	g_free(fb_hashrel);
	g_free(fb_frontback);
	g_free(fb_hashside);
	
	// Convert all numbers into strings first
	if (!pshow->currset->name[0])
		sprintf(set_buf, "%i", setnum);
	else
		strcpy(set_buf, pshow->currset->name);
	update_tempo();
	tempo = pshow->currtempo->tempo;
	sprintf(tempo_buf, "%i", tempo);
	//printf("tempo = %i\n", tempo);
	if (pshow->step)
	{
		// take the counts from the next set
		sprintf(count_buf, "%i", pshow->currset->next->counts);
	}
	else
	{
		// Take the counts from this set
		sprintf(count_buf, "%i", pshow->currset->counts);
	}
	// side-to-side relations
	sprintf(perf_buf, "%i", perf_cur);
	//retr_coord(pshow->currset->coords[perf_cur], &x, &y);
	sprintf(perf_buf_x, "%g", pshow->center->x);
	sprintf(perf_buf_y, "%g", pshow->center->y);
	// Now Update entries with new data
	gtk_entry_set_text(GTK_ENTRY (entry_sets), set_buf);
	gtk_entry_set_text(GTK_ENTRY (entry_tempo), tempo_buf);
	gtk_entry_set_text(GTK_ENTRY (entry_counts), count_buf);
	gtk_entry_set_text(GTK_ENTRY (entry_perf), perf_buf);
	//gtk_entry_set_text(GTK_ENTRY (entry_perf_x), perf_buf_x);
	//gtk_entry_set_text(GTK_ENTRY (entry_perf_y), perf_buf_y);
	return 0;
}

int buildIfacegtk(void)
{
	GtkActionGroup *action_group;	// menus
	GtkUIManager *menu_manager;	// menus
	GError *error;	
	GtkWidget *menubar;		// for menus
	GtkWidget *toolbar;
	// Container Widgets
	GtkWidget *box0;		// second-level packing box (for canvas)
	GtkWidget *box1;
	GtkWidget *setbox;		// Set attributes go in this box
	GtkWidget *perfbox;		// Dot attributes go in this box
	GtkWidget *perfSSbox;
	GtkWidget *perfFBbox;
	GtkWidget *perfEnbox;
	GtkWidget *box3;
	GtkWidget *menu_box;		// First-level packing box (for menus)
	GtkWidget *media_box;		// For media buttons
	// buttons, checks, etc
	GtkWidget *button;		// buttons
	GtkWidget *check;
	GtkWidget *label;
	GtkWidget *separator;
	GtkWidget *image;
	GtkWidget *alignment;
	GtkWidget *frame;
	gchar *sbinfo;

	// Field relation buttons
	//struct gtk_ssRel sidesideBtns;
	//struct gtk_fbRel frbkBtns;

	gint tmp_pos;

	int tempo;
	char set_buf[5];
	char count_buf[5];
	char tempo_buf[5];
	char perf_buf[5];
	char perf_buf_x[5];
	char perf_buf_y[5];
	char ss_buf[20];
	char fb_buf[20];

	first_time = 1;

	// Build the menu
	static GtkActionEntry entries[] =
	{
	{ "FileMenuAction", NULL, "_File" },	// name, stock id, label
	{ "ToolsMenuAction", NULL, "_Tools"},
	{ "HelpMenuAction", NULL, "_Help"},
	//{ "RunMenuAction", NULL, "_Run" },
	{ "SetMenuAction", NULL, "_Set" },
	//{ "AddMenuAction", NULL, "_Add"},
	{ "EditMenuAction", NULL, "_Edit"},
	{ "ViewMenuAction", NULL, "_View"},
	//{ "MoveMenuAction", NULL, "_Move"},
	{ "DotMenuAction", NULL, "_Dot"},
		
	// File Menu
	{ "NewAction", GTK_STOCK_NEW,
		"_New", "<control>N",
		"Create a new file",
		G_CALLBACK (not_implemented) },
	{ "OpenAction", GTK_STOCK_OPEN,
		"_Open", "<control>O",
		"Open an existing file",
		G_CALLBACK (wrap_load_dep) },
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
	// Edit Menu
	{ "UndoProcAction", NULL,
		"_Undo", "<control>z",
		"Undo previous action",
		G_CALLBACK(do_undo_gtk) },
	{ "RedoProcAction", NULL,
		"_Redo", "<control><shift>z",
		"Redo previous undo",
		G_CALLBACK(do_redo_gtk) },
	{ "SelectAllAction", NULL,
		"_Select All", "<control>A",
		"Select All Performers",
		G_CALLBACK (select_all_gtk) },
	{ "AddPerfAction", NULL,
		"_Add Performer", NULL,
		"Add a performer to the show",
		G_CALLBACK (add_perf_gtk) },
	{ "DelPerfAction", NULL,
		"_Delete Performer", NULL,
		"Delete a performer from the show",
		G_CALLBACK (delete_perf_gtk) },
	{ "AddSetAction", NULL,
		"_Add Set", NULL,
		"Add a set to the show",
		G_CALLBACK (add_set_gtk) },
	{ "DelSetAction", NULL,
		"_Delete Set", NULL,
		"Delete a set from the show",
		G_CALLBACK (delete_set_gtk) },
	// View Menu
	{ "ZoomInAction", NULL,
		"_Zoom In", "<control>equal",
		"Zoom the field in",
		G_CALLBACK(zoom_in) },
	{ "ZoomOutAction", NULL,
		"_Zoom Out", "<control>minus",
		"Zoom the field out",
		G_CALLBACK(zoom_out) },
	{ "ZoomStandardAction", NULL,
		"_Zoom Standard", "<control>0",
		"Zoom the field to 100%",
		G_CALLBACK(zoom_standard) },
	{ "ZoomFitAction", NULL,
		"_Zoom Fit", "<control>plus",
		"Fit the field in the window",
		G_CALLBACK(zoom_fit) },
	// Dot Menu
	{ "AlignGridAction", NULL,
		"_Align to Grid", "<control>G",
		"Align selected dots to 8:5 grid",
		G_CALLBACK(dot_align_to_grid) },
	{ "MoveUpAction", NULL,
		"_Move Up", "<control>Up",
		"Move performer up 1 step",
		G_CALLBACK (move_up) },
	{ "MoveDownAction", NULL,
		"_Move Down", "<control>Down",
		"Move performer down 1 step",
		G_CALLBACK (move_down) },
	{ "MoveLeftAction", NULL,
		"_Move Left", "<control>Left",
		"Move performer Left 1 step",
		G_CALLBACK (move_left) },
	{ "MoveRightAction", NULL,
		"_Move Right", "<control>Right",
		"Move performer right 1 step",
		G_CALLBACK (move_right) },
	{ "MoveUpSmallAction", NULL,
		"_Move Up (small)", "<control><shift>Up",
		"Move performer up 0.25 steps",
		G_CALLBACK (move_up_small) },
	{ "MoveDownSmallAction", NULL,
		"_Move Down (small)", "<control><shift>Down",
		"Move performer down 0.25 steps",
		G_CALLBACK (move_down_small) },
	{ "MoveLeftSmallAction", NULL,
		"_Move Left (small)", "<control><shift>Left",
		"Move performer Left 0.25 steps",
		G_CALLBACK (move_left_small) },
	{ "MoveRightSmallAction", NULL,
		"_Move Right (small)", "<control><shift>Right",
		"Move performer right 0.25 steps",
		G_CALLBACK (move_right_small) },
	{ "NextPerfAction", NULL,
		"_Next Performer", "bracketright",
		"Highlight next performer",
		G_CALLBACK (next_perf) },
	{ "PrevPerfAction", NULL,
		"_Previous Performer", "bracketleft",
		"Highlight previous performer",
		G_CALLBACK (prev_perf) },
	{ "ExpandPerfAction", NULL,
		"_Expand Form", "<control>bracketright",
		"Expand the form around weighted center",
		G_CALLBACK (expand_form) },
	{ "ContractPerfAction", NULL,
		"_Contract Form", "<control>bracketleft",
		"Contract the form around weighted center",
		G_CALLBACK (contract_form) },
	{ "RotCWPerfAction", NULL,
		"_Rotate CW", "<control>R",
		"Rotate the form clockwise around weighted center",
		G_CALLBACK(rot_cw) },
	{ "RotCCWPerfAction", NULL,
		"_Rotate CCW", "<control>E",
		"Rotate the form counterclockwise around weighted center",
		G_CALLBACK(rot_countercw) },
	{ "RotCWSPerfAction", NULL,
		"_Rotate CW (small)", "<control><shift>R",
		"Rotate the form a little clockwise around weighted center",
		G_CALLBACK(rot_cw_small) },
	{ "RotCCWSPerfAction", NULL,
		"_Rotate CCW (small)", "<control><shift>E",
		"Rotate the form a little counterclockwise around weighted center",
		G_CALLBACK(rot_countercw_small) },
	// Set Menu
	{ "SetSetNameAction", NULL,
		"_Set setname", NULL,
		"Set current page name",
		G_CALLBACK (set_set_name_gtk) },
	{ "FirstSetAction", NULL,
		"_First Set", "Home",
		"Go to first set",
		G_CALLBACK (set_first_gtk) },
	{ "NextSetAction", NULL,
		"_Next Set", "Next",
		"Go to next set",
		G_CALLBACK(set_next_gtk) },
	{ "StepNextSetAction", NULL,
		"_Next Count", "<control>Next",
		"Go to next count",
		G_CALLBACK (set_next_count_gtk) },
	{ "StepPrevSetAction", NULL,
		"_Prev Count", "<control>Prior",
		"Go to previous count",
		G_CALLBACK (set_prev_count_gtk) },
	{ "PrevSetAction", NULL,
		"_Previous Set", "Prior",
		"Go to previous set",
		G_CALLBACK (set_prev_gtk) },
	{ "LastSetAction", NULL,
		"_Last Set", "End",
		"Go to last set",
		G_CALLBACK (set_last_gtk) },
	{ "PlayHereAction", NULL,
		"_Play", "<control>Return",
		"Play from current set",
		G_CALLBACK (queue_show) },
	{ "PlayFromStartAction", NULL,
		"_Play from Start", "<control><shift>Return",
		"Play from first set",
		G_CALLBACK (play_show_from_start) },
	{ "StopPlayAction", NULL,
		"_Stop", NULL,
		"Stop playback",
		G_CALLBACK (stop_show) },
	{ "ForceRedrawAction", NULL,
		"_Force Redraw", "F5",
		"Refresh the canvas",
		G_CALLBACK (force_redraw) },
	{ "StepsizeAction", NULL,
		"_Calculate Step-size", NULL,
		"Find the step size of selected dot",
		G_CALLBACK (not_implemented) },
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


	// Window Actions
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_add_events (window, GDK_BUTTON_PRESS_MASK);
	gtk_window_set_title(GTK_WINDOW(window), "Drill-Writer Pre-Alpha");
	gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	
	// Might need these in the future
	//gtk_widget_set_app_paintable(window, TRUE);
	//gtk_widget_set_double_buffered(window, FALSE);


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

	// set controls and media controls
	media_box = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX (box0), media_box, FALSE, FALSE, 0);

	// Set attributes (set, counts, tempo, etc)
	setbox = gtk_hbox_new (FALSE, 5);	
	/*
	setbox = gtk_hbutton_box_new();
       	gtk_button_box_set_layout(GTK_BUTTON_BOX(setbox), GTK_BUTTONBOX_START);
	gtk_box_set_spacing(GTK_BOX(setbox), 20);
	*/
	// make alignment for sets
	alignment = gtk_alignment_new(0.015,0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER(alignment), setbox);

	gtk_box_pack_start(GTK_BOX (box0), alignment, FALSE, FALSE, 0);
	//gtk_box_pack_start(GTK_BOX (box0), setbox, FALSE, FALSE, 0);

	label = gtk_label_new ("Set:");
	gtk_box_pack_start (GTK_BOX (setbox), label, FALSE, TRUE, 0);

	// Set entry
	sprintf(set_buf, "%i", setnum);
	entry_sets = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_sets), 50);
	g_signal_connect(entry_sets, "activate", G_CALLBACK (goto_set_gtk), entry_sets);
	gtk_entry_set_text (GTK_ENTRY (entry_sets), set_buf);
	tmp_pos = GTK_ENTRY (entry_sets)->text_length;
	gtk_editable_set_editable(GTK_EDITABLE(entry_sets), FALSE);
	gtk_editable_select_region (GTK_EDITABLE (entry_sets),
			0, GTK_ENTRY (entry_sets)->text_length);
	gtk_entry_set_alignment (GTK_ENTRY (entry_sets), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_sets), 4);
	gtk_box_pack_start (GTK_BOX (setbox), entry_sets, FALSE, TRUE, 0);
	gtk_widget_show(entry_sets);

	//separator = gtk_vseparator_new ();
	//gtk_box_pack_start (GTK_BOX (setbox), separator, FALSE, TRUE, 0);

	// Count entry
	label = gtk_label_new ("Counts:");
	gtk_box_pack_start (GTK_BOX (setbox), label, FALSE, TRUE, 0);

	sprintf(count_buf, "%i", pshow->currset->counts);
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
	gtk_widget_show(entry_counts);

	// Tempo box (embedded in set attribute box)
	label = gtk_label_new ("Tempo");
	gtk_box_pack_start(GTK_BOX(setbox), label, FALSE, TRUE, 0);

	// Tempo entry box
	tempo = pshow->currtempo->tempo;
	sprintf(tempo_buf, "%i", tempo);
	entry_tempo = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry_tempo), 50);
	g_signal_connect(entry_tempo, "activate", G_CALLBACK(change_tempo_gtk), entry_tempo);
	gtk_entry_set_text(GTK_ENTRY(entry_tempo), tempo_buf);
	tmp_pos = GTK_ENTRY(entry_tempo)->text_length;
	gtk_editable_select_region(GTK_EDITABLE(entry_tempo),
			0, GTK_ENTRY(entry_tempo)->text_length);
	gtk_entry_set_alignment(GTK_ENTRY(entry_tempo), 1);
	gtk_entry_set_width_chars(GTK_ENTRY(entry_tempo), 4);
	gtk_box_pack_start(GTK_BOX(setbox), entry_tempo, FALSE, TRUE, 0);


	// create field canvas scroll container
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 10);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(box0), scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show(scrolled_window);

	// get and pack canvas
	drill = gtk_drill_new();
	gtk_widget_set_size_request(drill, 801, 426);
	gtk_scrolled_window_add_with_viewport(
			GTK_SCROLLED_WINDOW(scrolled_window), drill);
	//gtk_box_pack_start(GTK_BOX (box0), drill, TRUE, TRUE, 0);
	//g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), NULL);
	// draw the field the first time
	do_field=1;

	perfbox = gtk_hbox_new (FALSE, 5);	// Dot attributes
	// make alignment for performers
	alignment = gtk_alignment_new(0.015, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER(alignment), perfbox);
	gtk_box_pack_start(GTK_BOX (box0), alignment, FALSE, FALSE, 0);



	// Media Controls (First, prev, next, last)
	box1 = gtk_hbox_new (FALSE, 0);	
	gtk_box_pack_start(GTK_BOX (media_box), box1, FALSE, FALSE, 0);


	// Performer entry box
	// put it in a frame
	frame = gtk_frame_new("performer");
	perfEnbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), perfEnbox);
	gtk_box_pack_start(GTK_BOX(perfbox), frame, FALSE, TRUE, 5);

	sprintf(perf_buf, "%i", perf_cur);
	entry_perf = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf), 5);
	g_signal_connect (entry_perf, "activate", G_CALLBACK (goto_perf), entry_perf);
	gtk_entry_set_text (GTK_ENTRY (entry_perf), perf_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf), 4);
	//gtk_box_pack_start (GTK_BOX (perfbox), entry_perf, FALSE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (perfEnbox), entry_perf, FALSE, TRUE, 5);

	separator = gtk_vseparator_new();
	gtk_box_pack_start(GTK_BOX(perfbox), separator, FALSE, TRUE, 0);
	gtk_widget_show(separator);

	// Side-to-Side
	// Field Step entry
	frame = gtk_frame_new("side-to-side");
	perfSSbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), perfSSbox);
	gtk_box_pack_start(GTK_BOX(perfbox), frame, FALSE, TRUE, 5);
	sprintf(ss_buf, "%i", 0);
	sidesideBtns.ssStepEntry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(sidesideBtns.ssStepEntry), 50);
	g_signal_connect(sidesideBtns.ssStepEntry, "activate",
		       	G_CALLBACK(change_ss_entry), sidesideBtns.ssStepEntry);
	gtk_entry_set_text(GTK_ENTRY(sidesideBtns.ssStepEntry), ss_buf);
	tmp_pos = GTK_ENTRY(sidesideBtns.ssStepEntry)->text_length;
	gtk_editable_select_region(GTK_EDITABLE(sidesideBtns.ssStepEntry),
			0, GTK_ENTRY(sidesideBtns.ssStepEntry)->text_length);
	gtk_entry_set_alignment(GTK_ENTRY(sidesideBtns.ssStepEntry), 1);
	gtk_entry_set_width_chars(GTK_ENTRY(sidesideBtns.ssStepEntry), 4);
	//gtk_box_pack_start(GTK_BOX(perfbox), sidesideBtns.ssStepEntry, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(perfSSbox), sidesideBtns.ssStepEntry, FALSE, TRUE, 0);

	// yardline relation
	sidesideBtns.ssYdRelButton = gtk_button_new_with_label ("inside");
	g_signal_connect(sidesideBtns.ssYdRelButton,
		       	"clicked", G_CALLBACK(toggle_ssYdRel), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbox), sidesideBtns.ssYdRelButton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfSSbox), sidesideBtns.ssYdRelButton, FALSE, FALSE, 0);
	gtk_widget_show(sidesideBtns.ssYdRelButton);

	// side of the field
	sidesideBtns.ssSide = gtk_button_new_with_label ("Side 1");
	g_signal_connect(sidesideBtns.ssSide,
		       	"clicked", G_CALLBACK(toggle_ssSide), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbox), sidesideBtns.ssSide, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfSSbox), sidesideBtns.ssSide, FALSE, FALSE, 0);
	gtk_widget_show(sidesideBtns.ssSide);

	// yardline
	sidesideBtns.ssYdLine = gtk_button_new_with_label ("50");
	g_signal_connect(sidesideBtns.ssYdLine,
		       	"clicked", G_CALLBACK(toggle_ssYdLine), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbox), sidesideBtns.ssYdLine, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX (perfSSbox), sidesideBtns.ssYdLine, FALSE, FALSE, 5);
	gtk_widget_show(sidesideBtns.ssYdLine);


	// another separator
	separator = gtk_vseparator_new();
	gtk_box_pack_start(GTK_BOX(perfbox), separator, FALSE, TRUE, 0);
	gtk_widget_show(separator);


	// Front-To-Back
	// frame
	frame = gtk_frame_new("front-to-back");
	perfFBbox = gtk_hbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(frame), perfFBbox);
	gtk_box_pack_start(GTK_BOX(perfbox), frame, FALSE, TRUE, 5);

	// Field Step entry
	sprintf(fb_buf, "%i", 0);
	frbkBtns.fbStepEntry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(frbkBtns.fbStepEntry), 50);
	g_signal_connect(frbkBtns.fbStepEntry, "activate",
		       	G_CALLBACK(change_fb_entry), frbkBtns.fbStepEntry);
	gtk_entry_set_text(GTK_ENTRY(frbkBtns.fbStepEntry), fb_buf);
	tmp_pos = GTK_ENTRY(frbkBtns.fbStepEntry)->text_length;
	gtk_editable_select_region(GTK_EDITABLE(frbkBtns.fbStepEntry),
			0, GTK_ENTRY(frbkBtns.fbStepEntry)->text_length);
	gtk_entry_set_alignment(GTK_ENTRY(frbkBtns.fbStepEntry), 1);
	gtk_entry_set_width_chars(GTK_ENTRY(frbkBtns.fbStepEntry), 4);
	//gtk_box_pack_start(GTK_BOX(perfbox), frbkBtns.fbStepEntry, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(perfFBbox), frbkBtns.fbStepEntry, FALSE, TRUE, 0);

	// Hash relation button
	frbkBtns.fbHashRelButton = gtk_button_new_with_label ("inside");
	g_signal_connect(frbkBtns.fbHashRelButton,
		       	"clicked", G_CALLBACK(toggle_fbHashRel), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbox), frbkBtns.fbHashRelButton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfFBbox), frbkBtns.fbHashRelButton, FALSE, FALSE, 0);
	gtk_widget_show(frbkBtns.fbHashRelButton);

	// Front/Back toggle button
	frbkBtns.fbToggleButton = gtk_button_new_with_label ("Front");
	g_signal_connect(frbkBtns.fbToggleButton,
		       	"clicked", G_CALLBACK(toggle_fbFrontBack), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbox), frbkBtns.fbToggleButton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfFBbox), frbkBtns.fbToggleButton, FALSE, FALSE, 0);
	gtk_widget_show(frbkBtns.fbToggleButton);

	// Hash/Sideline toggle button
	frbkBtns.HashSideButton = gtk_button_new_with_label ("Sideline");
	g_signal_connect(frbkBtns.HashSideButton,
		       	"clicked", G_CALLBACK(toggle_HashSide), NULL);
	//gtk_box_pack_start(GTK_BOX (perfbox), frbkBtns.HashSideButton, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX (perfFBbox), frbkBtns.HashSideButton, FALSE, FALSE, 0);
	gtk_widget_show(frbkBtns.HashSideButton);


	/*
	label = gtk_label_new ("Performer:");
	gtk_box_pack_start (GTK_BOX (perfbox), label, FALSE, TRUE, 0);
	*/

	/*


	label = gtk_label_new ("X:");
	gtk_box_pack_start (GTK_BOX (perfbox), label, FALSE, TRUE, 0);

	//sprintf(perf_buf_x, "%g", pshow->currset->coords[perf_cur]->x);
	sprintf(perf_buf_x, "%g", pshow->center->x);
	entry_perf_x = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf_x), 5);
	g_signal_connect (entry_perf_x, "activate", G_CALLBACK (xperf_change), entry_perf_x);
	gtk_entry_set_text (GTK_ENTRY (entry_perf_x), perf_buf_x);
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf_x), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf_x), 4);
	gtk_box_pack_start (GTK_BOX (perfbox), entry_perf_x, FALSE, TRUE, 0);

	label = gtk_label_new ("Y:");
	gtk_box_pack_start (GTK_BOX (perfbox), label, FALSE, TRUE, 0);

	//sprintf(perf_buf_y, "%g", pshow->currset->coords[perf_cur]->y);
	sprintf(perf_buf_y, "%g", pshow->center->y);
	entry_perf_y = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf_y), 5);
	g_signal_connect (entry_perf_y, "activate", G_CALLBACK (yperf_change), entry_perf_y);
	gtk_entry_set_text (GTK_ENTRY (entry_perf_y), perf_buf_y);
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf_y), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf_y), 4);
	gtk_box_pack_start (GTK_BOX (perfbox), entry_perf_y, FALSE, TRUE, 0);
	*/

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
	g_signal_connect(button, "clicked", G_CALLBACK(set_first_gtk), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_GOTO_FIRST, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Previous Set
	button = gtk_button_new_with_label ("Prev"); 
	g_signal_connect(button, "clicked", G_CALLBACK(set_prev_gtk), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_REWIND, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Step
	button = gtk_button_new_with_label ("Step");
	g_signal_connect(button, "clicked", G_CALLBACK(set_next_count_gtk), NULL);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	// Next Set
	button = gtk_button_new_with_label ("Next");
	g_signal_connect(button, "clicked", G_CALLBACK(set_next_gtk), NULL);//, window);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_FORWARD, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Last Set
	button = gtk_button_new_with_label ("Last");
	image = gtk_image_new_from_stock (GTK_STOCK_GOTO_LAST, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	g_signal_connect(button, "clicked", G_CALLBACK(set_last_gtk), NULL);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// status bar
	statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX (box0), statusbar, FALSE, FALSE, 0);
	context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "statusbar");
	sbinfo = g_strdup_printf("status bar");
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), GPOINTER_TO_INT(context_id), sbinfo);
	g_free(sbinfo);
	gtk_widget_show(statusbar);
	context_id = 0;

	// Show everything
	gtk_widget_show(drill);
	gtk_widget_show(box0);
	gtk_widget_show(box1);
	gtk_widget_show(setbox);
	gtk_widget_show(perfEnbox);
	gtk_widget_show(perfSSbox);
	gtk_widget_show(perfFBbox);
	gtk_widget_show(perfbox);
	//gtk_widget_show(box3);
	gtk_widget_show(media_box);
	gtk_widget_show(menu_box);
	gtk_widget_show_all (window);
	gtk_widget_grab_focus(scrolled_window);

	return 0;
}


// undo code
void do_undo_gtk(GtkWidget *widget)
{
	int done;
	int first_one = 2;
	do
	{
		done = popFromStack(pshow, &pshow->undobr, &pshow->redobr);
		if (first_one)
			first_one = first_one - 1;
	} while ((done == 0 || first_one) && pshow->undobr);
	gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	return;
}
void do_redo_gtk(GtkWidget *widget)
{
	int done;
	do
	{
		done = popFromStack(pshow, &pshow->redobr, &pshow->undobr);
	} while (done == 0);
	gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	return;
}

