#include "drillwriter-gtk.h"
#include "dr-media.h"		// mediabar class
#include "dr-drill-class.h"
#include "dr-setbar.h"		// setbar class
#include "dr-perfbar.h"
#include "dr-sidebar.h"
#include "dr-sidebar-sets.h"
#include "dr-sidebar-groups.h"
#include "dr-group-cell.h"
#include "../structures.h"
#include "../dr_forms.h"
#include "../dr_select.h"
// TODO: free gtk widgets
/* 
 * 
 * TODO:	When in a form, endpoints are black, dynamic dots are grayed out
 * TODO:	Groups and forms will be separate entities.
 * TODO:	Groups can be nested (subgroups, subforms). Accessed on group sidebar
 * 		Forms can be linked (endpt to endpt).
 * TODO:	Handle arc forms like javascript spline example seen.
 * TODO:	Develop pango prototype for integration into program
 * TODO:	Work on open dialog to show only relevent files
 * TODO:	Gray out undo & redo when no undo/redo paths
 * TODO:	Fix redo
 *
 */

static const gchar *ui_info = 
"<ui>"
"	<menubar name='MainMenu'>"
"		<menu name='FileMenu' action='FileMenuAction'>"
"			<menuitem name='New' action='NewAction' />"
"			<menuitem name='Open' action='OpenAction' />"
"			<menuitem name='Save' action='SaveAction' />"
"			<menuitem name='Save As' action='SaveAsAction' />"
"			<menuitem name='Export to PDF' action='ExportPDFAction' />"
"			<menuitem name='Quit' action='QuitAction' />"
"			<placeholder name='FileMenuAdditions' />"
"		</menu>"
"		<menu name='EditMenu' action='EditMenuAction'>"
"			<menuitem name='UndoProc' action='UndoProcAction'/>"
"			<menuitem name='RedoProc' action='RedoProcAction'/>"
"			<menuitem name='SelectAll' action='SelectAllAction'/>"
//"			<menuitem name='AddSet' action='AddSetAction'/>"
"			<menuitem name='AddSetBefore' action='AddSetBeforeAction'/>"
"			<menuitem name='AddSetAfter' action='AddSetAfterAction'/>"
"			<menuitem name='DelSet' action='DelSetAction'/>"
"			<menuitem name='GroupAdd' action='GroupAddAction'/>"
"			<menuitem name='SelectMode' action='SelectModeAction'/>"
"			<menuitem name='AddPerf' action='AddPerfAction'/>"
"			<menuitem name='RevPerf' action='RevPerfAction'/>"
"			<menuitem name='DelPerf' action='DelPerfAction'/>"
"			<placeholder name='EditMenuAddition'/>"
"		</menu>"
"		<menu name='ViewMenu' action='ViewMenuAction'>"
"			<menuitem name='ZoomIn' action='ZoomInAction'/>"
"			<menuitem name='ZoomOut' action='ZoomOutAction'/>"
"			<menuitem name='ZoomStandard' action='ZoomStandardAction'/>"
"			<menuitem name='ZoomFit' action='ZoomFitAction'/>"
"			<menuitem name='ViewSidebar' action='ViewSidebarAction'/>"
"			<menuitem name='StepSize' action='StepSizeAction'/>"
"			<placeholder name='ViewMenuAddition'/>"
"		</menu>"
"		<menu name='DotMenu' action='DotMenuAction'>"
"			<menuitem name='AlignGrid' action='AlignGridAction'/>"
"			<menuitem name='MoveUp' action='MoveUpAction'/>"
"			<menuitem name='MoveDown' action='MoveDownAction'/>"
"			<menuitem name='MoveLeft' action='MoveLeftAction'/>"
"			<menuitem name='MoveRight' action='MoveRightAction'/>"
"			<menuitem name='MoveUpSmall' action='MoveUpSmallAction'/>"
"			<menuitem name='MoveDownSmall' action='MoveDownSmallAction'/>"
"			<menuitem name='MoveLeftSmall' action='MoveLeftSmallAction'/>"
"			<menuitem name='MoveRightSmall' action='MoveRightSmallAction'/>"
//"			<menuitem name='NextPerf' action='NextPerfAction'/>"
//"			<menuitem name='PrevPerf' action='PrevPerfAction'/>"
"			<menuitem name='ExpandPerf' action='ExpandPerfAction'/>"
"			<menuitem name='ContractPerf' action='ContractPerfAction'/>"
"			<menuitem name='RotCWPerf' action='RotCWPerfAction'/>"
"			<menuitem name='RotCCWPerf' action='RotCCWPerfAction'/>"
"			<menuitem name='RotCWSPerf' action='RotCWSPerfAction'/>"
"			<menuitem name='RotCCWSPerf' action='RotCCWSPerfAction'/>"
"			<placeholder name='DotMenuAdditioin'/>"
"		</menu>"
"		<menu name='SetMenu' action='SetMenuAction'>"
"			<menuitem name='SetSetName' action='SetSetNameAction'/>"
"			<menuitem name='NextSet' action='NextSetAction'/>"
"			<menuitem name='StepNextSet' action='StepNextSetAction'/>"
"			<menuitem name='StepPrevSet' action='StepPrevSetAction'/>"
"			<menuitem name='PrevSet' action='PrevSetAction'/>"
"			<menuitem name='FirstSet' action='FirstSetAction'/>"
"			<menuitem name='LastSet' action = 'LastSetAction'/>"
"			<menuitem name='PlayHere' action='PlayHereAction'/>"
"			<menuitem name='PlayFromStart' action='PlayFromStartAction'/>"
"			<menuitem name='StopPlay' action='StopPlayAction'/>"
"			<placeholder name='SetMenuAdditions' />"
"		</menu>"
"		<menu name='ToolsMenu' action='ToolsMenuAction'>"
"			<menuitem name='Force Redraw' action='ForceRedrawAction'/>"
"			<menuitem name='Calculate Step-size' action='StepsizeAction' />"
"			<menuitem name='Snapshot' action='SnapshotAction' />"
"			<placeholder name='ToolsMenuAdditions' />"
"		</menu>"
"		<menu name='HelpMenu' action='HelpMenuAction'>"
"			<menuitem name='About' action='AboutAction' />"
"			<placeholder name='HelpMenuAdditions' />"
"		</menu>"
"	</menubar>"
"</ui>";




