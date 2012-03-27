// Functions that manipulate performers go here
#include "drill.h"
void goto_perf (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int perf_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer<perfnum)
			perf_cur=perf_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void xperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int perf_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_x));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=160)
			perf[setnum][perf_cur][0] = perf_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void yperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int perf_buffer;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_y));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=85)
			perf[setnum][perf_cur][1] = perf_buffer;
		gtk_widget_queue_draw_area(window, 0, 0, width, height+2*step);
	}
}

void add_perf (GtkWidget *widget)
{
	perfnum++;
	perf_cur=perfnum-1;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}
void delete_perf(GtkWidget *widget)
{
	int i, j;
	for (i=0; i<set_tot; i++)
	{
		for (j=perf_cur; j<perfnum; j++)
		{
			perf[i][j][0] = perf[i][j+1][0];
			perf[i][j][1] = perf[i][j+1][1];
		}
	}
	perfnum--;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}

// dot storage
int dot_construct(struct ldot_proto **dots, int size)
{
	// Build storage for dots
	int i;
	dots = (struct ldot_proto**) malloc(size * sizeof(struct ldot_proto*));
	if (dots == NULL)
	{
		// allocation error
		return -1;
	}
	for (i=0; i<size; i++)
	{
		// Initialize data
		dots[i] = (struct ldot_proto*) malloc(sizeof(struct ldot_proto));
		if (dots[i] == NULL)
		{
			// allocation error
			return -1;
		}
		dots[i]->name = (char*) malloc(1 * sizeof(char));
		dots[i]->symbol = (char*) malloc(1 * sizeof(char));
		if (dots[i]->name == NULL || dots[i]->symbol == NULL)
		{
			// allocation error
			return -1;
		}
		dots[i]->name[0] = '\0';
		dots[i]->symbol = '\0';
		dots[i]->next = 0;
	}
	return 0;
}


