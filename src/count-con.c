// Functions manipulating count structure goes here
#include "drill.h"
void goto_count (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int count_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_counts));
		count_buffer = atoi(entry_buffer);
		if (setnum+1<set_tot && count_buffer < counts[setnum+1])
			set_step=count_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}
		
void change_counts (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int count_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_counts));
		count_buffer = atoi(entry_buffer);
		if (count_buffer > 0)
			counts[setnum] = count_buffer;
	}
}