static void not_implemented ()
{	// If I don't have something yet, callbacks go here
	g_print("This isn't done yet...\n");
}


/*
static void quit_action ()
{	// Quit (will have more here later)
	gtk_main_quit();
}
*/

void button_upifCh(GtkWidget *button, char *newinfo)
{
	const gchar *buffer;
	buffer = gtk_button_get_label(GTK_BUTTON(button));
	if (strcmp(buffer, newinfo))
		gtk_button_set_label(GTK_BUTTON(button), newinfo);
	return;
}

void entry_upifCh(GtkWidget *entry, char *newinfo)
{

	const char *buffer;
	buffer = gtk_entry_get_text(GTK_ENTRY(entry));
	if (strcmp(buffer, newinfo))
		gtk_entry_set_text(GTK_ENTRY(entry), newinfo);
	return;
}


/*
static gboolean msel_buttonsel(GtkWidget *widget, GdkEvent *event)
{
	// change mouse mode from menu button
	if (event->type == GDK_BUTTON_PRESS)
	{
		GdkEventButton *bevent = (GdkEventButton*)event;
		gtk_menu_popup(GTK_MENU(widget), NULL, NULL, NULL, NULL,
				bevent->button, bevent->time);
		return TRUE;
	}
	return FALSE;
}
*/


