#include "dr-sidebar-groups.h"
#include "d_gtk.h"

static void dr_sidebar_groups_class_init(DrSidebarGroupsClass *klass);
static void dr_sidebar_groups_init(DrSidebarGroups *sidebar_groups);

struct _DrSidebarGroupsPrivate {
	GtkWidget *entry_groupname;
};

G_DEFINE_TYPE (DrSidebarGroups, dr_sidebar_groups, GTK_TYPE_VBOX)

#define DR_SIDEBAR_GROUPS_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_SIDEBAR_GROUPS_TYPE, DrSidebarGroupsPrivate))

static void dr_sidebar_groups_class_init(DrSidebarGroupsClass *class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(g_object_class, sizeof(DrSidebarGroupsPrivate));
}

static void dr_sidebar_groups_init(DrSidebarGroups *sidebargroups)
{
	g_return_if_fail(IS_SIDEBAR_GROUPS(sidebargroups));
	GtkWidget *entry_groupname;

	char groupname_buf[20];

	sidebargroups->priv = DR_SIDEBAR_GROUPS_GET_PRIVATE(sidebargroups);

	sprintf(groupname_buf, "Empty");
	entry_groupname = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_groupname), 5);
	//g_signal_connect (entry_groupname, "activate", G_CALLBACK (goto_perf), entry_groupname);
	gtk_entry_set_text (GTK_ENTRY (entry_groupname), groupname_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (entry_groupname), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_groupname), 4);
	//gtk_box_pack_start (GTK_BOX (perfbar), entry_groupname, FALSE, TRUE, 5);
	gtk_box_pack_start (GTK_BOX (sidebargroups), entry_groupname, FALSE, TRUE, 5);
	gtk_widget_show(entry_groupname);
	sidebargroups->priv->entry_groupname = entry_groupname;
	return;
}

GtkWidget *dr_sidebar_groups_new(void)
{
	return GTK_WIDGET(g_object_new(dr_sidebar_groups_get_type(), NULL));
}

void dr_sidebar_groups_update(GtkWidget *sidebargroups)
{
	g_return_if_fail(IS_SIDEBAR_GROUPS(sidebargroups));
	char groupname_buf[20];
	DrSidebarGroups *lsidebargroups;
	lsidebargroups = (DrSidebarGroups*)sidebargroups;

	snprintf(groupname_buf, 19, "Empty");
	gtk_entry_set_text(GTK_ENTRY(lsidebargroups->priv->entry_groupname), groupname_buf);
}


