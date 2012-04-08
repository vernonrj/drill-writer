// Functions changing attributes of entries go here
//#include "drill.h"
#include "d_gtk.h"
GtkWidget *entry_perf;
GtkWidget *entry_perf_x;
GtkWidget *entry_perf_y;
static void entry_toggle_editable(GtkWidget *checkbutton, GtkWidget *entry)
{
	gtk_editable_set_editable (GTK_EDITABLE (entry),
			GTK_TOGGLE_BUTTON (checkbutton)->active);
}

static void entry_toggle_visibility(GtkWidget *checkbutton, GtkWidget *entry)
{
	gtk_entry_set_visibility(GTK_ENTRY (entry), GTK_TOGGLE_BUTTON (checkbutton)->active);
}

