#include "d_gtk.h"
#include "dr-sidebar-perfs.h"

static void dr_sidebar_perfs_class_init(DrSidebarPerfsClass *klass);
static void dr_sidebar_perfs_init(DrSidebarPerfs *sidebar_perfs);

struct _DrSidebarPerfsPrivate {
	GtkWidget *entry_perf;
	GtkWidget *entry_stepsize;
	GtkWidget *entry_stepsize_max;
	GtkWidget *entry_perf_max;
};

G_DEFINE_TYPE (DrSidebarPerfs, dr_sidebar_perfs, GTK_TYPE_VBOX)

#define DR_SIDEBAR_PERFS_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_SIDEBAR_PERFS_TYPE, DrSidebarPerfsPrivate))

static void dr_sidebar_perfs_class_init(DrSidebarPerfsClass *class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(g_object_class, sizeof(DrSidebarPerfsPrivate));
}

static void dr_sidebar_perfs_init(DrSidebarPerfs *sidebarperfs)
{
	GtkWidget *entry_perf;
	GtkWidget *entry_stepsize;
	GtkWidget *entry_stepsize_max;
	GtkWidget *entry_perf_max;
	GtkWidget *pnumbox;
	GtkWidget *pstszbox;
	GtkWidget *pstszmaxbox;
	GtkWidget *perfmaxbox;
	GtkWidget *label;
	GtkWidget *frame;
	GtkWidget *framebox;

	char perf_buf[20];
	char stepsize_buf[20];
	char stepsize_max_buf[20];
	char perf_max_buf[20];
	double stepsize;
	int index;
	double stepsize_max = 0.0;

	sidebarperfs->priv = DR_SIDEBAR_PERFS_GET_PRIVATE(sidebarperfs);

	//pnumbox = gtk_hbox_new(FALSE, 0);
	pnumbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (sidebarperfs), pnumbox, FALSE, TRUE, 5);

	label = gtk_label_new("Perf: ");
	gtk_box_pack_start (GTK_BOX (pnumbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	sprintf(perf_buf, "%i", perf_cur);
	entry_perf = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf), 5);
	//g_signal_connect (entry_perf, "activate", G_CALLBACK (goto_perf), entry_perf);
	gtk_entry_set_text (GTK_ENTRY (entry_perf), perf_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf), 4);
	//gtk_box_pack_start (GTK_BOX (perfbar), entry_perf, FALSE, TRUE, 5);
	gtk_box_pack_end (GTK_BOX (pnumbox), entry_perf, FALSE, FALSE, 5);
	gtk_widget_show(entry_perf);
	sidebarperfs->priv->entry_perf = entry_perf;

	frame = gtk_frame_new("Stepsize");
	gtk_box_pack_start(GTK_BOX (sidebarperfs), frame, FALSE, TRUE, 5);
	//framebox = gtk_vbox_new(FALSE, 0);
	framebox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER(frame), framebox);

	//pstszbox = gtk_hbox_new(FALSE, 0);
	pstszbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX (framebox), pstszbox, FALSE, TRUE, 5);

	label = gtk_label_new("Avg: ");
	gtk_box_pack_start(GTK_BOX (pstszbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	stepsize = perf_average_stepsize_selected(pshow);
	if (stepsize < 0.1)
		snprintf(stepsize_buf, 19, "None");
	else
		sprintf(stepsize_buf, "%.2f:5", stepsize);
	entry_stepsize = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_stepsize), 10);
	gtk_entry_set_text (GTK_ENTRY (entry_stepsize), stepsize_buf);
	gtk_entry_set_alignment(GTK_ENTRY (entry_stepsize), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_stepsize), 10);
	gtk_box_pack_end (GTK_BOX (pstszbox), entry_stepsize, FALSE, FALSE, 5);
	gtk_widget_show(entry_stepsize);
	sidebarperfs->priv->entry_stepsize = entry_stepsize;

	//pstszmaxbox = gtk_hbox_new(FALSE, 0);
	pstszmaxbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(framebox), pstszmaxbox, FALSE, TRUE, 5);

	label = gtk_label_new("Max: ");
	gtk_box_pack_start(GTK_BOX (pstszmaxbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	index = perf_max_stepsize_selected(pshow, &stepsize_max);
	if (stepsize_max < 0.1)
		snprintf(stepsize_max_buf, 19, "None");
	else
		sprintf(stepsize_max_buf, "%.2f:5", stepsize_max);
	entry_stepsize_max = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_stepsize_max), 10);
	gtk_entry_set_text (GTK_ENTRY (entry_stepsize_max), stepsize_max_buf);
	gtk_entry_set_alignment(GTK_ENTRY (entry_stepsize_max), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_stepsize_max), 10);
	gtk_box_pack_end (GTK_BOX (pstszmaxbox), entry_stepsize_max, FALSE, FALSE, 5);
	gtk_widget_show(entry_stepsize_max);
	sidebarperfs->priv->entry_stepsize_max = entry_stepsize_max;

	//perfmaxbox = gtk_hbox_new(FALSE, 0);
	perfmaxbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(framebox), perfmaxbox, FALSE, TRUE, 5);

	label = gtk_label_new("Max (Dot): ");
	gtk_box_pack_start(GTK_BOX (perfmaxbox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	if (index == -1)
		snprintf(perf_max_buf, 19, "None");
	else
		snprintf(perf_max_buf, 19, "%i", index);
	entry_perf_max = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf_max), 10);
	gtk_entry_set_text (GTK_ENTRY (entry_perf_max), perf_max_buf);
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf_max), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf_max), 4);
	gtk_box_pack_end (GTK_BOX (perfmaxbox), entry_perf_max, FALSE, FALSE, 5);
	gtk_widget_show(entry_perf_max);
	sidebarperfs->priv->entry_perf_max = entry_perf_max;

	return;
}

GtkWidget *dr_sidebar_perfs_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_perfs_get_type(), NULL));
}

void dr_sidebar_perfs_update(GtkWidget *sidebarperfs)
{
	g_return_if_fail(IS_SIDEBAR_PERFS(sidebarperfs));
	char perf_buf[20];
	char stepsize_buf[20];
	char stepsize_max_buf[20];
	char perf_max_buf[20];
	double stepsize;
	double stepsize_max;
	int index;

	DrSidebarPerfs *lsidebarperfs;
	lsidebarperfs = (DrSidebarPerfs*)sidebarperfs;

	snprintf(perf_buf, 19, "%i", perf_cur);
	gtk_entry_set_text(GTK_ENTRY(lsidebarperfs->priv->entry_perf), perf_buf);

	stepsize = perf_average_stepsize_selected(pshow);
	if (stepsize < 0.1)
		snprintf(stepsize_buf, 19, "None");
	else
		sprintf(stepsize_buf, "%.2f:5", stepsize);
	gtk_entry_set_text(GTK_ENTRY(lsidebarperfs->priv->entry_stepsize), stepsize_buf);

	index = perf_max_stepsize_selected(pshow, &stepsize_max);
	if (stepsize_max < 0.1)
		snprintf(stepsize_max_buf, 19, "None");
	else
		snprintf(stepsize_max_buf, 19, "%.2f:5", stepsize_max);
	gtk_entry_set_text(GTK_ENTRY(lsidebarperfs->priv->entry_stepsize_max), stepsize_max_buf);

	if (index == -1)
		snprintf(perf_max_buf, 19, "None");
	else
		snprintf(perf_max_buf, 19, "%i", index);
	gtk_entry_set_text(GTK_ENTRY(lsidebarperfs->priv->entry_perf_max), perf_max_buf);



}


