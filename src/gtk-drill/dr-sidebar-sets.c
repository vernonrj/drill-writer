#include "drillwriter-gtk.h"
#include "dr-sidebar-sets.h"

static void dr_sidebar_sets_class_init(DrSidebarSetsClass *klass);
static void dr_sidebar_sets_init(DrSidebarSets *sidebar_sets);

struct _DrSidebarSetsPrivate {
	GtkWidget *entry_setname;
	GtkWidget *button_addSetBefore;
	GtkWidget *button_addSetAfter;
	GtkWidget *button_delSet;
	GtkWidget *mediabar;
};

G_DEFINE_TYPE (DrSidebarSets, dr_sidebar_sets, GTK_TYPE_VBOX)

#define DR_SIDEBAR_SETS_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_SIDEBAR_SETS_TYPE, DrSidebarSetsPrivate))

static void dr_sidebar_sets_class_init(DrSidebarSetsClass *class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(g_object_class, sizeof(DrSidebarSetsPrivate));
}

static void dr_sidebar_sets_init(DrSidebarSets *sidebarsets)
{
	g_return_if_fail(IS_SIDEBAR_SETS(sidebarsets));
	GtkWidget *entry_setname;
	GtkWidget *label;
	GtkWidget *namebox;
	GtkWidget *mediabar;
	GtkWidget *box_add_set;

	char setname_buf[20];

	sidebarsets->priv = DR_SIDEBAR_SETS_GET_PRIVATE(sidebarsets);

	//namebox = gtk_hbox_new(FALSE, 0);
	namebox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (sidebarsets), namebox, FALSE, TRUE, 5);

	label = gtk_label_new("Set: ");
	gtk_box_pack_start (GTK_BOX (namebox), label, FALSE, FALSE, 5);
	gtk_widget_show(label);

	sprintf(setname_buf, "%i", pstate.setnum);
	entry_setname = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_setname), 5);
	//g_signal_connect (entry_setname, "activate", G_CALLBACK (goto_perf), entry_setname);
	gtk_entry_set_text (GTK_ENTRY (entry_setname), setname_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (entry_setname), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_setname), 4);
	//gtk_box_pack_start (GTK_BOX (perfbar), entry_setname, FALSE, TRUE, 5);
	gtk_box_pack_end (GTK_BOX (namebox), entry_setname, FALSE, FALSE, 5);
	gtk_widget_show(entry_setname);
	sidebarsets->priv->entry_setname = entry_setname;

	box_add_set = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(sidebarsets), box_add_set, FALSE, FALSE, 0);
	gtk_widget_show(box_add_set);

	// add set before button
	sidebarsets->priv->button_addSetBefore = gtk_button_new_with_label("Add Before");
	gtk_box_pack_start(GTK_BOX(box_add_set), sidebarsets->priv->button_addSetBefore, FALSE, FALSE, 0);
	g_signal_connect(sidebarsets->priv->button_addSetBefore, "clicked",
			G_CALLBACK(add_set_before_gtk), NULL);
	gtk_widget_show(sidebarsets->priv->button_addSetBefore);

	// add set after button
	sidebarsets->priv->button_addSetAfter = gtk_button_new_with_label("Add After");
	gtk_box_pack_start(GTK_BOX(box_add_set), sidebarsets->priv->button_addSetAfter, FALSE, FALSE, 0);
	g_signal_connect(sidebarsets->priv->button_addSetAfter, "clicked",
			G_CALLBACK(add_set_after_gtk), NULL);
	gtk_widget_show(sidebarsets->priv->button_addSetAfter);


	// delete set button
	sidebarsets->priv->button_delSet = gtk_button_new_with_label("Delete Set");
	gtk_box_pack_start(GTK_BOX(sidebarsets), sidebarsets->priv->button_delSet, FALSE, FALSE, 0);
	g_signal_connect(sidebarsets->priv->button_delSet, "clicked",
			G_CALLBACK(delete_set_gtk), NULL);
	gtk_widget_show(sidebarsets->priv->button_delSet);



	mediabar = dr_mediabar_new();
	gtk_box_pack_start(GTK_BOX(sidebarsets), mediabar, FALSE, FALSE, 5);
	sidebarsets->priv->mediabar = mediabar;
	return;
}

GtkWidget *dr_sidebar_sets_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_sets_get_type(), NULL));
}

void dr_sidebar_sets_update(GtkWidget *sidebarsets)
{
	g_return_if_fail(IS_SIDEBAR_SETS(sidebarsets));
	char setname_buf[20];
	DrSidebarSets *lsidebarsets;
	lsidebarsets = (DrSidebarSets*)sidebarsets;

	snprintf(setname_buf, 19, "%i", pstate.setnum);
	gtk_entry_set_text(GTK_ENTRY(lsidebarsets->priv->entry_setname), setname_buf);
}