void scrollbar_scroll(GtkWidget *widget)
{
	//hscroll = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
	//vscroll = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
	double hval = gtk_adjustment_get_value(hscroll);
	double vval = gtk_adjustment_get_value(vscroll);
	canvas_move(widget, (hval-fldstate.fieldx), (vval-fldstate.fieldy));


	return;
}


void scrollbar_unblock(GtkWidget *widget)
{
	g_signal_handler_unblock(hscroll, hscroll_id);
	g_signal_handler_unblock(vscroll, vscroll_id);
}


int buildIfacegtk(void)
{
	GtkWidget *hscrollbar, *vscrollbar;
	// Build the Gtk Interface
	GtkActionGroup *action_group;	// menus
	GtkUIManager *menu_manager;	// menus
	GError *error;	
	GtkWidget *menubar;		// for menus
	// panes
	GtkWidget *hpaned;
	// Container Widgets
	GtkWidget *box0;		// second-level packing box (for canvas)
	GtkWidget *menu_box;		// First-level packing box (for menus)
	// buttons, checks, etc
	GtkWidget *alignment;
	// mouse mode (menu)
	//GtkWidget *mMode;		// mouse mode
	//GtkWidget *mMenuBar;
	//GtkWidget *item;
	// statusbar
	gchar *sbinfo;
	double hsc_width, hsc_height;
	GtkAllocation allc;


	//GtkWidget *mediabar;

	pstate.first_time = 1;

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
		G_CALLBACK (new_file_gtk) },
	{ "OpenAction", GTK_STOCK_OPEN,
		"_Open", "<control>O",
		"Open an existing file",
		G_CALLBACK (open_file_gtk) },
	{ "SaveAction", GTK_STOCK_SAVE,
		"_Save", "<control>S",
		"Save file",
		G_CALLBACK (save_file_gtk) },
	{ "SaveAsAction", NULL,
		"_Save As", NULL,
		"Save to new file",
		G_CALLBACK (save_file_as_gtk) },
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
	{ "SelectModeAction", NULL,
		"_Selectmode", "Escape",
		"Go to selection mode",
		G_CALLBACK (select_mode_gtk) },
	{ "AddSetBeforeAction", NULL,
		"_Add Set Before", NULL,
		"Add a set before the current set",
		G_CALLBACK (add_set_before_gtk) },
	{ "AddSetAfterAction", NULL,
		"_Add Set After", NULL,
		"Add a set after the current set",
		G_CALLBACK (add_set_after_gtk) },
	/*
	{ "AddSetAction", NULL,
		"_Add Set", NULL,
		"Add a set to the show",
		G_CALLBACK (add_set_gtk) },
		*/
	{ "DelSetAction", NULL,
		"_Delete Set", NULL,
		"Delete a set from the show",
		G_CALLBACK (delete_set_gtk) },
	{ "GroupAddAction", NULL,
		"_Group", NULL,
		"Add selected dots to a group",
		G_CALLBACK (add_group_gtk) },
	{ "AddPerfAction", NULL,
		"_Add Performer", NULL,
		"Add a performer to the show",
		G_CALLBACK (add_perf_gtk) },
	{ "RevPerfAction", NULL,
		"_Revert Perf", "Delete",
		"Revert a dot to previous set",
		G_CALLBACK (revert_perf_gtk) },
	{ "DelPerfAction", NULL,
		"_Delete Performer", NULL,
		"Delete a performer from the show",
		G_CALLBACK (delete_perf_gtk) },
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
	{ "ViewSidebarAction", NULL,
		"_Sidebar", "F9",
		"Toggle Sidebar",
		G_CALLBACK(dr_sidebar_toggle) },
	{ "StepSizeAction", NULL,
		"_View Stepsize", NULL,
		"View stepsize of selected dots",
		G_CALLBACK(view_stepsize_gtk) },
	// Dot Menu
	{ "AlignGridAction", NULL,
		"_Align to Grid", "<control>G",
		"Align selected dots to 8:5 grid",
		G_CALLBACK(coords_dot_align_to_grid_gtk) },
	{ "MoveUpAction", NULL,
		"_Move Up", "<control>Up",
		"Move performer up 1 step",
		G_CALLBACK (coords_move_selected_up) },
	{ "MoveDownAction", NULL,
		"_Move Down", "<control>Down",
		"Move performer down 1 step",
		G_CALLBACK (coords_move_selected_down) },
	{ "MoveLeftAction", NULL,
		"_Move Left", "<control>Left",
		"Move performer Left 1 step",
		G_CALLBACK (coords_move_selected_left) },
	{ "MoveRightAction", NULL,
		"_Move Right", "<control>Right",
		"Move performer right 1 step",
		G_CALLBACK (coords_move_selected_right) },
	{ "MoveUpSmallAction", NULL,
		"_Move Up (small)", "<control><shift>Up",
		"Move performer up 0.25 steps",
		G_CALLBACK (coords_move_selected_up_small) },
	{ "MoveDownSmallAction", NULL,
		"_Move Down (small)", "<control><shift>Down",
		"Move performer down 0.25 steps",
		G_CALLBACK (coords_move_selected_down_small) },
	{ "MoveLeftSmallAction", NULL,
		"_Move Left (small)", "<control><shift>Left",
		"Move performer Left 0.25 steps",
		G_CALLBACK (coords_move_selected_left_small) },
	{ "MoveRightSmallAction", NULL,
		"_Move Right (small)", "<control><shift>Right",
		"Move performer right 0.25 steps",
		G_CALLBACK (coords_move_selected_right_small) },
	/*
	{ "NextPerfAction", NULL,
		"_Next Performer", "bracketright",
		"Highlight next performer",
		G_CALLBACK (next_perf) },
	{ "PrevPerfAction", NULL,
		"_Previous Performer", "bracketleft",
		"Highlight previous performer",
		G_CALLBACK (prev_perf) },
		*/
	{ "ExpandPerfAction", NULL,
		"_Expand Form", "<control>bracketright",
		"Expand the form around weighted center",
		G_CALLBACK (coords_expand_form_gtk) },
	{ "ContractPerfAction", NULL,
		"_Contract Form", "<control>bracketleft",
		"Contract the form around weighted center",
		G_CALLBACK (coords_contract_form_gtk) },
	{ "RotCWPerfAction", NULL,
		"_Rotate CW", "<control>R",
		"Rotate the form clockwise around weighted center",
		G_CALLBACK(coords_rot_cw_gtk) },
	{ "RotCCWPerfAction", NULL,
		"_Rotate CCW", "<control>E",
		"Rotate the form counterclockwise around weighted center",
		G_CALLBACK(coords_rot_countercw_gtk) },
	{ "RotCWSPerfAction", NULL,
		"_Rotate CW (small)", "<control><shift>R",
		"Rotate the form a little clockwise around weighted center",
		G_CALLBACK(coords_rot_cw_small_gtk) },
	{ "RotCCWSPerfAction", NULL,
		"_Rotate CCW (small)", "<control><shift>E",
		"Rotate the form a little counterclockwise around weighted center",
		G_CALLBACK(coords_rot_countercw_small_gtk) },
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
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	// Might need these in the future
	//gtk_widget_set_app_paintable(window, TRUE);
	//gtk_widget_set_double_buffered(window, FALSE);
	// Window Signals
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Menu Stuff
	//menu_box = gtk_vbox_new (FALSE, 0);
	menu_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	action_group = gtk_action_group_new ("TestActions");
	menu_manager = gtk_ui_manager_new ();

	// Hook up window to menu_box
	gtk_container_add (GTK_CONTAINER (window), menu_box);
	gtk_action_group_add_actions (action_group, entries, n_entries, NULL);
	gtk_ui_manager_insert_action_group (menu_manager, action_group, 0);

	// Read menu UI from XML file
	error = NULL;
	/*
	gtk_ui_manager_add_ui_from_file (menu_manager, "menu-ui.xml", &error);
	if (error)
	{
		g_message ("building menus failed: %s", error->message);
		g_error_free (error);
	}
	*/
	gtk_ui_manager_add_ui_from_string(menu_manager, ui_info, -1, &error);

	// Get the menubar and toolbar and put them in vertical packing box
	menubar = gtk_ui_manager_get_widget (menu_manager, "/MainMenu");
	gtk_box_pack_start (GTK_BOX (menu_box), menubar, FALSE, FALSE, 0);

	// Make sure the accelerators work
	gtk_window_add_accel_group (GTK_WINDOW (window),
			gtk_ui_manager_get_accel_group (menu_manager));

	// Start packing canvas
	//box0 = gtk_vbox_new (FALSE, 0);
	box0 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX (menu_box), box0, TRUE, TRUE, 0);
	gtk_widget_show(menu_box);
	gtk_widget_show(box0);

	// set controls and media controls
	//media_box = gtk_vbox_new (FALSE, 0);
	//gtk_box_pack_start(GTK_BOX (box0), media_box, FALSE, FALSE, 0);
	

	//mediabar = dr_mediabar_new();
	//gtk_box_pack_start(GTK_BOX(box0), mediabar, FALSE, FALSE, 0);


	/*
	// mouse mode
	mMode = gtk_menu_new();
	item = gtk_menu_item_new_with_label("Select");
	g_signal_connect(item, "activate", G_CALLBACK(select_mode_gtk), NULL);
	gtk_widget_show(item);
	gtk_menu_shell_append(GTK_MENU_SHELL(mMode), item);

	item = gtk_menu_item_new_with_label("Add");
	g_signal_connect(item, "activate", G_CALLBACK(add_perf_gtk), NULL);
	gtk_widget_show(item);
	gtk_menu_shell_append(GTK_MENU_SHELL(mMode), item);

	mMenuBar = gtk_menu_bar_new();
	gtk_widget_show(mMode);
	gtk_widget_show(mMenuBar);

	menuButton = gtk_button_new_with_label("Select");
	g_signal_connect_swapped(menuButton, "event", G_CALLBACK(msel_buttonsel), mMode);
	gtk_box_pack_start(GTK_BOX(setbox), menuButton, TRUE, TRUE, 0);
	gtk_widget_show(menuButton);
	*/
	setbar = dr_setbar_new();
	gtk_box_pack_start(GTK_BOX(box0), setbar, FALSE, FALSE, 0);

	// make the sidebar pane
	hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(box0), hpaned, TRUE, TRUE, 0);


	sidebar = dr_sidebar_new();
	gtk_paned_add1(GTK_PANED(hpaned), sidebar);


	// get and pack canvas
	drill = gtk_drill_new();
	gtk_widget_set_size_request(drill, 460, 100);
	gtk_widget_set_hexpand(GTK_WIDGET(drill), TRUE);
	gtk_widget_set_vexpand(GTK_WIDGET(drill), TRUE);
	mybox = gtk_grid_new();
	gtk_paned_add2(GTK_PANED(hpaned), mybox);
	gtk_grid_attach(GTK_GRID(mybox), (GtkWidget*)drill, 0, 0, 2, 2);
	gtk_widget_show(mybox);
	// draw the field the first time
	do_field = 1;
	do_dots = 1;
	do_selected = 1;
	gtk_widget_show(drill);

	// create field canvas scroll container
	gtk_widget_get_allocation(drill, &allc);
	hsc_width = allc.width;
	hsc_height = allc.height;
	hscroll = (GtkAdjustment*)gtk_adjustment_new(0.0, 0.0, hsc_width, hsc_width / 10, hsc_width / 10 * 9, hsc_width);
	vscroll = (GtkAdjustment*)gtk_adjustment_new(0.0, 0.0, hsc_height, hsc_height/ 10, hsc_height / 10 * 9, hsc_height);
	hscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, hscroll);
	vscrollbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, vscroll);
	//gtk_table_attach(GTK_TABLE(mybox), vscrollbar, 1, 2, 0, 1, GTK_SHRINK, GTK_SHRINK|GTK_FILL, 0, 0);
	//gtk_table_attach(GTK_TABLE(mybox), hscrollbar, 0, 1, 1, 2, GTK_SHRINK|GTK_FILL, GTK_SHRINK, 0, 0);
	gtk_grid_attach(GTK_GRID(mybox), vscrollbar, 2, 0, 1, 2);
	gtk_grid_attach(GTK_GRID(mybox), hscrollbar, 0, 2, 2, 1);
	gtk_widget_show(vscrollbar);
	gtk_widget_show(hscrollbar);
	//vscroll->value = 10.0;
	hscroll_id = g_signal_connect(hscroll, "value_changed",
			G_CALLBACK(scrollbar_scroll), NULL);
	vscroll_id = g_signal_connect(vscroll, "value_changed",
			G_CALLBACK(scrollbar_scroll), NULL);



	
	perfbar = dr_perfbar_new();

	//perfbox = gtk_hbox_new (FALSE, 5);	// Dot attributes
	// make alignment for performers
	alignment = gtk_alignment_new(0.015, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER(alignment), perfbar);
	gtk_box_pack_start(GTK_BOX (box0), alignment, FALSE, FALSE, 0);




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
	//gtk_widget_show(box1);
	//gtk_widget_show(setbox);
	//gtk_widget_show(box3);
	//gtk_widget_show(media_box);
	gtk_widget_show_all (window);
	//gtk_widget_grab_focus(scrolled_window);
	gtk_adjustment_configure(hscroll, 0.0, 0.0, hsc_width, hsc_width / 10, hsc_width / 10 * 9, hsc_width);
	gtk_adjustment_configure(vscroll, 0.0, 0.0, hsc_height, hsc_height/ 10, hsc_height / 10 * 9, hsc_height);
	//gtk_adjustment_configure(hscroll, 0.0, 0.0, drill->allocation.width, drill->allocation.width / 10, drill->allocation.width / 10 * 9, drill->allocation.width);
	//gtk_adjustment_configure(vscroll, 0.0, 0.0, drill->allocation.height, drill->allocation.height / 10, drill->allocation.height / 10 * 9, drill->allocation.height);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);

	return 0;
}


