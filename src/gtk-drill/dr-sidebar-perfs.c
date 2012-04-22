#include "dr-sidebar-perfs.h"
#include "d_gtk.h"

static void dr_sidebar_perfs_class_init(DrSidebarPerfsClass *klass);
static void dr_sidebar_perfs_init(DrSidebarPerfs *sidebar_perfs);

struct _DrSidebarPerfsPrivate {
	GtkWidget *entry_perf;
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
	g_return_if_fail(IS_SIDEBAR_PERFS(sidebarperfs));
	GtkWidget *entry_perf;

	char perf_buf[20];

	sidebarperfs->priv = DR_SIDEBAR_PERFS_GET_PRIVATE(sidebarperfs);

	sprintf(perf_buf, "%i", perf_cur);
	entry_perf = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf), 5);
	//g_signal_connect (entry_perf, "activate", G_CALLBACK (goto_perf), entry_perf);
	gtk_entry_set_text (GTK_ENTRY (entry_perf), perf_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf), 4);
	//gtk_box_pack_start (GTK_BOX (perfbar), entry_perf, FALSE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (sidebarperfs), entry_perf, FALSE, TRUE, 5);
	gtk_widget_show(entry_perf);
	sidebarperfs->priv->entry_perf = entry_perf;
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
	DrSidebarPerfs *lsidebarperfs;
	lsidebarperfs = (DrSidebarPerfs*)sidebarperfs;

	snprintf(perf_buf, 19, "%i", perf_cur);
	gtk_entry_set_text(GTK_ENTRY(lsidebarperfs->priv->entry_perf), perf_buf);
}