// undo code
void do_undo_gtk(GtkWidget *widget)
{
	int done;
	int first_one = 2;
	double lwidth, lheight;
	do
	{
		done = popFromStack(pshow, &pstate.undobr, &pstate.redobr);
		if (first_one)
			first_one = first_one - 1;
	} while ((done == 0 || first_one) && pstate.undobr);
	lwidth = fldstate.width;
	lheight = fldstate.height+2*fldstate.canv_step;
	//gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	gtk_widget_queue_draw_area(window, 0, 0, lwidth, lheight);
	return;
}
void do_redo_gtk(GtkWidget *widget)
{
	int done;
	double lwidth, lheight;
	do
	{
		done = popFromStack(pshow, &pstate.redobr, &pstate.undobr);
	} while (done == 0);
	lwidth = fldstate.width;
	lheight = fldstate.height+2*fldstate.canv_step;
	gtk_widget_queue_draw_area(window, 0, 0, lwidth, lheight);
	return;
}


int main (int argc, char *argv[])
{

	drillwriter_main(argc, argv);
	///*
	// Start up gtk
	gtk_init(&argc, &argv);
	// Create gtk interface
	buildIfacegtk();
	//printf("%s\n", perf_buf_x);


	// Run Main Loop
	gtk_main();
	// */
	
	//menuIface();
	undo_destroy(&pstate.undobr, pshow);
	show_destroy(&pshow);

	return 0;
}

